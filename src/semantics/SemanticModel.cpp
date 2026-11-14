#include "SemanticModel.hpp"

void SemanticModel::set_type(const Expression* expr, TypeRef type) {
    if (expr) {
        m_expr_types[expr] = std::move(type);
    }
}

TypeRef SemanticModel::type_of(const Expression* expr) const {
    auto it = m_expr_types.find(expr);
    return it != m_expr_types.end() ? it->second : nullptr;
}

void SemanticModel::bind_variable(const AstNode* node, const VariableSymbol* var) {
    if (node) {
        m_var_bindings[node] = var;
    }
}

const VariableSymbol* SemanticModel::variable_of(const AstNode* node) const {
    auto it = m_var_bindings.find(node);
    return it != m_var_bindings.end() ? it->second : nullptr;
}

void SemanticModel::bind_routine(const AstNode* call, const RoutineSymbol* routine) {
    if (call) {
        m_routine_bindings[call] = routine;
    }
}

const RoutineSymbol* SemanticModel::routine_of(const AstNode* call) const {
    auto it = m_routine_bindings.find(call);
    return it != m_routine_bindings.end() ? it->second : nullptr;
}

void SemanticModel::set_conversion(const Expression* expr, ConversionKind kind) {
    if (expr) {
        m_conversions[expr] = kind;
    }
}

ConversionKind SemanticModel::conversion_of(const Expression* expr) const {
    auto it = m_conversions.find(expr);
    return it != m_conversions.end() ? it->second : ConversionKind::None;
}

void SemanticModel::forget(const AstNode* node) {
    if (!node) {
        return;
    }
    m_routine_bindings.erase(node);
    m_var_bindings.erase(node);
    if (const auto* expr = dynamic_cast<const Expression*>(node)) {
        m_expr_types.erase(expr);
        m_conversions.erase(expr);
    }
}
