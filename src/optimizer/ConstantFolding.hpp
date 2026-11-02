#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"

class ConstantFolder {
public:
    bool run(Program* program);

private:
    std::unique_ptr<Expression> optimize_expr(std::unique_ptr<Expression> expr);
    std::unique_ptr<Expression> simplify_binary(std::unique_ptr<Expression> expr);
    void optimize_statements(std::vector<std::unique_ptr<AstNode>>& stmts);

    bool m_changed = false;
};
