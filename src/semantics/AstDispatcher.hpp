#pragma once
#include "Dispatcher.hpp"
#include "Services.hpp"
#include "rules/types/PrimitiveTypeRule.hpp"
#include "rules/types/IdentifierTypeRule.hpp"
#include "rules/types/ArrayTypeRule.hpp"
#include "rules/types/RecordTypeRule.hpp"
#include "rules/expressions/LiteralRules.hpp"
#include "rules/expressions/IdentifierRule.hpp"
#include "rules/expressions/UnaryRule.hpp"
#include "rules/expressions/BinaryRule.hpp"
#include "rules/expressions/FieldAccessRule.hpp"
#include "rules/expressions/ElementAccessRule.hpp"
#include "rules/expressions/RoutineCallExprRule.hpp"
#include "rules/declarations/ProgramRule.hpp"
#include "rules/declarations/VariableDeclarationRule.hpp"
#include "rules/declarations/TypeDeclarationRule.hpp"
#include "rules/declarations/RoutineDeclarationRule.hpp"
#include "rules/statements/AssignRule.hpp"
#include "rules/statements/RoutineCallStmtRule.hpp"
#include "rules/statements/PrintRule.hpp"
#include "rules/statements/IfRule.hpp"
#include "rules/statements/WhileRule.hpp"
#include "rules/statements/ForRule.hpp"
#include "rules/statements/ReturnRule.hpp"

class AstDispatcher final : public Dispatcher {
private:
    class RejectingVisitor;
    class ExpressionVisitor;
    class TypeVisitor;
    class StatementVisitor;

    Services& m_services;

    PrimitiveTypeRule m_primitive_type;
    IdentifierTypeRule m_identifier_type;
    ArrayTypeRule m_array_type;
    RecordTypeRule m_record_type;

    IntLiteralRule m_int_literal;
    RealLiteralRule m_real_literal;
    BoolLiteralRule m_bool_literal;
    IdentifierRule m_identifier;
    UnaryRule m_unary;
    BinaryRule m_binary;
    FieldAccessRule m_field_access;
    ElementAccessRule m_element_access;
    RoutineCallExprRule m_call_expr;

    ProgramRule m_program;
    VariableDeclarationRule m_variable_declaration;
    TypeDeclarationRule m_type_declaration;
    RoutineDeclarationRule m_routine_declaration;

    AssignRule m_assign;
    RoutineCallStmtRule m_call_stmt;
    PrintRule m_print;
    IfRule m_if;
    WhileRule m_while;
    ForRule m_for;
    ReturnRule m_return;

public:
    explicit AstDispatcher(Services& services);

    ExprResult expression(Expression* node, const Context& ctx) override;
    TypeRef type(TypeNode* node, const Context& ctx) override;
    StmtResult statement(AstNode* node, const Context& ctx) override;
    StmtResult statement_list(const std::vector<std::unique_ptr<AstNode>>& nodes, const Context& ctx) override;
};
