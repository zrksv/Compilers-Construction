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

void SemanticModel::bind_variable(const Identifier* id, const VariableSymbol* var) {
    if (id) {
        m_var_bindings[id] = var;
    }
}

const VariableSymbol* SemanticModel::variable_of(const Identifier* id) const {
    auto it = m_var_bindings.find(id);
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
