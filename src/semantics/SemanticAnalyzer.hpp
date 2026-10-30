#pragma once
#include <memory>
#include "../ast/AST.hpp"
#include "Scope.hpp"
#include "Diagnostics.hpp"
#include "DeclarationAnalyzer.hpp"
#include "StatementAnalyzer.hpp"

class SemanticAnalyzer {
private:
    std::shared_ptr<Scope> m_global_scope;
    DeclarationAnalyzer m_declarations;
    StatementAnalyzer m_statements;
    Diagnostics m_diagnostics;

public:
    SemanticAnalyzer()
        : m_global_scope(std::make_shared<Scope>()),
          m_declarations(m_global_scope),
          m_statements(m_global_scope, m_declarations) {}

    [[nodiscard]] bool analyze(Program* program);

    const Diagnostics& get_diagnostics() const {
        return m_diagnostics;
    }

    std::shared_ptr<Scope> get_global_scope() const {
        return m_global_scope;
    }
};
