#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../common/Location.hpp"
#include "../ast/AST.hpp"
#include "Type.hpp"
#include "Symbol.hpp"
#include "Scope.hpp"

struct SemanticError {
    Location location;
    std::string message;
};

class DeclarationAnalyzer {
private:
    std::shared_ptr<Scope> m_current_scope;
    std::vector<SemanticError> m_errors;
    size_t m_next_type_id = 1;

public:
    explicit DeclarationAnalyzer(std::shared_ptr<Scope> global_scope)
        : m_current_scope(std::move(global_scope)) {}

    bool has_errors() const {
        return !m_errors.empty();
    }

    const std::vector<SemanticError>& get_errors() const {
        return m_errors;
    }

    std::shared_ptr<Scope> get_current_scope() const {
        return m_current_scope;
    }

    void report_error(const Location& loc, const std::string& message) {
        m_errors.push_back({loc, message});
    }

    std::shared_ptr<Type> resolve_type(TypeNode* node);

    void analyze(TypeDeclaration* decl);
    void analyze(VariableDeclaration* decl);
    void analyze(RoutineDeclaration* decl);
    void analyze(Program* program);

    std::shared_ptr<Type> infer_expression_type(Expression* expr);
};