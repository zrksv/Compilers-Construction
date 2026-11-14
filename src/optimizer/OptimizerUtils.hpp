#pragma once
#include <memory>
#include <functional>
#include <optional>
#include <type_traits>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/ConstantEvaluator.hpp"
#include "../semantics/SemanticModel.hpp"

namespace optimizer_utils {

namespace detail {

template<class Derived, class Base>
auto* as(Base* node) {
    using Result = std::conditional_t<std::is_const_v<Base>, const Derived, Derived>;
    return dynamic_cast<Result*>(node);
}

template<class Node, class Visit>
void for_each_child(Node* node, Visit&& visit) {
    if (!node) return;
    if (auto* program = as<Program>(node)) {
        for (auto& declaration : program->declarations) visit(declaration);
    } else if (auto* r = as<RoutineDeclaration>(node)) {
        for (auto& parameter : r->parameters) visit(parameter);
        visit(r->return_type);
        visit(r->expression_body);
        for (auto& s : r->body) visit(s);
    } else if (auto* b = as<BinaryExpr>(node)) {
        visit(b->left);
        visit(b->right);
    } else if (auto* u = as<UnaryExpr>(node)) {
        visit(u->child);
    } else if (auto* a = as<AssignStatement>(node)) {
        visit(a->target);
        visit(a->value);
    } else if (auto* p = as<PrintStatement>(node)) {
        for (auto& item : p->to_print) visit(item);
    } else if (auto* call = as<RoutineCallStmt>(node)) {
        for (auto& arg : call->arguments) visit(arg);
    } else if (auto* call = as<RoutineCallExpr>(node)) {
        for (auto& arg : call->arguments) visit(arg);
    } else if (auto* i = as<IfStatement>(node)) {
        visit(i->condition);
        for (auto& s : i->then_body) visit(s);
        for (auto& s : i->else_body) visit(s);
    } else if (auto* w = as<WhileStatement>(node)) {
        visit(w->condition);
        for (auto& s : w->body) visit(s);
    } else if (auto* f = as<ForStatement>(node)) {
        visit(f->range_start);
        visit(f->range_end);
        for (auto& s : f->body) visit(s);
    } else if (auto* v = as<VariableDeclaration>(node)) {
        visit(v->type);
        visit(v->expression);
    } else if (auto* parameter = as<ParameterDeclaration>(node)) {
        visit(parameter->type);
    } else if (auto* t = as<TypeDeclaration>(node)) {
        visit(t->type);
    } else if (auto* at = as<ArrayTypeNode>(node)) {
        visit(at->size);
        visit(at->element_type);
    } else if (auto* rt = as<RecordTypeNode>(node)) {
        for (auto& field : rt->declarations) visit(field);
    } else if (auto* ret = as<ReturnStatement>(node)) {
        visit(ret->expression);
    } else if (auto* fa = as<FieldAccessExpr>(node)) {
        visit(fa->object);
    } else if (auto* ea = as<ElementAccessExpr>(node)) {
        visit(ea->array);
        visit(ea->index);
    }
}

} // namespace detail

std::unique_ptr<Expression> make_literal(const ConstantValue& val, Location loc);
bool is_literal(const Expression* expr);
void for_each_child(const AstNode* node, const std::function<void(const AstNode*)>& fn);
bool can_discard_evaluation(const AstNode* node, const SemanticModel& model);
std::optional<bool> constant_condition(const Expression* expr, const SemanticModel& model);
bool declares_names(const std::vector<std::unique_ptr<AstNode>>& stmts);

void forget_subtree(SemanticModel& model, const AstNode* node);
void forget_all(SemanticModel& model, const std::vector<std::unique_ptr<AstNode>>& nodes);
std::unique_ptr<Expression> replace_expr(SemanticModel& model,
                                         std::unique_ptr<Expression> old_expr,
                                         std::unique_ptr<Expression> new_expr);

} // namespace optimizer_utils
