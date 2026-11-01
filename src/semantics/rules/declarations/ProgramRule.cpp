#include "rules/declarations/ProgramRule.hpp"

StmtResult ProgramRule::analyze(Program& node, const Context& ctx) {
    m_dispatcher.statement_list(node.declarations, ctx);

    if (ctx.scope) {
        for (const auto& [name, routine] : ctx.scope->get_routines()) {
            if (routine.is_forward && !routine.is_defined) {
                error(routine.location, "Routine '" + name + "' was forward-declared but never defined");
            }
        }
    }

    return StmtResult{};
}
