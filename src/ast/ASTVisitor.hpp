#pragma once

class Program;
class IntLiteral;
class RealLiteral;
class BoolLiteral;
class Identifier;
class UnaryExpr;
class BinaryExpr;
class RoutineCallExpr;
class FieldAccessExpr;
class ElementAccessExpr;
class AssignStatement;
class RoutineCallStmt;
class PrintStatement;
class IfStatement;
class WhileStatement;
class ForStatement;
class ReturnStatement;
class PrimitiveTypeNode;
class IdentifierTypeNode;
class ArrayTypeNode;
class RecordTypeNode;
class ParameterDeclaration;
class VariableDeclaration;
class RoutineDeclaration;
class TypeDeclaration;

class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    virtual void visit(IntLiteral& node) = 0;
    virtual void visit(RealLiteral& node) = 0;
    virtual void visit(BoolLiteral& node) = 0;
    virtual void visit(Identifier& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(RoutineCallExpr& node) = 0;
    virtual void visit(AssignStatement& node) = 0;
    virtual void visit(RoutineCallStmt& node) = 0;
    virtual void visit(PrintStatement& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(ForStatement& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(VariableDeclaration& node) = 0;
    virtual void visit(RoutineDeclaration& node) = 0;
    virtual void visit(PrimitiveTypeNode& node) = 0;
    virtual void visit(IdentifierTypeNode& node) = 0;
    virtual void visit(ArrayTypeNode& node) = 0;
    virtual void visit(RecordTypeNode& node) = 0;
    virtual void visit(FieldAccessExpr& node) = 0;
    virtual void visit(ElementAccessExpr& node) = 0;
    virtual void visit(TypeDeclaration& node) = 0;
    virtual void visit(ParameterDeclaration& node) = 0;
    virtual void visit(Program& node) = 0;
};
