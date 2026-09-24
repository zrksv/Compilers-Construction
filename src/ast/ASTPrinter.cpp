#include "ASTPrinter.hpp"

std::string binop_to_string(BinOp op) {
    switch (op) {
        case BinOp::Plus: return "+";
        case BinOp::Minus: return "-";
        case BinOp::Mul: return "*";
        case BinOp::Div: return "/";
        case BinOp::Mod: return "%";
        case BinOp::And: return "and";
        case BinOp::Or: return "or";
        case BinOp::Xor: return "xor";
        case BinOp::Less: return "<";
        case BinOp::LessEqual: return "<=";
        case BinOp::Greater: return ">";
        case BinOp::GreaterEqual: return ">=";
        case BinOp::Equal: return "=";
        case BinOp::NotEqual: return "/=";
        default: return "?";
    }
}

std::string unaryop_to_string(UnaryOp op) {
    switch (op) {
        case UnaryOp::Minus: return "-";
        case UnaryOp::Plus: return "+";
        case UnaryOp::Not: return "not";
        default: return "?";
    }
}

void AstPrinter::visit(IntLiteral& node) {
    out << "IntLiteral(" << node.value << ")";
}

void AstPrinter::visit(RealLiteral& node) {
    out << "RealLiteral(" << node.value << ")";
}

void AstPrinter::visit(BoolLiteral& node) {
    out << "BoolLiteral(" << (node.value ? "true" : "false") << ")";
}

void AstPrinter::visit(Identifier& node) {
    out << "Identifier(" << node.name << ")";
}

void AstPrinter::visit(UnaryExpr& node) {
    out << "UnaryExpr(" << unaryop_to_string(node.op) << ")\n";
    print_indent(true);
    out << "Child: ";
    is_last.push_back(true);
    node.child->accept(*this);
    is_last.pop_back();
}

void AstPrinter::visit(BinaryExpr& node) {
    out << "BinaryExpr(" << binop_to_string(node.op) << ")\n";
    
    print_indent(false);
    out << "Left: ";
    is_last.push_back(false);
    node.left->accept(*this);
    out << "\n";
    is_last.pop_back();
    
    print_indent(true);
    out << "Right: ";
    is_last.push_back(true);
    node.right->accept(*this);
    is_last.pop_back();
}

void AstPrinter::visit(RoutineCallExpr& node) {
    out << "RoutineCallExpr(" << node.routine_name << ")";
    if (node.arguments.empty()) return;
    out << "\n";
    for (size_t i = 0; i < node.arguments.size(); ++i) {
        bool last = (i == node.arguments.size() - 1);
        print_indent(last);
        out << "Arg: ";
        is_last.push_back(last);
        node.arguments[i]->accept(*this);
        if (!last) out << "\n";
        is_last.pop_back();
    }
}

void AstPrinter::visit(AssignStatement& node) {
    out << "AssignStatement(" << node.name << ")\n";
    print_indent(true);
    out << "Value: ";
    is_last.push_back(true);
    node.value->accept(*this);
    is_last.pop_back();
}

void AstPrinter::visit(RoutineCallStmt& node) {
    out << "RoutineCallStmt(" << node.routine_name << ")";
    if (node.arguments.empty()) return;
    out << "\n";
    for (size_t i = 0; i < node.arguments.size(); ++i) {
        bool last = (i == node.arguments.size() - 1);
        print_indent(last);
        out << "Arg: ";
        is_last.push_back(last);
        node.arguments[i]->accept(*this);
        if (!last) out << "\n";
        is_last.pop_back();
    }
}

void AstPrinter::visit(PrintStatement& node) {
    out << "PrintStatement";
    if (node.to_print.empty()) return;
    out << "\n";
    for (size_t i = 0; i < node.to_print.size(); ++i) {
        bool last = (i == node.to_print.size() - 1);
        print_indent(last);
        out << "Arg: ";
        is_last.push_back(last);
        node.to_print[i]->accept(*this);
        if (!last) out << "\n";
        is_last.pop_back();
    }
}

