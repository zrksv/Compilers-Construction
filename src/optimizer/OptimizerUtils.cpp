#include "OptimizerUtils.hpp"

namespace optimizer_utils {

std::unique_ptr<Expression> make_literal(const ConstantValue& val, Location loc) {
    if (val.kind == ConstantKind::Integer) return std::make_unique<IntLiteral>(static_cast<int>(val.int_val), loc);
    if (val.kind == ConstantKind::Real) return std::make_unique<RealLiteral>(val.real_val, loc);
    if (val.kind == ConstantKind::Boolean) return std::make_unique<BoolLiteral>(val.bool_val, loc);
    return nullptr;
}

bool is_literal(const Expression* expr) {
    return dynamic_cast<const IntLiteral*>(expr) ||
           dynamic_cast<const RealLiteral*>(expr) ||
           dynamic_cast<const BoolLiteral*>(expr);
}

void for_each_child(const AstNode* node, const std::function<void(const AstNode*)>& fn) {
    if (!node) return;
    if (const auto* r = dynamic_cast<const RoutineDeclaration*>(node)) {
        if (r->expression_body) fn(r->expression_body.get());
        for (const auto& s : r->body) fn(s.get());
    } else if (const auto* b = dynamic_cast<const BinaryExpr*>(node)) {
        fn(b->left.get()); fn(b->right.get());
    } else if (const auto* u = dynamic_cast<const UnaryExpr*>(node)) {
        fn(u->child.get());
    } else if (const auto* a = dynamic_cast<const AssignStatement*>(node)) {
        fn(a->target.get()); fn(a->value.get());
    } else if (const auto* p = dynamic_cast<const PrintStatement*>(node)) {
        for (const auto& item : p->to_print) fn(item.get());
    } else if (const auto* c = dynamic_cast<const RoutineCallStmt*>(node)) {
        for (const auto& arg : c->arguments) fn(arg.get());
    } else if (const auto* c = dynamic_cast<const RoutineCallExpr*>(node)) {
        for (const auto& arg : c->arguments) fn(arg.get());
    } else if (const auto* i = dynamic_cast<const IfStatement*>(node)) {
        fn(i->condition.get());
        for (const auto& s : i->then_body) fn(s.get());
        for (const auto& s : i->else_body) fn(s.get());
    } else if (const auto* w = dynamic_cast<const WhileStatement*>(node)) {
        fn(w->condition.get());
        for (const auto& s : w->body) fn(s.get());
    } else if (const auto* f = dynamic_cast<const ForStatement*>(node)) {
        if (f->range_start) fn(f->range_start.get());
        if (f->range_end) fn(f->range_end.get());
        for (const auto& s : f->body) fn(s.get());
    } else if (const auto* v = dynamic_cast<const VariableDeclaration*>(node)) {
        if (v->expression) fn(v->expression.get());
    } else if (const auto* ret = dynamic_cast<const ReturnStatement*>(node)) {
        if (ret->expression) fn(ret->expression.get());
    } else if (const auto* fa = dynamic_cast<const FieldAccessExpr*>(node)) {
        fn(fa->object.get());
    } else if (const auto* ea = dynamic_cast<const ElementAccessExpr*>(node)) {
        fn(ea->array.get()); fn(ea->index.get());
    }
}

bool is_pure(const AstNode* node) {
    if (!node) return true;
    if (dynamic_cast<const RoutineCallExpr*>(node) || dynamic_cast<const RoutineCallStmt*>(node)) return false;
    bool pure = true;
    for_each_child(node, [&](const AstNode* c) { if (pure && !is_pure(c)) pure = false; });
    return pure;
}

bool contains_identifier(const AstNode* node, const std::string& name) {
    if (!node) return false;
    if (const auto* id = dynamic_cast<const Identifier*>(node)) return id->name == name;
    bool found = false;
    for_each_child(node, [&](const AstNode* c) { if (!found && contains_identifier(c, name)) found = true; });
    return found;
}

bool ends_with_return(const std::vector<std::unique_ptr<AstNode>>& stmts) {
    if (stmts.empty()) return false;
    const auto* last = stmts.back().get();
    if (dynamic_cast<const ReturnStatement*>(last)) return true;
    if (const auto* ifs = dynamic_cast<const IfStatement*>(last)) {
        return ends_with_return(ifs->then_body) && ends_with_return(ifs->else_body);
    }
    return false;
}

} // namespace optimizer_utils
