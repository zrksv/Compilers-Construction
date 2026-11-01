#pragma once
#include <vector>
#include <memory>
#include "Context.hpp"
#include "Results.hpp"
#include "ast/AST.hpp"

class Dispatcher {
public:
    virtual ~Dispatcher() = default;

    virtual ExprResult expression(Expression* node, const Context& ctx) = 0;
    virtual TypeRef type(TypeNode* node, const Context& ctx) = 0;
    virtual StmtResult statement(AstNode* node, const Context& ctx) = 0;
    virtual StmtResult statement_list(const std::vector<std::unique_ptr<AstNode>>& nodes, const Context& ctx) = 0;
};
