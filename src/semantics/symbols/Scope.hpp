#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Symbol.hpp"

class Scope {
private:
    Scope* m_parent = nullptr;
    std::vector<std::unique_ptr<Scope>> m_children;
    std::unordered_map<std::string, VariableSymbol> m_variables;
    std::unordered_map<std::string, TypeSymbol> m_types;
    std::unordered_map<std::string, RoutineSymbol> m_routines;

public:
    explicit Scope(Scope* parent = nullptr) : m_parent(parent) {}

    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;

    bool is_global() const {
        return m_parent == nullptr;
    }

    Scope* get_parent() const {
        return m_parent;
    }

    Scope& create_child();

    bool declare_variable(const VariableSymbol& var);
    bool declare_type(const TypeSymbol& type_sym);
    bool declare_routine(const RoutineSymbol& routine);

    VariableSymbol* lookup_variable(const std::string& name, bool check_parents = true);
    TypeSymbol* lookup_type(const std::string& name, bool check_parents = true);
    RoutineSymbol* lookup_routine(const std::string& name, bool check_parents = true);

    const std::unordered_map<std::string, RoutineSymbol>& get_routines() const {
        return m_routines;
    }

    const std::unordered_map<std::string, VariableSymbol>& get_variables() const {
        return m_variables;
    }

    const std::unordered_map<std::string, TypeSymbol>& get_types() const {
        return m_types;
    }
};
