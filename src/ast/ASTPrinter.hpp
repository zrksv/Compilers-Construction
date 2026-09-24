#pragma once
#include <ostream>
#include <vector>
#include "AST.hpp"

class AstPrinter {
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

    void visit(IntLiteral& node);
    void visit(RealLiteral& node);
    void visit(BoolLiteral& node);
    void visit(Identifier& node);
    void visit(UnaryExpr& node);
    void visit(BinaryExpr& node);
    void visit(RoutineCallExpr& node);
    void visit(AssignStatement& node);
    void visit(RoutineCallStmt& node);
    void visit(PrintStatement& node);
    void visit(IfStatement& node);
    void visit(WhileStatement& node);
    void visit(ForStatement& node);
    void visit(ReturnStatement& node);
    void visit(VariableDeclaration& node);
    void visit(RoutineDeclaration& node);
};