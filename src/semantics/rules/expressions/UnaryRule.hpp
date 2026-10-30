#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class UnaryRule : public ExpressionRule<UnaryExpr> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(UnaryExpr& node, const Context& ctx) override {
        auto child_res = m_dispatcher.expression(node.child.get(), ctx);
        if (child_res.is_error()) {
            return ExprResult::error();
        }

        if (node.op == UnaryOp::Plus || node.op == UnaryOp::Minus) {
            if (!child_res.type->is_numeric()) {
                error(node.location, "Unary +/- requires numeric operand, got " + child_res.type->to_string());
                return ExprResult::error();
            }
            return ExprResult::value(child_res.type);
        }

        if (node.op == UnaryOp::Not) {
            if (!child_res.type->is_boolean()) {
                error(node.location, "'not' operator requires boolean operand, got " + child_res.type->to_string());
                return ExprResult::error();
            }
            return ExprResult::value(PrimitiveType::get_boolean());
        }

        return ExprResult::error();
    }
};
