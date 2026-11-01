#include "rules/expressions/RoutineCallExprRule.hpp"

ExprResult RoutineCallExprRule::analyze(RoutineCallExpr& node, const Context& ctx) {
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
