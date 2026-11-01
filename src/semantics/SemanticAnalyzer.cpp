#include "SemanticAnalyzer.hpp"

SemanticAnalyzer::SemanticAnalyzer()
    : m_services(m_diagnostics, m_types, m_model),
      m_dispatcher(m_services) {}

Context SemanticAnalyzer::root_context() {
    return Context(&m_model.global_scope(), nullptr, TypeUse::Variable);
}

bool SemanticAnalyzer::analyze(Program* program) {
    if (!program) {
        m_diagnostics.report(Location{}, "Internal error: no program to analyze");
        return false;
    }

    m_dispatcher.statement(program, root_context());
    m_diagnostics.sort();
    return !m_diagnostics.has_errors();
}
