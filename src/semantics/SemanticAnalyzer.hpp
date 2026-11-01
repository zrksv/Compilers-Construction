#pragma once
#include "AstDispatcher.hpp"
#include "Context.hpp"
#include "Diagnostics.hpp"
#include "SemanticModel.hpp"
#include "Services.hpp"
#include "ast/AST.hpp"
#include "types/TypeFactory.hpp"

class SemanticAnalyzer {
private:
    Diagnostics m_diagnostics;
    TypeFactory m_types;
    SemanticModel m_model;
    Services m_services;
    AstDispatcher m_dispatcher;

public:
    SemanticAnalyzer();

    SemanticAnalyzer(const SemanticAnalyzer&) = delete;
    SemanticAnalyzer& operator=(const SemanticAnalyzer&) = delete;

    bool analyze(Program* program);

    Context root_context();
    AstDispatcher& dispatcher() { return m_dispatcher; }

    Diagnostics& get_diagnostics() { return m_diagnostics; }
    SemanticModel& get_model() { return m_model; }
    Scope& get_global_scope() { return m_model.global_scope(); }
};
