#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"

class BranchPruner {
public:
    bool run(Program* program);

private:
    void prune_statements(std::vector<std::unique_ptr<AstNode>>& stmts);
    bool m_changed = false;
};
