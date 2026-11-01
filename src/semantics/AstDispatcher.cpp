#include "AstDispatcher.hpp"
#include "ast/ASTVisitor.hpp"

class AstDispatcher::RejectingVisitor : public AstVisitor {
private:
    Services& m_services;

    void reject(AstNode& node) {
        m_services.diagnostics.report(node.location, "Internal error: unexpected node in semantic analysis");
    }

public:
    explicit RejectingVisitor(Services& services) : m_services(services) {}

    void visit(IntLiteral& node) override { reject(node); }
    void visit(RealLiteral& node) override { reject(node); }
    void visit(BoolLiteral& node) override { reject(node); }
    void visit(Identifier& node) override { reject(node); }
    void visit(UnaryExpr& node) override { reject(node); }
    void visit(BinaryExpr& node) override { reject(node); }
    void visit(RoutineCallExpr& node) override { reject(node); }
    void visit(AssignStatement& node) override { reject(node); }
    void visit(RoutineCallStmt& node) override { reject(node); }
    void visit(PrintStatement& node) override { reject(node); }
    void visit(IfStatement& node) override { reject(node); }
    void visit(WhileStatement& node) override { reject(node); }
    void visit(ForStatement& node) override { reject(node); }
    void visit(ReturnStatement& node) override { reject(node); }
    void visit(VariableDeclaration& node) override { reject(node); }
    void visit(RoutineDeclaration& node) override { reject(node); }
    void visit(PrimitiveTypeNode& node) override { reject(node); }
    void visit(IdentifierTypeNode& node) override { reject(node); }
    void visit(ArrayTypeNode& node) override { reject(node); }
    void visit(RecordTypeNode& node) override { reject(node); }
    void visit(FieldAccessExpr& node) override { reject(node); }
    void visit(ElementAccessExpr& node) override { reject(node); }
    void visit(TypeDeclaration& node) override { reject(node); }
    void visit(ParameterDeclaration& node) override { reject(node); }
    void visit(Program& node) override { reject(node); }
};

class AstDispatcher::ExpressionVisitor final : public RejectingVisitor {
private:
    AstDispatcher& m_owner;
    const Context& m_ctx;

public:
    ExprResult result = ExprResult::error();

    ExpressionVisitor(AstDispatcher& owner, const Context& ctx)
        : RejectingVisitor(owner.m_services), m_owner(owner), m_ctx(ctx) {}

    void visit(IntLiteral& node) override { result = m_owner.m_int_literal.analyze(node, m_ctx); }
    void visit(RealLiteral& node) override { result = m_owner.m_real_literal.analyze(node, m_ctx); }
    void visit(BoolLiteral& node) override { result = m_owner.m_bool_literal.analyze(node, m_ctx); }
    void visit(Identifier& node) override { result = m_owner.m_identifier.analyze(node, m_ctx); }
    void visit(UnaryExpr& node) override { result = m_owner.m_unary.analyze(node, m_ctx); }
    void visit(BinaryExpr& node) override { result = m_owner.m_binary.analyze(node, m_ctx); }
    void visit(RoutineCallExpr& node) override { result = m_owner.m_call_expr.analyze(node, m_ctx); }
    void visit(FieldAccessExpr& node) override { result = m_owner.m_field_access.analyze(node, m_ctx); }
    void visit(ElementAccessExpr& node) override { result = m_owner.m_element_access.analyze(node, m_ctx); }
};

class AstDispatcher::TypeVisitor final : public RejectingVisitor {
private:
    AstDispatcher& m_owner;
    const Context& m_ctx;

public:
    TypeRef result = PrimitiveType::get_error();

    TypeVisitor(AstDispatcher& owner, const Context& ctx)
        : RejectingVisitor(owner.m_services), m_owner(owner), m_ctx(ctx) {}

