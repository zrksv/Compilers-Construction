#pragma once
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/ConstantEvaluator.hpp"

namespace optimizer_utils {

std::unique_ptr<Expression> make_literal(const ConstantValue& val, Location loc);
bool is_literal(const Expression* expr);
void for_each_child(const AstNode* node, const std::function<void(const AstNode*)>& fn);
bool is_pure(const AstNode* node);
bool contains_identifier(const AstNode* node, const std::string& name);
bool ends_with_return(const std::vector<std::unique_ptr<AstNode>>& stmts);

} // namespace optimizer_utils
