#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/SemanticModel.hpp"

class ConstantFolder {
public:
    explicit ConstantFolder(SemanticModel& model) : m_model(model) {}

    bool run(Program* program);

private:
    std::unique_ptr<Expression> optimize_expr(std::unique_ptr<Expression> expr);
    std::unique_ptr<Expression> simplify_binary(std::unique_ptr<Expression> expr);
    void optimize_statements(std::vector<std::unique_ptr<AstNode>>& stmts);

    SemanticModel& m_model;
    bool m_changed = false;
};
