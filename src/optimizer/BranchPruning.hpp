#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/SemanticModel.hpp"

class BranchPruner {
public:
    explicit BranchPruner(SemanticModel& model) : m_model(model) {}

    bool run(Program* program);

private:
    void prune_statements(std::vector<std::unique_ptr<AstNode>>& stmts);
    SemanticModel& m_model;
    bool m_changed = false;
};
