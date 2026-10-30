#include "SemanticAnalyzer.hpp"

bool SemanticAnalyzer::analyze(Program* program) {
    if (!program) {
        m_diagnostics.report({}, "No program to analyze");
        return false;
    }

    for (const auto& item : program->declarations) {
        if (auto* var_decl = dynamic_cast<VariableDeclaration*>(item.get())) {
            m_declarations.analyze(var_decl);
        } else if (auto* type_decl = dynamic_cast<TypeDeclaration*>(item.get())) {
            m_declarations.analyze(type_decl);
        } else if (auto* routine_decl = dynamic_cast<RoutineDeclaration*>(item.get())) {
            if (auto parameters = m_declarations.declare_routine_header(routine_decl)) {
                m_statements.analyze_routine(routine_decl, *parameters);
            }
        } else if (item) {
            m_diagnostics.report(item->location, "Only declarations are allowed at the top level of a program");
        }
    }

    for (const auto& [name, routine] : m_global_scope->get_routines()) {
        if (routine.is_forward && !routine.is_defined) {
            m_diagnostics.report(routine.location, "Routine '" + name + "' was forward-declared but never defined");
        }
    }

    m_diagnostics.append(m_declarations.get_errors());
    m_diagnostics.append(m_statements.get_errors());
    m_diagnostics.sort();

    return !m_diagnostics.has_errors();
}
