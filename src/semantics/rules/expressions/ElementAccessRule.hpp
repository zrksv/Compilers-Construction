#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class ElementAccessRule : public ExpressionRule<ElementAccessExpr> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(ElementAccessExpr& node, const Context& ctx) override;
};