    void visit(PrimitiveTypeNode& node) override { result = m_owner.m_primitive_type.resolve(node, m_ctx); }
    void visit(IdentifierTypeNode& node) override { result = m_owner.m_identifier_type.resolve(node, m_ctx); }
    void visit(ArrayTypeNode& node) override { result = m_owner.m_array_type.resolve(node, m_ctx); }
    void visit(RecordTypeNode& node) override { result = m_owner.m_record_type.resolve(node, m_ctx); }
};

class AstDispatcher::StatementVisitor final : public RejectingVisitor {
private:
    AstDispatcher& m_owner;
    const Context& m_ctx;

public:
    StmtResult result = StmtResult{};

    StatementVisitor(AstDispatcher& owner, const Context& ctx)
        : RejectingVisitor(owner.m_services), m_owner(owner), m_ctx(ctx) {}

    void visit(Program& node) override { result = m_owner.m_program.analyze(node, m_ctx); }
    void visit(VariableDeclaration& node) override { result = m_owner.m_variable_declaration.analyze(node, m_ctx); }
    void visit(TypeDeclaration& node) override { result = m_owner.m_type_declaration.analyze(node, m_ctx); }
    void visit(RoutineDeclaration& node) override { result = m_owner.m_routine_declaration.analyze(node, m_ctx); }
    void visit(AssignStatement& node) override { result = m_owner.m_assign.analyze(node, m_ctx); }
    void visit(RoutineCallStmt& node) override { result = m_owner.m_call_stmt.analyze(node, m_ctx); }
    void visit(PrintStatement& node) override { result = m_owner.m_print.analyze(node, m_ctx); }
    void visit(IfStatement& node) override { result = m_owner.m_if.analyze(node, m_ctx); }
    void visit(WhileStatement& node) override { result = m_owner.m_while.analyze(node, m_ctx); }
    void visit(ForStatement& node) override { result = m_owner.m_for.analyze(node, m_ctx); }
    void visit(ReturnStatement& node) override { result = m_owner.m_return.analyze(node, m_ctx); }
};

AstDispatcher::AstDispatcher(Services& services)
    : m_services(services),
      m_primitive_type(*this, services),
      m_identifier_type(*this, services),
      m_array_type(*this, services),
      m_record_type(*this, services),
      m_int_literal(*this, services),
      m_real_literal(*this, services),
      m_bool_literal(*this, services),
      m_identifier(*this, services),
      m_unary(*this, services),
      m_binary(*this, services),
      m_field_access(*this, services),
      m_element_access(*this, services),
      m_call_expr(*this, services),
      m_program(*this, services),
      m_variable_declaration(*this, services),
      m_type_declaration(*this, services),
      m_routine_declaration(*this, services),
      m_assign(*this, services),
      m_call_stmt(*this, services),
      m_print(*this, services),
      m_if(*this, services),
      m_while(*this, services),
      m_for(*this, services),
      m_return(*this, services) {}

ExprResult AstDispatcher::expression(Expression* node, const Context& ctx) {
    if (!node) {
        return ExprResult::error();
    }
    ExpressionVisitor visitor(*this, ctx);
    node->accept(visitor);
    m_services.model.set_type(node, visitor.result.type);
    return visitor.result;
}

TypeRef AstDispatcher::type(TypeNode* node, const Context& ctx) {
    if (!node) {
        return PrimitiveType::get_error();
    }
    TypeVisitor visitor(*this, ctx);
    node->accept(visitor);
    return visitor.result;
}

StmtResult AstDispatcher::statement(AstNode* node, const Context& ctx) {
    if (!node) {
        return StmtResult{};
    }
    StatementVisitor visitor(*this, ctx);
    node->accept(visitor);
    return visitor.result;
}

StmtResult AstDispatcher::statement_list(const std::vector<std::unique_ptr<AstNode>>& nodes, const Context& ctx) {
    StmtResult result;
    for (const auto& node : nodes) {
        if (statement(node.get(), ctx).returns) {
            result.returns = true;
        }
    }
    return result;
}
