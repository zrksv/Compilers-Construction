#include "rules/statements/RoutineCallStmtRule.hpp"

StmtResult RoutineCallStmtRule::analyze(RoutineCallStmt& node, const Context& ctx) {
    m_checker.check(&node, node.routine_name, node.arguments, ctx);
    return StmtResult{};
}
