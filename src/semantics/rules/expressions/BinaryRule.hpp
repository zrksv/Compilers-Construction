#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class BinaryRule : public ExpressionRule<BinaryExpr> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(BinaryExpr& node, const Context& ctx) override;
};
