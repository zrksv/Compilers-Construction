#pragma once
#include <string>
#include "types/Type.hpp"
#include "symbols/Scope.hpp"

enum class TypeUse {
    Variable,
    Parameter,
    Result,
    Field,
    Element
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

    Context with_child_scope() const;

    Context enter_routine(const RoutineInfo* info) const;

    // Create a copy with modified TypeUse position
    Context with_type_use(TypeUse use) const {
        Context copy = *this;
        copy.type_use = use;
        return copy;
    }

    bool is_inside_routine() const {
        return routine != nullptr;
    }

    // Unsized arrays are only allowed directly as routine parameters
    bool allows_unsized_array() const {
        return type_use == TypeUse::Parameter;
    }
};
