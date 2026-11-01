#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"
#include "rules/common/CallChecker.hpp"

class RoutineCallStmtRule final : public StatementRule<RoutineCallStmt> {
private:
    CallChecker m_checker;

public:
    RoutineCallStmtRule(Dispatcher& dispatcher, Services& services)
        : StatementRule(dispatcher, services), m_checker(dispatcher, services) {}

    StmtResult analyze(RoutineCallStmt& node, const Context& ctx) override;
};
