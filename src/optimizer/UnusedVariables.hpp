#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/SemanticModel.hpp"

class UnusedVariableEliminator {
public:
    explicit UnusedVariableEliminator(SemanticModel& model) : m_model(model) {}

    bool run(Program* program);

private:
    void eliminate_in_statements(std::vector<std::unique_ptr<AstNode>>& stmts, bool in_routine);
    SemanticModel& m_model;
    bool m_changed = false;
};
