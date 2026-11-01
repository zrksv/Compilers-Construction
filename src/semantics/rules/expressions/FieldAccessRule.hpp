#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class FieldAccessRule : public ExpressionRule<FieldAccessExpr> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(FieldAccessExpr& node, const Context& ctx) override;
};
