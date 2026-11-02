#include "ConstantFolding.hpp"
#include "OptimizerUtils.hpp"
#include "../semantics/ConstantEvaluator.hpp"

bool ConstantFolder::run(Program* program) {
    if (!program) return false;
    m_changed = false;
    optimize_statements(program->declarations);
    return m_changed;
}

std::unique_ptr<Expression> ConstantFolder::optimize_expr(std::unique_ptr<Expression> expr) {
    if (!expr) return nullptr;

    auto val = ConstantEvaluator::evaluate(expr.get());
    if (val.has_value() && !optimizer_utils::is_literal(expr.get())) {
        m_changed = true;
        return optimizer_utils::make_literal(val, expr->location);
    }

    if (auto* b = dynamic_cast<BinaryExpr*>(expr.get())) {
        b->left = optimize_expr(std::move(b->left));
        b->right = optimize_expr(std::move(b->right));
        return simplify_binary(std::move(expr));
    }
    if (auto* u = dynamic_cast<UnaryExpr*>(expr.get())) {
        u->child = optimize_expr(std::move(u->child));
        return expr;
    }
    if (auto* ea = dynamic_cast<ElementAccessExpr*>(expr.get())) {
        ea->array = optimize_expr(std::move(ea->array));
        ea->index = optimize_expr(std::move(ea->index));
        return expr;
    }
    if (auto* fa = dynamic_cast<FieldAccessExpr*>(expr.get())) {
        fa->object = optimize_expr(std::move(fa->object));
        return expr;
    }
    if (auto* c = dynamic_cast<RoutineCallExpr*>(expr.get())) {
        for (auto& a : c->arguments) a = optimize_expr(std::move(a));
        return expr;
    }
    return expr;
}

std::unique_ptr<Expression> ConstantFolder::simplify_binary(std::unique_ptr<Expression> expr) {
    auto* bin = dynamic_cast<BinaryExpr*>(expr.get());
    if (!bin) return expr;

    auto val = ConstantEvaluator::evaluate(bin);
    if (val.has_value()) {
        m_changed = true;
        return optimizer_utils::make_literal(val, bin->location);
    }

    auto L = ConstantEvaluator::evaluate(bin->left.get());
    auto R = ConstantEvaluator::evaluate(bin->right.get());

    bool L_zero = L.has_value() && L.is_zero();
    bool R_zero = R.has_value() && R.is_zero();
    bool L_one = L.has_value() && L.kind == ConstantKind::Integer && L.int_val == 1;
    bool R_one = R.has_value() && R.kind == ConstantKind::Integer && R.int_val == 1;
    bool L_true = L.has_value() && L.kind == ConstantKind::Boolean && L.bool_val;
    bool R_true = R.has_value() && R.kind == ConstantKind::Boolean && R.bool_val;
    bool L_false = L.has_value() && L.kind == ConstantKind::Boolean && !L.bool_val;
    bool R_false = R.has_value() && R.kind == ConstantKind::Boolean && !R.bool_val;

    if ((bin->op == BinOp::Plus && R_zero) || (bin->op == BinOp::Minus && R_zero)) {
        m_changed = true; return std::move(bin->left);
    }
    if (bin->op == BinOp::Plus && L_zero) {
        m_changed = true; return std::move(bin->right);
    }
    if ((bin->op == BinOp::Mul || bin->op == BinOp::Div) && R_one) {
        m_changed = true; return std::move(bin->left);
    }
    if (bin->op == BinOp::Mul && L_one) {
        m_changed = true; return std::move(bin->right);
    }

    if (bin->op == BinOp::Mul && ((R_zero && optimizer_utils::is_pure(bin->left.get())) ||
                                  (L_zero && optimizer_utils::is_pure(bin->right.get())))) {
        m_changed = true; return std::make_unique<IntLiteral>(0, bin->location);
    }

    if (bin->op == BinOp::And) {
        if (R_true) { m_changed = true; return std::move(bin->left); }
        if (L_true) { m_changed = true; return std::move(bin->right); }
        if ((R_false && optimizer_utils::is_pure(bin->left.get())) ||
            (L_false && optimizer_utils::is_pure(bin->right.get()))) {
            m_changed = true; return std::make_unique<BoolLiteral>(false, bin->location);
        }
    }
    if (bin->op == BinOp::Or) {
        if (R_false) { m_changed = true; return std::move(bin->left); }
        if (L_false) { m_changed = true; return std::move(bin->right); }
        if ((R_true && optimizer_utils::is_pure(bin->left.get())) ||
            (L_true && optimizer_utils::is_pure(bin->right.get()))) {
            m_changed = true; return std::make_unique<BoolLiteral>(true, bin->location);
        }
    }
    return expr;
}

void ConstantFolder::optimize_statements(std::vector<std::unique_ptr<AstNode>>& stmts) {
    for (auto& stmt : stmts) {
        if (!stmt) continue;

        if (auto* r = dynamic_cast<RoutineDeclaration*>(stmt.get())) {
            if (r->expression_body) r->expression_body = optimize_expr(std::move(r->expression_body));
            else optimize_statements(r->body);
        } else if (auto* v = dynamic_cast<VariableDeclaration*>(stmt.get())) {
            if (v->expression) v->expression = optimize_expr(std::move(v->expression));
        } else if (auto* a = dynamic_cast<AssignStatement*>(stmt.get())) {
            a->target = optimize_expr(std::move(a->target));
            a->value = optimize_expr(std::move(a->value));
        } else if (auto* p = dynamic_cast<PrintStatement*>(stmt.get())) {
            for (auto& item : p->to_print) item = optimize_expr(std::move(item));
        } else if (auto* c = dynamic_cast<RoutineCallStmt*>(stmt.get())) {
            for (auto& arg : c->arguments) arg = optimize_expr(std::move(arg));
        } else if (auto* ret = dynamic_cast<ReturnStatement*>(stmt.get())) {
            if (ret->expression) ret->expression = optimize_expr(std::move(ret->expression));
        } else if (auto* ifs = dynamic_cast<IfStatement*>(stmt.get())) {
            ifs->condition = optimize_expr(std::move(ifs->condition));
            optimize_statements(ifs->then_body);
            optimize_statements(ifs->else_body);
        } else if (auto* whl = dynamic_cast<WhileStatement*>(stmt.get())) {
            whl->condition = optimize_expr(std::move(whl->condition));
            optimize_statements(whl->body);
        } else if (auto* f = dynamic_cast<ForStatement*>(stmt.get())) {
            if (f->range_start) f->range_start = optimize_expr(std::move(f->range_start));
            if (f->range_end) f->range_end = optimize_expr(std::move(f->range_end));
            optimize_statements(f->body);
        }
    }
}
