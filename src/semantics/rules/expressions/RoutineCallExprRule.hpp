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

    ExprResult analyze(RoutineCallExpr& node, const Context& ctx) override {
        auto* routine = m_checker.check(&node, node.routine_name, node.arguments, ctx);
        if (!routine) {
            return ExprResult::error();
        }

        if (routine->is_procedure()) {
            error(node.location, "Procedure '" + node.routine_name +
                  "' does not return a value and cannot be used in an expression");
            return ExprResult::error();
        }

        return ExprResult::value(routine->return_type);
    }
};
