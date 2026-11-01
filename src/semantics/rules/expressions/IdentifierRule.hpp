#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class IdentifierRule : public ExpressionRule<Identifier> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(Identifier& node, const Context& ctx) override;
};
