#include "StatementAnalyzer.hpp"
#include "ExpressionAnalyzer.hpp"

std::shared_ptr<Type> StatementAnalyzer::analyze_expression(Expression* expr) {
    ExpressionAnalyzer analyzer(m_scope, m_errors);
    return analyzer.analyze(expr);
}

void StatementAnalyzer::analyze_routine(RoutineDeclaration* decl, const std::vector<VariableSymbol>& parameters) {
    if (!decl) return;

    auto* symbol = m_scope->lookup_routine(decl->name);
    m_routine_name = decl->name;
    m_return_type = symbol ? symbol->return_type : PrimitiveType::get_error();

    auto routine_scope = m_scope->create_child();
    for (const auto& param : parameters) {
        routine_scope->declare_variable(param);
    }

    if (decl->body_type == RoutineBodyType::Expression) {
        if (m_return_type->is_void()) {
            report_error(decl->location, "Routine '" + decl->name + "' with '=>' body must declare a return type");
        }

        auto prev_scope = m_scope;
        m_scope = routine_scope;
        auto value_type = analyze_expression(decl->expression_body.get());
        m_scope = prev_scope;

        if (!m_return_type->is_void() && !ExpressionAnalyzer::can_assign(m_return_type, value_type)) {
            report_error(decl->expression_body->location,
                         "Cannot return value of type " + value_type->to_string() +
                         " from routine '" + decl->name + "' returning " + m_return_type->to_string());
        }
        return;
    }

    analyze_body(routine_scope, decl->body);

    if (!m_return_type->is_void() && !m_return_type->is_error() && !always_returns(decl->body)) {
        report_error(decl->location, "Routine '" + decl->name + "' must return a value of type " +
                                     m_return_type->to_string() + " on every path");
    }
}

void StatementAnalyzer::analyze_body(const std::shared_ptr<Scope>& scope, const std::vector<std::unique_ptr<AstNode>>& body) {
    auto prev_scope = m_scope;
    m_scope = scope;
    m_declarations.set_current_scope(scope);

    for (const auto& item : body) {
        analyze_item(item.get());
    }

    m_scope = prev_scope;
    m_declarations.set_current_scope(prev_scope);
}

void StatementAnalyzer::analyze_item(AstNode* item) {
    if (!item) return;

    if (auto* var_decl = dynamic_cast<VariableDeclaration*>(item)) {
        m_declarations.analyze(var_decl);
    } else if (auto* type_decl = dynamic_cast<TypeDeclaration*>(item)) {
        m_declarations.analyze(type_decl);
    } else if (auto* routine_decl = dynamic_cast<RoutineDeclaration*>(item)) {
        m_declarations.analyze(routine_decl);
    } else if (auto* assign = dynamic_cast<AssignStatement*>(item)) {
        analyze(assign);
    } else if (auto* call = dynamic_cast<RoutineCallStmt*>(item)) {
        analyze(call);
    } else if (auto* print = dynamic_cast<PrintStatement*>(item)) {
        analyze(print);
    } else if (auto* if_stmt = dynamic_cast<IfStatement*>(item)) {
        analyze(if_stmt);
    } else if (auto* while_stmt = dynamic_cast<WhileStatement*>(item)) {
        analyze(while_stmt);
    } else if (auto* for_stmt = dynamic_cast<ForStatement*>(item)) {
        analyze(for_stmt);
    } else if (auto* return_stmt = dynamic_cast<ReturnStatement*>(item)) {
        analyze(return_stmt);
    } else {
        report_error(item->location, "Unknown statement node in semantic analysis");
    }
}

void StatementAnalyzer::check_condition(Expression* condition, const std::string& statement_name) {
    auto cond_type = analyze_expression(condition);
    if (!ExpressionAnalyzer::can_assign(PrimitiveType::get_boolean(), cond_type)) {
        report_error(condition->location, "Condition of '" + statement_name +
                                          "' must be boolean or integer, got " + cond_type->to_string());
    }
}

void StatementAnalyzer::check_assignment_target(Expression* target) {
    if (auto* id = dynamic_cast<Identifier*>(target)) {
        auto* sym = m_scope->lookup_variable(id->name);
        if (sym && sym->is_readonly) {
            report_error(id->location, "Cannot assign to read-only loop variable '" + id->name + "'");
        }
        return;
    }

    if (auto* field = dynamic_cast<FieldAccessExpr*>(target)) {
        if (field->field_name == "size") {
            report_error(field->location, "Array size is read-only and cannot be assigned");
        }
        return;
    }

    if (dynamic_cast<ElementAccessExpr*>(target)) {
        return;
    }

    report_error(target->location, "Left side of assignment must be a variable, a record field or an array element");
}

void StatementAnalyzer::check_call_arguments(const std::string& routine_name,
                                             const RoutineSymbol& routine,
                                             const std::vector<std::unique_ptr<Expression>>& arguments) {
    for (size_t i = 0; i < arguments.size(); ++i) {
        auto arg_type = analyze_expression(arguments[i].get());
        if (i >= routine.parameters.size()) continue;

        auto param_type = routine.parameters[i].type;
        if (!ExpressionAnalyzer::can_assign(param_type, arg_type)) {
            report_error(arguments[i]->location,
                         "Argument " + std::to_string(i + 1) + " of '" + routine_name + "' expects type " +
                         param_type->to_string() + ", got " + arg_type->to_string());
        }
    }
}

