#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../ast/AST.hpp"
#include "Type.hpp"
#include "Symbol.hpp"
#include "Scope.hpp"
#include "Diagnostics.hpp"
#include "DeclarationAnalyzer.hpp"

class StatementAnalyzer {
private:
    std::shared_ptr<Scope> m_scope;
    DeclarationAnalyzer& m_declarations;
    std::vector<SemanticError> m_errors;
    std::string m_routine_name;
    std::shared_ptr<Type> m_return_type;

    void report_error(const Location& loc, const std::string& message) {
        m_errors.push_back({loc, message});
    }

    std::shared_ptr<Type> analyze_expression(Expression* expr);
    void analyze_body(const std::shared_ptr<Scope>& scope, const std::vector<std::unique_ptr<AstNode>>& body);
    void analyze_item(AstNode* item);
    void check_condition(Expression* condition, const std::string& statement_name);
    void check_assignment_target(Expression* target);
    void check_call_arguments(const std::string& routine_name,
                              const RoutineSymbol& routine,
                              const std::vector<std::unique_ptr<Expression>>& arguments);

    void analyze(AssignStatement* stmt);
    void analyze(RoutineCallStmt* stmt);
    void analyze(PrintStatement* stmt);
    void analyze(IfStatement* stmt);
    void analyze(WhileStatement* stmt);
    void analyze(ForStatement* stmt);
    void analyze(ReturnStatement* stmt);

    static bool always_returns(const std::vector<std::unique_ptr<AstNode>>& body);
    static bool always_returns(AstNode* item);

public:
    StatementAnalyzer(std::shared_ptr<Scope> global_scope, DeclarationAnalyzer& declarations)
        : m_scope(std::move(global_scope)), m_declarations(declarations) {}

    void analyze_routine(RoutineDeclaration* decl, const std::vector<VariableSymbol>& parameters);

    const std::vector<SemanticError>& get_errors() const {
        return m_errors;
    }
};
