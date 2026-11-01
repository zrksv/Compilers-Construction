#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class IntLiteralRule : public ExpressionRule<IntLiteral> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(IntLiteral&, const Context&) override;
};

class RealLiteralRule : public ExpressionRule<RealLiteral> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(RealLiteral&, const Context&) override;
};

class BoolLiteralRule : public ExpressionRule<BoolLiteral> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(BoolLiteral&, const Context&) override;
};
