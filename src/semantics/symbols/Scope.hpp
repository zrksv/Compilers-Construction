#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "Symbol.hpp"

class Scope : public std::enable_shared_from_this<Scope> {
private:
    std::shared_ptr<Scope> m_parent;
    std::unordered_map<std::string, VariableSymbol> m_variables;
    std::unordered_map<std::string, TypeSymbol> m_types;
    std::unordered_map<std::string, RoutineSymbol> m_routines;

public:

    explicit Scope(std::shared_ptr<Scope> parent = nullptr)
        : m_parent(std::move(parent)) {}

    bool is_global() const {
        return m_parent == nullptr;
    }

    std::shared_ptr<Scope> get_parent() const {
        return m_parent;
    }

    std::shared_ptr<Scope> create_child();

    bool declare_variable(const VariableSymbol& var);
    bool declare_type(const TypeSymbol& type_sym);
    bool declare_routine(const RoutineSymbol& routine);

    VariableSymbol* lookup_variable(const std::string& name, bool check_parents = true);
    TypeSymbol* lookup_type(const std::string& name, bool check_parents = true);
    RoutineSymbol* lookup_routine(const std::string& name, bool check_parents = true);

    const std::unordered_map<std::string, RoutineSymbol>& get_routines() const {
        return m_routines;
    }
};
