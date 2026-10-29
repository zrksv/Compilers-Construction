#include "Scope.hpp"

std::shared_ptr<Scope> Scope::create_child() {
    return std::make_shared<Scope>(shared_from_this());
}

// ============================================================================
// Declarations
// ============================================================================
bool Scope::declare_variable(const VariableSymbol& var) {
    if (m_variables.find(var.name) != m_variables.end()) {
        return false;
    }
    m_variables.emplace(var.name, var);
    return true;
}

bool Scope::declare_type(const TypeSymbol& type_sym) {
    if (m_types.find(type_sym.name) != m_types.end()) {
        return false;
    }
    m_types.emplace(type_sym.name, type_sym);
    return true;
}

bool Scope::declare_routine(const RoutineSymbol& routine) {
    if (m_routines.find(routine.name) != m_routines.end()) {
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

