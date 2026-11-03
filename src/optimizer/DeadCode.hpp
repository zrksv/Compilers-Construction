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
    void eliminate_in_statements(std::vector<std::unique_ptr<AstNode>>& stmts);
    void drop_rest(const std::vector<std::unique_ptr<AstNode>>& stmts, size_t from);
    SemanticModel& m_model;
    bool m_changed = false;
};
