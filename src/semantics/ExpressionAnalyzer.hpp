#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Type.hpp"
#include "Scope.hpp"
#include "DeclarationAnalyzer.hpp"
#include "../ast/AST.hpp"

class ExpressionAnalyzer {
private:
    std::shared_ptr<Scope> m_scope;
    std::vector<SemanticError>& m_errors;

    void report_error(const Location& loc, const std::string& message) {
        m_errors.push_back({loc, message});
    }

public:
    ExpressionAnalyzer(std::shared_ptr<Scope> scope, std::vector<SemanticError>& errors)
            : m_scope(std::move(scope)), m_errors(errors) {}

    std::shared_ptr<Type> analyze(Expression* expr);

    static bool can_assign(const std::shared_ptr<Type>& target, const std::shared_ptr<Type>& source);
};