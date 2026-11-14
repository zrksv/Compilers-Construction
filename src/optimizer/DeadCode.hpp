#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/SemanticModel.hpp"

class DeadCodeEliminator {
public:
    explicit DeadCodeEliminator(SemanticModel& model) : m_model(model) {}

    bool run(Program* program);

private:
    bool eliminate_in_statements(std::vector<std::unique_ptr<AstNode>>& statements);
    bool eliminate_in_statement(AstNode* statement);
    SemanticModel& m_model;
    bool m_changed = false;
};