void StatementAnalyzer::analyze(AssignStatement* stmt) {
    auto target_type = analyze_expression(stmt->target.get());
    auto value_type = analyze_expression(stmt->value.get());

    check_assignment_target(stmt->target.get());

    if (!ExpressionAnalyzer::can_assign(target_type, value_type)) {
        report_error(stmt->location, "Cannot assign value of type " + value_type->to_string() +
                                     " to target of type " + target_type->to_string());
    }
}

void StatementAnalyzer::analyze(RoutineCallStmt* stmt) {
    auto* routine = m_scope->lookup_routine(stmt->routine_name);
    if (!routine) {
        report_error(stmt->location, "Routine '" + stmt->routine_name + "' is not declared");
        for (const auto& arg : stmt->arguments) {
            analyze_expression(arg.get());
        }
        return;
    }

    if (stmt->arguments.size() != routine->parameters.size()) {
        report_error(stmt->location, "Routine '" + stmt->routine_name + "' expects " +
                                     std::to_string(routine->parameters.size()) + " argument(s), got " +
                                     std::to_string(stmt->arguments.size()));
    }

    check_call_arguments(stmt->routine_name, *routine, stmt->arguments);
}

void StatementAnalyzer::analyze(PrintStatement* stmt) {
    for (const auto& expr : stmt->to_print) {
        auto type = analyze_expression(expr.get());
        if (type->is_array() || type->is_record()) {
            report_error(expr->location, "Cannot print value of aggregate type " + type->to_string());
        }
    }
}

void StatementAnalyzer::analyze(IfStatement* stmt) {
    check_condition(stmt->condition.get(), "if");
    analyze_body(m_scope->create_child(), stmt->then_body);
    analyze_body(m_scope->create_child(), stmt->else_body);
}

void StatementAnalyzer::analyze(WhileStatement* stmt) {
    check_condition(stmt->condition.get(), "while");
    analyze_body(m_scope->create_child(), stmt->body);
}

void StatementAnalyzer::analyze(ForStatement* stmt) {
    std::shared_ptr<Type> iterator_type = PrimitiveType::get_integer();

    if (stmt->range_end) {
        auto start_type = analyze_expression(stmt->range_start.get());
        auto end_type = analyze_expression(stmt->range_end.get());

        if (!ExpressionAnalyzer::can_assign(PrimitiveType::get_integer(), start_type)) {
            report_error(stmt->range_start->location, "Lower bound of 'for' range must be compatible with integer, got " +
                                                      start_type->to_string());
        }
        if (!ExpressionAnalyzer::can_assign(PrimitiveType::get_integer(), end_type)) {
            report_error(stmt->range_end->location, "Upper bound of 'for' range must be compatible with integer, got " +
                                                    end_type->to_string());
        }
    } else {
        auto collection_type = analyze_expression(stmt->range_start.get());
        if (collection_type->is_error()) {
            iterator_type = PrimitiveType::get_error();
        } else if (!collection_type->is_array()) {
            report_error(stmt->range_start->location, "'for' without '..' requires an array, got " +
                                                      collection_type->to_string());
            iterator_type = PrimitiveType::get_error();
        } else {
            iterator_type = std::static_pointer_cast<ArrayType>(collection_type->unalias())->element_type;
        }
    }

    auto loop_scope = m_scope->create_child();
    loop_scope->declare_variable(VariableSymbol(stmt->iterator_name, iterator_type, true, stmt->location));
    analyze_body(loop_scope, stmt->body);
}

void StatementAnalyzer::analyze(ReturnStatement* stmt) {
    if (m_return_type->is_void()) {
        if (stmt->expression) {
            analyze_expression(stmt->expression.get());
            report_error(stmt->location, "Routine '" + m_routine_name + "' has no return type and cannot return a value");
        }
        return;
    }

    if (!stmt->expression) {
        report_error(stmt->location, "Routine '" + m_routine_name + "' must return a value of type " +
                                     m_return_type->to_string());
        return;
    }

    auto value_type = analyze_expression(stmt->expression.get());
    if (!ExpressionAnalyzer::can_assign(m_return_type, value_type)) {
        report_error(stmt->expression->location,
                     "Cannot return value of type " + value_type->to_string() +
                     " from routine '" + m_routine_name + "' returning " + m_return_type->to_string());
    }
}

bool StatementAnalyzer::always_returns(const std::vector<std::unique_ptr<AstNode>>& body) {
    for (const auto& item : body) {
        if (always_returns(item.get())) {
            return true;
        }
    }
    return false;
}

bool StatementAnalyzer::always_returns(AstNode* item) {
    if (dynamic_cast<ReturnStatement*>(item)) {
        return true;
    }
    if (auto* if_stmt = dynamic_cast<IfStatement*>(item)) {
        return always_returns(if_stmt->then_body) && always_returns(if_stmt->else_body);
    }
    return false;
}