void AstPrinter::visit(IfStatement& node) {
    out << "IfStatement\n";
    
    print_indent(false);
    out << "Condition: ";
    is_last.push_back(false);
    node.condition->accept(*this);
    out << "\n";
    is_last.pop_back();

    bool has_else = !node.else_body.empty();
    
    print_indent(!has_else);
    out << "Then:\n";
    is_last.push_back(!has_else);
    for (size_t i = 0; i < node.then_body.size(); ++i) {
        bool last = (i == node.then_body.size() - 1);
        print_indent(last);
        is_last.push_back(last);
        node.then_body[i]->accept(*this);
        if (!last || has_else) out << "\n"; 
        is_last.pop_back();
    }
    is_last.pop_back();

    if (has_else) {
        print_indent(true);
        out << "Else:\n";
        is_last.push_back(true);
        for (size_t i = 0; i < node.else_body.size(); ++i) {
            bool last = (i == node.else_body.size() - 1);
            print_indent(last);
            is_last.push_back(last);
            node.else_body[i]->accept(*this);
            if (!last) out << "\n";
            is_last.pop_back();
        }
        is_last.pop_back();
    }
}

void AstPrinter::visit(WhileStatement& node) {
    out << "WhileLoop\n";
    
    print_indent(false);
    out << "Condition: ";
    is_last.push_back(false);
    node.condition->accept(*this);
    out << "\n";
    is_last.pop_back();
    
    print_indent(true);
    out << "Body:\n";
    is_last.push_back(true);
    for (size_t i = 0; i < node.body.size(); ++i) {
        bool last = (i == node.body.size() - 1);
        print_indent(last);
        is_last.push_back(last);
        node.body[i]->accept(*this);
        if (!last) out << "\n";
        is_last.pop_back();
    }
    is_last.pop_back();
}

void AstPrinter::visit(ForStatement& node) {
    out << "ForLoop(" << node.iterator_name << (node.is_reverse ? " reverse" : "") << ")\n";
    
    print_indent(false);
    out << "Start: ";
    is_last.push_back(false);
    node.range_start->accept(*this);
    out << "\n";
    is_last.pop_back();

    print_indent(false);
    out << "End: ";
    is_last.push_back(false);
    node.range_end->accept(*this);
    out << "\n";
    is_last.pop_back();
    
    print_indent(true);
    out << "Body:\n";
    is_last.push_back(true);
    for (size_t i = 0; i < node.body.size(); ++i) {
        bool last = (i == node.body.size() - 1);
        print_indent(last);
        is_last.push_back(last);
        node.body[i]->accept(*this);
        if (!last) out << "\n";
        is_last.pop_back();
    }
    is_last.pop_back();
}

void AstPrinter::visit(ReturnStatement& node) {
    if (node.expression) {
        out << "ReturnStatement\n";
        print_indent(true);
        out << "Value: ";
        is_last.push_back(true);
        node.expression->accept(*this);
        is_last.pop_back();
    } else {
        out << "ReturnStatement(empty)";
    }
}

void AstPrinter::visit(VariableDeclaration& node) {
    out << "VariableDeclaration(" << node.name;
    if (!node.type.empty()) out << " : " << node.type;
    out << ")";
    if (node.expression) {
        out << "\n";
        print_indent(true);
        out << "Init: ";
        is_last.push_back(true);
        node.expression->accept(*this);
        is_last.pop_back();
    }
}

void AstPrinter::visit(RoutineDeclaration& node) {
    out << "RoutineDeclaration(" << node.name << ")";
    if (!node.parameters.empty()) {
        out << " params: [";
        for (size_t i = 0; i < node.parameters.size(); ++i) {
            out << node.parameters[i] << (i + 1 == node.parameters.size() ? "" : ", ");
        }
        out << "]";
    }
    if (!node.return_type.empty()) out << " returns: " << node.return_type;
    
    if (!node.body.empty()) {
        out << "\n";
        print_indent(true);
        out << "Body:\n";
        is_last.push_back(true);
        for (size_t i = 0; i < node.body.size(); ++i) {
            bool last = (i == node.body.size() - 1);
            print_indent(last);
            is_last.push_back(last);
            node.body[i]->accept(*this);
            if (!last) out << "\n";
            is_last.pop_back();
        }
        is_last.pop_back();
    }
}