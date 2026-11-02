#include "OptimizerUtils.hpp"
#include <cmath>
#include <cstdint>

namespace optimizer_utils {

std::unique_ptr<Expression> make_literal(const ConstantValue& val, Location loc) {
    if (val.kind == ConstantKind::Integer) return std::make_unique<IntLiteral>(static_cast<int>(val.int_val), loc);
    if (val.kind == ConstantKind::Real) return std::make_unique<RealLiteral>(val.real_val, loc);
    if (val.kind == ConstantKind::Boolean) return std::make_unique<BoolLiteral>(val.bool_val, loc);
    return nullptr;
}

bool is_literal(const Expression* expr) {
    return dynamic_cast<const IntLiteral*>(expr) ||
           dynamic_cast<const RealLiteral*>(expr) ||
           dynamic_cast<const BoolLiteral*>(expr);
}

void for_each_child(const AstNode* node, const std::function<void(const AstNode*)>& fn) {
    if (!node) return;
    if (const auto* r = dynamic_cast<const RoutineDeclaration*>(node)) {
        if (r->expression_body) fn(r->expression_body.get());
        for (const auto& s : r->body) fn(s.get());
    } else if (const auto* b = dynamic_cast<const BinaryExpr*>(node)) {
        fn(b->left.get()); fn(b->right.get());
    } else if (const auto* u = dynamic_cast<const UnaryExpr*>(node)) {
        fn(u->child.get());
    } else if (const auto* a = dynamic_cast<const AssignStatement*>(node)) {
        fn(a->target.get()); fn(a->value.get());
    } else if (const auto* p = dynamic_cast<const PrintStatement*>(node)) {
        for (const auto& item : p->to_print) fn(item.get());
    } else if (const auto* c = dynamic_cast<const RoutineCallStmt*>(node)) {
        for (const auto& arg : c->arguments) fn(arg.get());
    } else if (const auto* c = dynamic_cast<const RoutineCallExpr*>(node)) {
        for (const auto& arg : c->arguments) fn(arg.get());
    } else if (const auto* i = dynamic_cast<const IfStatement*>(node)) {
        fn(i->condition.get());
        for (const auto& s : i->then_body) fn(s.get());
        for (const auto& s : i->else_body) fn(s.get());
    } else if (const auto* w = dynamic_cast<const WhileStatement*>(node)) {
        fn(w->condition.get());
        for (const auto& s : w->body) fn(s.get());
    } else if (const auto* f = dynamic_cast<const ForStatement*>(node)) {
        if (f->range_start) fn(f->range_start.get());
        if (f->range_end) fn(f->range_end.get());
        for (const auto& s : f->body) fn(s.get());
    } else if (const auto* v = dynamic_cast<const VariableDeclaration*>(node)) {
        if (v->type) fn(v->type.get());
        if (v->expression) fn(v->expression.get());
    } else if (const auto* t = dynamic_cast<const TypeDeclaration*>(node)) {
        if (t->type) fn(t->type.get());
    } else if (const auto* at = dynamic_cast<const ArrayTypeNode*>(node)) {
        if (at->size) fn(at->size.get());
        if (at->element_type) fn(at->element_type.get());
    } else if (const auto* rt = dynamic_cast<const RecordTypeNode*>(node)) {
        for (const auto& field : rt->declarations) fn(field.get());
    } else if (const auto* ret = dynamic_cast<const ReturnStatement*>(node)) {
        if (ret->expression) fn(ret->expression.get());
    } else if (const auto* fa = dynamic_cast<const FieldAccessExpr*>(node)) {
        fn(fa->object.get());
    } else if (const auto* ea = dynamic_cast<const ElementAccessExpr*>(node)) {
        fn(ea->array.get()); fn(ea->index.get());
    }
}

static bool conversion_is_safe(const Expression* expr, const SemanticModel& model) {
    ConversionKind kind = model.conversion_of(expr);
    if (kind == ConversionKind::IntegerToBoolean) {
        auto val = ConstantEvaluator::evaluate(expr);
        return val.kind == ConstantKind::Integer && (val.int_val == 0 || val.int_val == 1);
    }
    if (kind == ConversionKind::RealToInteger) {
        auto val = ConstantEvaluator::evaluate(expr);
        if (val.kind == ConstantKind::Integer) return true;
        if (val.kind != ConstantKind::Real) return false;
        double rounded = std::round(val.real_val);
        return rounded >= INT32_MIN && rounded <= INT32_MAX;
    }
    return true;
}

static bool index_is_safe(const ElementAccessExpr* access, const SemanticModel& model) {
    auto idx = ConstantEvaluator::evaluate(access->index.get());
    if (idx.kind != ConstantKind::Integer) return false;
    TypeRef array_type = model.type_of(access->array.get());
    if (!array_type || !array_type->is_array()) return false;
    auto arr = std::static_pointer_cast<ArrayType>(array_type->unalias());
    return arr->size.has_value() && idx.int_val >= 1 &&
           idx.int_val <= static_cast<int64_t>(arr->size.value());
}

static bool division_is_safe(const BinaryExpr* bin, const SemanticModel& model) {
    TypeRef result = model.type_of(bin);
    if (!result) return false;
    if (result->is_real()) return true;
    auto divisor = ConstantEvaluator::evaluate(bin->right.get());
    return divisor.kind == ConstantKind::Integer && divisor.int_val != 0;
}

bool is_pure(const AstNode* node, const SemanticModel& model) {
    if (!node) return true;
    if (dynamic_cast<const RoutineCallExpr*>(node) || dynamic_cast<const RoutineCallStmt*>(node)) return false;
    if (const auto* expr = dynamic_cast<const Expression*>(node)) {
        if (!conversion_is_safe(expr, model)) return false;
    }
    if (const auto* ea = dynamic_cast<const ElementAccessExpr*>(node)) {
        if (!index_is_safe(ea, model)) return false;
    }
    if (const auto* b = dynamic_cast<const BinaryExpr*>(node)) {
        if ((b->op == BinOp::Div || b->op == BinOp::Mod) && !division_is_safe(b, model)) return false;
    }
    bool pure = true;
    for_each_child(node, [&](const AstNode* c) { if (pure && !is_pure(c, model)) pure = false; });
    return pure;
}

bool creation_is_pure(const TypeNode* type, const SemanticModel& model) {
    if (!type || dynamic_cast<const PrimitiveTypeNode*>(type)) return true;
    if (const auto* arr = dynamic_cast<const ArrayTypeNode*>(type)) {
        return creation_is_pure(arr->element_type.get(), model);
    }
    if (const auto* rec = dynamic_cast<const RecordTypeNode*>(type)) {
        for (const auto& field : rec->declarations) {
            if (!creation_is_pure(field->type.get(), model) || !is_pure(field->expression.get(), model)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool contains_identifier(const AstNode* node, const std::string& name) {
    if (!node) return false;
    if (const auto* id = dynamic_cast<const Identifier*>(node)) return id->name == name;
    bool found = false;
    for_each_child(node, [&](const AstNode* c) { if (!found && contains_identifier(c, name)) found = true; });
    return found;
}

bool ends_with_return(const std::vector<std::unique_ptr<AstNode>>& stmts) {
    if (stmts.empty()) return false;
    const auto* last = stmts.back().get();
    if (dynamic_cast<const ReturnStatement*>(last)) return true;
    if (const auto* ifs = dynamic_cast<const IfStatement*>(last)) {
        return ends_with_return(ifs->then_body) && ends_with_return(ifs->else_body);
    }
    return false;
}

bool declares_names(const std::vector<std::unique_ptr<AstNode>>& stmts) {
    for (const auto& s : stmts) {
        if (dynamic_cast<const VariableDeclaration*>(s.get()) || dynamic_cast<const TypeDeclaration*>(s.get())) {
            return true;
        }
    }
    return false;
}

bool same_type(const TypeRef& a, const TypeRef& b) {
    return a && b && a->unalias()->equals(*b->unalias());
}

void forget_subtree(SemanticModel& model, const AstNode* node) {
    if (!node) return;
    for_each_child(node, [&](const AstNode* c) { forget_subtree(model, c); });
    model.forget(node);
}

void forget_all(SemanticModel& model, const std::vector<std::unique_ptr<AstNode>>& nodes) {
    for (const auto& n : nodes) forget_subtree(model, n.get());
}

std::unique_ptr<Expression> replace_expr(SemanticModel& model,
                                         std::unique_ptr<Expression> old_expr,
                                         std::unique_ptr<Expression> new_expr) {
    if (!model.type_of(new_expr.get())) {
        model.set_type(new_expr.get(), model.type_of(old_expr.get()));
    }
    model.set_conversion(new_expr.get(), model.conversion_of(old_expr.get()));
    forget_subtree(model, old_expr.get());
    return new_expr;
}

} // namespace optimizer_utils
