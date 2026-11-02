#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"

class UnusedVariableEliminator {
public:
    bool run(Program* program);

private:
    void eliminate_in_statements(std::vector<std::unique_ptr<AstNode>>& stmts, bool in_routine);
    bool m_changed = false;
};
