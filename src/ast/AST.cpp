#include "AST.hpp"
#include "ASTVisitor.hpp"

void IntLiteral::accept(AstVisitor& visitor) { visitor.visit(*this); }
void RealLiteral::accept(AstVisitor& visitor) { visitor.visit(*this); }
void BoolLiteral::accept(AstVisitor& visitor) { visitor.visit(*this); }
void Identifier::accept(AstVisitor& visitor) { visitor.visit(*this); }
void UnaryExpr::accept(AstVisitor& visitor) { visitor.visit(*this); }
void BinaryExpr::accept(AstVisitor& visitor) { visitor.visit(*this); }
void RoutineCallExpr::accept(AstVisitor& visitor) { visitor.visit(*this); }
void AssignStatement::accept(AstVisitor& visitor) { visitor.visit(*this); }
void RoutineCallStmt::accept(AstVisitor& visitor) { visitor.visit(*this); }
void PrintStatement::accept(AstVisitor& visitor) { visitor.visit(*this); }
void IfStatement::accept(AstVisitor& visitor) { visitor.visit(*this); }
void WhileStatement::accept(AstVisitor& visitor) { visitor.visit(*this); }
void ForStatement::accept(AstVisitor& visitor) { visitor.visit(*this); }
void ReturnStatement::accept(AstVisitor& visitor) { visitor.visit(*this); }
void VariableDeclaration::accept(AstVisitor& visitor) { visitor.visit(*this); }
void RoutineDeclaration::accept(AstVisitor& visitor) { visitor.visit(*this); }
void PrimitiveTypeNode::accept(AstVisitor& visitor) { visitor.visit(*this); }
void IdentifierTypeNode::accept(AstVisitor& visitor) { visitor.visit(*this); }
void ArrayTypeNode::accept(AstVisitor& visitor) { visitor.visit(*this); }
void RecordTypeNode::accept(AstVisitor& visitor) { visitor.visit(*this); }
void FieldAccessExpr::accept(AstVisitor& visitor) { visitor.visit(*this); }
void ElementAccessExpr::accept(AstVisitor& visitor) { visitor.visit(*this); }
void TypeDeclaration::accept(AstVisitor& visitor) { visitor.visit(*this); }
void ParameterDeclaration::accept(AstVisitor& visitor) { visitor.visit(*this); }
void Program::accept(AstVisitor& visitor) { visitor.visit(*this); }
