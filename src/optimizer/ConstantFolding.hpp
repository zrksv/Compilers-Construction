#pragma once
#include <memory>
#include "ast/AST.hpp"
#include "../semantics/SemanticModel.hpp"

class ConstantFolder {
public:
    explicit ConstantFolder(SemanticModel& model) : m_model(model) {}

    bool run(Program* program);

private:
    std::unique_ptr<Expression> optimize_expr(std::unique_ptr<Expression> expr);
    std::unique_ptr<Expression> simplify_binary(BinaryExpr& expr);
    void optimize_children(AstNode* node);

    SemanticModel& m_model;
    bool m_changed = false;
};
