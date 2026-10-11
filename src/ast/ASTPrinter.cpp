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
    if (auto* id = dynamic_cast<Identifier*>(node.target.get())) {
        out << "AssignStatement(" << id->name << ")\n";
    } else {
        out << "AssignStatement\n";
        print_indent(false);
        out << "Target: ";
        is_last.push_back(false);
        node.target->accept(*this);
        out << "\n";
        is_last.pop_back();
    }

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
    
    if (node.range_end) {
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
    } else {
        print_indent(false);
        out << "Array: ";
        is_last.push_back(false);
        node.range_start->accept(*this);
        out << "\n";
        is_last.pop_back();
    }
    
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
    if (auto* prim = dynamic_cast<PrimitiveTypeNode*>(node.type.get())) {
        out << " : " << prim->type_name;
    }
    out << ")";

    bool has_complex_type = (node.type != nullptr && dynamic_cast<PrimitiveTypeNode*>(node.type.get()) == nullptr);
    if (has_complex_type || node.expression) {
        out << "\n";
    }

    if (has_complex_type) {
        bool last = !node.expression;
        print_indent(last);
        out << "Type: ";
        is_last.push_back(last);
        node.type->accept(*this);
        if (!last) out << "\n";
        is_last.pop_back();
    }

    if (node.expression) {
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
            auto& p = node.parameters[i];
            out << p->name;
            if (p->type) {
                if (auto* prim = dynamic_cast<PrimitiveTypeNode*>(p->type.get())) {
                    out << " : " << prim->type_name;
                } else if (auto* id = dynamic_cast<IdentifierTypeNode*>(p->type.get())) {
                    out << " : " << id->name;
                }
            }
            if (i + 1 < node.parameters.size()) {
                out << ", ";
            }
        }
        out << "]";
    }
    if (node.return_type) {
        out << " returns: ";
        if (auto* prim = dynamic_cast<PrimitiveTypeNode*>(node.return_type.get())) {
            out << prim->type_name;
        } else if (auto* id = dynamic_cast<IdentifierTypeNode*>(node.return_type.get())) {
            out << id->name;
        }
    }
    
    if (node.body_type == RoutineBodyType::Forward) {
        out << " (forward)";
    } else if (node.body_type == RoutineBodyType::Expression) {
        out << "\n";
        print_indent(true);
        out << "ExprBody: ";
        is_last.push_back(true);
        node.expression_body->accept(*this);
        is_last.pop_back();
    } else if (node.body_type == RoutineBodyType::Block && !node.body.empty()) {
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

void AstPrinter::visit(PrimitiveTypeNode& node) {
    out << "PrimitiveType(" << node.type_name << ")";
}

void AstPrinter::visit(IdentifierTypeNode& node) {
    out << "IdentifierType(" << node.name << ")";
}

void AstPrinter::visit(ArrayTypeNode& node) {
    out << "ArrayType\n";

    print_indent(false);
    out << "Size: ";
    is_last.push_back(false);
    if (node.size) {
        node.size->accept(*this);
    } else {
        out << "empty";
    }
    out << "\n";
    is_last.pop_back();

    print_indent(true);
    out << "Element: ";
    is_last.push_back(true);
    node.element_type->accept(*this);
    is_last.pop_back();
}

void AstPrinter::visit(RecordTypeNode& node) {
    out << "RecordType";
    if (node.declarations.empty()) return;
    out << "\n";
    for (size_t i = 0; i < node.declarations.size(); ++i) {
        bool last = (i == node.declarations.size() - 1);
        print_indent(last);
        is_last.push_back(last);
        node.declarations[i]->accept(*this);
        if (!last) out << "\n";
        is_last.pop_back();
    }
}

void AstPrinter::visit(FieldAccessExpr& node) {
    out << "FieldAccess(" << node.field_name << ")\n";
    print_indent(true);
    out << "Object: ";
    is_last.push_back(true);
    node.object->accept(*this);
    is_last.pop_back();
}

void AstPrinter::visit(ElementAccessExpr& node) {
    out << "ElementAccess\n";

    print_indent(false);
    out << "Array: ";
    is_last.push_back(false);
    node.array->accept(*this);
    out << "\n";
    is_last.pop_back();

    print_indent(true);
    out << "Index: ";
    is_last.push_back(true);
    node.index->accept(*this);
    is_last.pop_back();
}

void AstPrinter::visit(TypeDeclaration& node) {
    out << "TypeDeclaration(" << node.name << ")\n";
    print_indent(true);
    out << "Type: ";
    is_last.push_back(true);
    node.type->accept(*this);
    is_last.pop_back();
}

void AstPrinter::visit(Program& node) {
    out << "Program";
    if (node.declarations.empty()) return;
    out << "\n";
    for (size_t i = 0; i < node.declarations.size(); ++i) {
        bool last = (i == node.declarations.size() - 1);
        print_indent(last);
        is_last.push_back(last);
        node.declarations[i]->accept(*this);
        if (!last) out << "\n";
        is_last.pop_back();
    }
}

void AstPrinter::visit(ParameterDeclaration& node) {
    out << "ParameterDeclaration(" << node.name;
    if (node.type) {
        if (auto* prim = dynamic_cast<PrimitiveTypeNode*>(node.type.get())) {
            out << " : " << prim->type_name;
        } else if (auto* id = dynamic_cast<IdentifierTypeNode*>(node.type.get())) {
            out << " : " << id->name;
        }
    }
    out << ")";
    if (node.type && !dynamic_cast<PrimitiveTypeNode*>(node.type.get()) && !dynamic_cast<IdentifierTypeNode*>(node.type.get())) {
        out << "\n";
        print_indent(true);
        out << "Type: ";
        is_last.push_back(true);
        node.type->accept(*this);
        is_last.pop_back();
    }
}