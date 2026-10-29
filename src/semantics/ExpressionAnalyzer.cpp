#include "ExpressionAnalyzer.hpp"

bool ExpressionAnalyzer::can_assign(const std::shared_ptr<Type>& target, const std::shared_ptr<Type>& source) {
    if (!target || !source) return false;
    if (target->is_error() || source->is_error()) return true;

    auto t = target->unalias();
    auto s = source->unalias();

    if (t->equals(*s)) return true;

    if (t->is_integer()) {
        return s->is_integer() || s->is_real() || s->is_boolean();
    }

    if (t->is_real()) {
        return s->is_real() || s->is_integer() || s->is_boolean();
    }

    if (t->is_boolean()) {
        if (s->is_real()) return false;
        return s->is_boolean() || s->is_integer();
    }

    return t->equals(*s);
}

std::shared_ptr<Type> ExpressionAnalyzer::analyze(Expression* expr) {
    if (!expr) return PrimitiveType::get_error();

    if (dynamic_cast<IntLiteral*>(expr))  return PrimitiveType::get_integer();
    if (dynamic_cast<RealLiteral*>(expr)) return PrimitiveType::get_real();
    if (dynamic_cast<BoolLiteral*>(expr)) return PrimitiveType::get_boolean();

    if (auto* id = dynamic_cast<Identifier*>(expr)) {
        auto* sym = m_scope->lookup_variable(id->name);
        if (!sym) {
            report_error(id->location, "Variable '" + id->name + "' is not declared");
            return PrimitiveType::get_error();
        }
        return sym->type;
    }

    if (auto* un = dynamic_cast<UnaryExpr*>(expr)) {
        auto child_type = analyze(un->child.get());
        if (child_type->is_error()) return PrimitiveType::get_error();

        if (un->op == UnaryOp::Plus || un->op == UnaryOp::Minus) {
            if (!child_type->is_numeric()) {
                report_error(un->location, "Unary +/- requires numeric operand, got " + child_type->to_string());
                return PrimitiveType::get_error();
            }
            return child_type;
        }
        if (un->op == UnaryOp::Not) {
            if (!child_type->is_boolean()) {
                report_error(un->location, "'not' operator requires boolean operand, got " + child_type->to_string());
                return PrimitiveType::get_error();
            }
            return PrimitiveType::get_boolean();
        }
    }

    if (auto* bin = dynamic_cast<BinaryExpr*>(expr)) {
        auto left_t = analyze(bin->left.get());
        auto right_t = analyze(bin->right.get());
        if (left_t->is_error() || right_t->is_error()) return PrimitiveType::get_error();

        if (bin->op == BinOp::Plus || bin->op == BinOp::Minus ||
            bin->op == BinOp::Mul  || bin->op == BinOp::Div) {
            if (!left_t->is_numeric() || !right_t->is_numeric()) {
                report_error(bin->location, "Arithmetic operator requires numeric operands");
                return PrimitiveType::get_error();
            }
            if (left_t->is_real() || right_t->is_real()) {
                return PrimitiveType::get_real();
            }
            return PrimitiveType::get_integer();
        }

        if (bin->op == BinOp::Mod) {
            if (!left_t->is_integer() || !right_t->is_integer()) {
                report_error(bin->location, "Operator '%' requires integer operands");
                return PrimitiveType::get_error();
            }
            return PrimitiveType::get_integer();
        }

        if (bin->op == BinOp::And || bin->op == BinOp::Or || bin->op == BinOp::Xor) {
            if (!left_t->is_boolean() || !right_t->is_boolean()) {
                report_error(bin->location, "Logical operator requires boolean operands");
                return PrimitiveType::get_error();
            }
            return PrimitiveType::get_boolean();
        }

        if (bin->op == BinOp::Less || bin->op == BinOp::LessEqual ||
            bin->op == BinOp::Greater || bin->op == BinOp::GreaterEqual) {
            if (!left_t->is_numeric() || !right_t->is_numeric()) {
                report_error(bin->location, "Relational operator requires numeric operands");
                return PrimitiveType::get_error();
            }
            return PrimitiveType::get_boolean();
        }

        if (bin->op == BinOp::Equal || bin->op == BinOp::NotEqual) {
            if (!can_assign(left_t, right_t) && !can_assign(right_t, left_t)) {
                report_error(bin->location, "Cannot compare incompatible types " + left_t->to_string() + " and " + right_t->to_string());
                return PrimitiveType::get_error();
            }
            return PrimitiveType::get_boolean();
        }
    }

    if (auto* fa = dynamic_cast<FieldAccessExpr*>(expr)) {
        auto obj_type = analyze(fa->object.get());
        if (obj_type->is_error()) return PrimitiveType::get_error();

        auto u = obj_type->unalias();

        if (u->is_array() && fa->field_name == "size") {
            return PrimitiveType::get_integer();
        }

        if (u->is_record()) {
            auto rec = std::static_pointer_cast<RecordType>(u);
            const auto* field = rec->find_field(fa->field_name);
            if (!field) {
                report_error(fa->location, "Record '" + rec->name + "' has no field named '" + fa->field_name + "'");
                return PrimitiveType::get_error();
            }
            return field->type;
        }

        report_error(fa->location, "Cannot access field on non-record type: " + obj_type->to_string());
        return PrimitiveType::get_error();
    }

    if (auto* ea = dynamic_cast<ElementAccessExpr*>(expr)) {
        auto arr_type = analyze(ea->array.get());
        auto idx_type = analyze(ea->index.get());
        if (arr_type->is_error() || idx_type->is_error()) return PrimitiveType::get_error();

        auto u = arr_type->unalias();
        if (!u->is_array()) {
            report_error(ea->location, "Indexing target must be an array, got " + arr_type->to_string());
            return PrimitiveType::get_error();
        }

        if (!idx_type->is_integer()) {
            report_error(ea->location, "Array index must be an integer, got " + idx_type->to_string());
            return PrimitiveType::get_error();
        }

        auto arr = std::static_pointer_cast<ArrayType>(u);
        return arr->element_type;
    }

    if (auto* call = dynamic_cast<RoutineCallExpr*>(expr)) {
        auto* routine = m_scope->lookup_routine(call->routine_name);
        if (!routine) {
            report_error(call->location, "Routine '" + call->routine_name + "' is not declared");
            return PrimitiveType::get_error();
        }

        if (routine->is_procedure()) {
            report_error(call->location, "Procedure '" + call->routine_name + "' does not return a value and cannot be used in an expression");
            return PrimitiveType::get_error();
        }

        if (call->arguments.size() != routine->parameters.size()) {
            report_error(call->location, "Routine '" + call->routine_name + "' expects " +
                                         std::to_string(routine->parameters.size()) + " argument(s), got " +
                                         std::to_string(call->arguments.size()));
            return routine->return_type;
        }

        for (size_t i = 0; i < call->arguments.size(); ++i) {
            auto arg_type = analyze(call->arguments[i].get());
            auto param_type = routine->parameters[i].type;
            if (!can_assign(param_type, arg_type)) {
                report_error(call->arguments[i]->location,
                             "Argument " + std::to_string(i + 1) + " expects type " +
                             param_type->to_string() + ", got " + arg_type->to_string());
            }
        }

        return routine->return_type;
    }

    report_error(expr->location, "Unknown expression node in semantic analysis");
    return PrimitiveType::get_error();
}