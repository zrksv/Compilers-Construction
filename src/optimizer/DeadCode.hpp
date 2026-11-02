#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"

class DeadCodeEliminator {
public:
    bool run(Program* program);

private:
    void eliminate_in_statements(std::vector<std::unique_ptr<AstNode>>& stmts);
    bool m_changed = false;
};
