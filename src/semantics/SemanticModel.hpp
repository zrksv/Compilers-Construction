#pragma once
#include <unordered_map>
#include "ast/AST.hpp"
#include "symbols/Scope.hpp"
#include "symbols/Symbol.hpp"
#include "types/Type.hpp"
#include "types/TypeCompatibility.hpp"

class SemanticModel {
private:
    Scope m_global_scope;
    std::unordered_map<const Expression*, TypeRef> m_expr_types;
    std::unordered_map<const Identifier*, const VariableSymbol*> m_var_bindings;
    std::unordered_map<const AstNode*, const RoutineSymbol*> m_routine_bindings;
    std::unordered_map<const Expression*, ConversionKind> m_conversions;

public:
    SemanticModel() = default;

    SemanticModel(const SemanticModel&) = delete;
    SemanticModel& operator=(const SemanticModel&) = delete;

    Scope& global_scope() { return m_global_scope; }
    const Scope& global_scope() const { return m_global_scope; }

    void set_type(const Expression* expr, TypeRef type);
    TypeRef type_of(const Expression* expr) const;

    void bind_variable(const Identifier* id, const VariableSymbol* var);
    const VariableSymbol* variable_of(const Identifier* id) const;

    void bind_routine(const AstNode* call, const RoutineSymbol* routine);
    const RoutineSymbol* routine_of(const AstNode* call) const;

    void set_conversion(const Expression* expr, ConversionKind kind);
    ConversionKind conversion_of(const Expression* expr) const;
};
