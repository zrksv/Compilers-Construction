#include "AST.hpp"
#include "ASTPrinter.hpp"

void IntLiteral::accept(AstPrinter& printer) { printer.visit(*this); }
void RealLiteral::accept(AstPrinter& printer) { printer.visit(*this); }
void BoolLiteral::accept(AstPrinter& printer) { printer.visit(*this); }
void Identifier::accept(AstPrinter& printer) { printer.visit(*this); }
void UnaryExpr::accept(AstPrinter& printer) { printer.visit(*this); }
void BinaryExpr::accept(AstPrinter& printer) { printer.visit(*this); }
void RoutineCallExpr::accept(AstPrinter& printer) { printer.visit(*this); }
void AssignStatement::accept(AstPrinter& printer) { printer.visit(*this); }
void RoutineCallStmt::accept(AstPrinter& printer) { printer.visit(*this); }
void PrintStatement::accept(AstPrinter& printer) { printer.visit(*this); }
void IfStatement::accept(AstPrinter& printer) { printer.visit(*this); }
void WhileStatement::accept(AstPrinter& printer) { printer.visit(*this); }
void ForStatement::accept(AstPrinter& printer) { printer.visit(*this); }
void ReturnStatement::accept(AstPrinter& printer) { printer.visit(*this); }
void VariableDeclaration::accept(AstPrinter& printer) { printer.visit(*this); }
void RoutineDeclaration::accept(AstPrinter& printer) { printer.visit(*this); }
