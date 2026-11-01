#include "Scope.hpp"

Scope& Scope::create_child() {
    auto child = std::make_unique<Scope>(this);
    m_children.push_back(std::move(child));
    return *m_children.back();
}

bool Scope::is_declared_locally(const std::string& name) const {
    return m_variables.find(name) != m_variables.end() ||
           m_types.find(name) != m_types.end() ||
           m_routines.find(name) != m_routines.end();
}

// ============================================================================
// Declarations
// ============================================================================

bool Scope::declare_variable(const VariableSymbol& var) {
    if (is_declared_locally(var.name)) {
        return false;
    }
    m_variables.emplace(var.name, var);
    return true;
}

bool Scope::declare_type(const TypeSymbol& type_sym) {
    if (is_declared_locally(type_sym.name)) {
        return false;
    }
    m_types.emplace(type_sym.name, type_sym);
    return true;
}

bool Scope::declare_routine(const RoutineSymbol& routine) {
    if (is_declared_locally(routine.name)) {
        return false;
    }
    m_routines.emplace(routine.name, routine);
    return true;
}

// ============================================================================
// Lookup
// ============================================================================

VariableSymbol* Scope::lookup_variable(const std::string& name, bool check_parents) {
    auto it = m_variables.find(name);
    if (it != m_variables.end()) {
        return &it->second;
    }

    if (check_parents && m_parent) {
        return m_parent->lookup_variable(name, true);
    }
    return nullptr;
}

TypeSymbol* Scope::lookup_type(const std::string& name, bool check_parents) {
    auto it = m_types.find(name);
    if (it != m_types.end()) {
        return &it->second;
    }

    if (check_parents && m_parent) {
        return m_parent->lookup_type(name, true);
    }
    return nullptr;
}

RoutineSymbol* Scope::lookup_routine(const std::string& name, bool check_parents) {
    auto it = m_routines.find(name);
    if (it != m_routines.end()) {
        return &it->second;
    }

    if (check_parents && m_parent) {
        return m_parent->lookup_routine(name, true);
    }
    return nullptr;
}
