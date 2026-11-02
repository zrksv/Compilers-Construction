#pragma once
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/ConstantEvaluator.hpp"
#include "../semantics/SemanticModel.hpp"

namespace optimizer_utils {

std::unique_ptr<Expression> make_literal(const ConstantValue& val, Location loc);
bool is_literal(const Expression* expr);
void for_each_child(const AstNode* node, const std::function<void(const AstNode*)>& fn);
bool is_pure(const AstNode* node, const SemanticModel& model);
bool creation_is_pure(const TypeNode* type, const SemanticModel& model);
bool contains_identifier(const AstNode* node, const std::string& name);
bool ends_with_return(const std::vector<std::unique_ptr<AstNode>>& stmts);
bool declares_names(const std::vector<std::unique_ptr<AstNode>>& stmts);
bool same_type(const TypeRef& a, const TypeRef& b);

void forget_subtree(SemanticModel& model, const AstNode* node);
void forget_all(SemanticModel& model, const std::vector<std::unique_ptr<AstNode>>& nodes);
std::unique_ptr<Expression> replace_expr(SemanticModel& model,
                                         std::unique_ptr<Expression> old_expr,
                                         std::unique_ptr<Expression> new_expr);

} // namespace optimizer_utils
