#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"
#include "rules/common/CallChecker.hpp"

class RoutineCallExprRule : public ExpressionRule<RoutineCallExpr> {
private:
    CallChecker m_checker;

public:
    RoutineCallExprRule(Dispatcher& dispatcher, Services& services)
        : ExpressionRule(dispatcher, services), m_checker(dispatcher, services) {}

    ExprResult analyze(RoutineCallExpr& node, const Context& ctx) override;
};
