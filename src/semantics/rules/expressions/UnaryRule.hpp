#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class UnaryRule : public ExpressionRule<UnaryExpr> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(UnaryExpr& node, const Context& ctx) override;
};
