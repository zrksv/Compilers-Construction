#pragma once
#include <ostream>
#include <vector>
#include "AST.hpp"
#include "ASTVisitor.hpp"

class AstPrinter final : public AstVisitor {
private:
    std::ostream& out;
    std::vector<bool> is_last;

    void print_indent(bool last_child) {
        for (size_t i = 0; i < is_last.size(); ++i) {
            if (is_last[i]) {
                out << "    ";
            } else {
                out << "│   ";
            }
        }
        if (last_child) {
            out << "└── ";
        } else {
            out << "├── ";
        }
    }

public:
    explicit AstPrinter(std::ostream& out_stream) : out(out_stream) {}

    void visit(IntLiteral& node) override;
    void visit(RealLiteral& node) override;
    void visit(BoolLiteral& node) override;
    void visit(Identifier& node) override;
    void visit(UnaryExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(RoutineCallExpr& node) override;
    void visit(AssignStatement& node) override;
    void visit(RoutineCallStmt& node) override;
    void visit(PrintStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(VariableDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(PrimitiveTypeNode& node) override;
    void visit(IdentifierTypeNode& node) override;
    void visit(ArrayTypeNode& node) override;
    void visit(RecordTypeNode& node) override;
    void visit(FieldAccessExpr& node) override;
    void visit(ElementAccessExpr& node) override;
    void visit(TypeDeclaration& node) override;
    void visit(ParameterDeclaration& node) override;
    void visit(Program& node) override;
};