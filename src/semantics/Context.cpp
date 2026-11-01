#include "Context.hpp"

Context Context::with_child_scope() const {
    Context copy = *this;
    if (scope) {
        copy.scope = &scope->create_child();
    }
    return copy;
}

Context Context::enter_routine(const RoutineInfo* info) const {
    Context copy = *this;
    if (scope) {
        copy.scope = &scope->create_child();
    }
    copy.routine = info;
    return copy;
}
