#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class IntLiteralRule : public ExpressionRule<IntLiteral> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(IntLiteral&, const Context&) override {
        return ExprResult::value(PrimitiveType::get_integer());
    }
};

class RealLiteralRule : public ExpressionRule<RealLiteral> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(RealLiteral&, const Context&) override {
        return ExprResult::value(PrimitiveType::get_real());
    }
};

class BoolLiteralRule : public ExpressionRule<BoolLiteral> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(BoolLiteral&, const Context&) override {
        return ExprResult::value(PrimitiveType::get_boolean());
    }
};
