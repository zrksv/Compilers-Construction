#pragma once
#include <string>
#include "types/Type.hpp"
#include "symbols/Scope.hpp"

enum class TypeUse {
    Variable,
    Parameter,
    Result,
    Field,
    Element,
    AliasTarget
};

struct RoutineInfo {
    std::string name;
    TypeRef return_type;
};

class Context {
public:
    Scope* scope = nullptr;
    const RoutineInfo* routine = nullptr;
    TypeUse type_use = TypeUse::Variable;

    Context() = default;
    Context(Scope* s, const RoutineInfo* r, TypeUse u)
        : scope(s), routine(r), type_use(u) {}

    // Create a copy with a new child scope
    Context with_child_scope() const {
        Context copy = *this;
        if (scope) {
            copy.scope = &scope->create_child();
        }
        return copy;
    }

    // Create a copy configured for routine body analysis
    Context enter_routine(const RoutineInfo* info) const {
        Context copy = *this;
        if (scope) {
            copy.scope = &scope->create_child();
        }
        copy.routine = info;
        return copy;
    }

    // Create a copy with modified TypeUse position
    Context with_type_use(TypeUse use) const {
        Context copy = *this;
        copy.type_use = use;
        return copy;
    }

    bool is_inside_routine() const {
        return routine != nullptr;
    }

    // Unsized arrays are only allowed in parameters and type alias targets
    bool allows_unsized_array() const {
        return type_use == TypeUse::Parameter || type_use == TypeUse::AliasTarget;
    }
};
