#include "DeclarationAnalyzer.hpp"
#include "ExpressionAnalyzer.hpp"
#include <unordered_set>

std::shared_ptr<Type> DeclarationAnalyzer::resolve_type(TypeNode* node) {
    if (!node) {
        return PrimitiveType::get_error();
    }

    // Primitive Types (integer, real, boolean)
    if (auto* prim = dynamic_cast<PrimitiveTypeNode*>(node)) {
        if (prim->type_name == "integer") return PrimitiveType::get_integer();
        if (prim->type_name == "real")    return PrimitiveType::get_real();
        if (prim->type_name == "boolean") return PrimitiveType::get_boolean();

        report_error(prim->location, "Unknown primitive type: " + prim->type_name);
        return PrimitiveType::get_error();
    }

    // Aliases
    if (auto* id_node = dynamic_cast<IdentifierTypeNode*>(node)) {
        auto* symbol = m_current_scope->lookup_type(id_node->name);
        if (!symbol) {
            report_error(id_node->location, "Type '" + id_node->name + "' is not declared");
            return PrimitiveType::get_error();
        }
        return symbol->type;
    }

    // Arrays
    if (auto* arr_node = dynamic_cast<ArrayTypeNode*>(node)) {
        auto element_type = resolve_type(arr_node->element_type.get());

        // No size for parameters
        if (!arr_node->size) {
            return std::make_shared<ArrayType>(element_type, std::nullopt, m_next_type_id++);
        }

        // Basic declaration with size
        if (auto* int_lit = dynamic_cast<IntLiteral*>(arr_node->size.get())) {
            if (int_lit->value <= 0) {
                report_error(arr_node->size->location, "Array size must be greater than zero");
                return std::make_shared<ArrayType>(element_type, 1, m_next_type_id++);
            }
            return std::make_shared<ArrayType>(element_type, static_cast<size_t>(int_lit->value), m_next_type_id++);
        }
        report_error(arr_node->size->location, "Array size must be a compile-time integer constant");
        return PrimitiveType::get_error();
    }

    // Records
    if (auto* rec_node = dynamic_cast<RecordTypeNode*>(node)) {
        std::vector<RecordField> fields;
        std::unordered_set<std::string> seen_fields;

        for (const auto& decl : rec_node->declarations) {
            if (seen_fields.find(decl->name) != seen_fields.end()) {
                report_error(decl->location, "Duplicate field '" + decl->name + "' in record");
                continue;
            }
            seen_fields.insert(decl->name);

            auto field_type = resolve_type(decl->type.get());

            if (field_type->is_array()) {
                auto arr = std::static_pointer_cast<ArrayType>(field_type->unalias());
                if (arr->is_unsized()) {
                    report_error(decl->location, "Record field cannot be an unsized array");
                }
            }

            fields.push_back({decl->name, field_type});
        }

        return std::make_shared<RecordType>("", fields, m_next_type_id++);
    }

    report_error(node->location, "Unknown type node in AST");
    return PrimitiveType::get_error();
}

void DeclarationAnalyzer::analyze(TypeDeclaration* decl) {
    if (!decl) return;

    if (m_current_scope->lookup_type(decl->name, false)) {
        report_error(decl->location, "Type '" + decl->name + "' is already declared in this scope");
        return;
    }

    auto resolved_type = resolve_type(decl->type.get());
    if (resolved_type->is_record()) {
        auto rec = std::static_pointer_cast<RecordType>(resolved_type->unalias());
        if (rec->name.empty()) {
            rec->name = decl->name;
        }
    } else {
        resolved_type = std::make_shared<AliasType>(decl->name, resolved_type);
    }

    TypeSymbol sym(decl->name, resolved_type, decl->location);
    m_current_scope->declare_type(sym);
}

void DeclarationAnalyzer::analyze(VariableDeclaration* decl) {
    if (!decl) return;

    if (m_current_scope->lookup_variable(decl->name, false)) {
        report_error(decl->location, "Variable '" + decl->name + "' is already declared in this scope");
        return;
    }

    std::shared_ptr<Type> var_type = nullptr;

    if (decl->type) {
        var_type = resolve_type(decl->type.get());

        if (var_type->is_array()) {
            auto arr = std::static_pointer_cast<ArrayType>(var_type->unalias());
            if (arr->is_unsized()) {
                report_error(decl->location, "Array size cannot be omitted for variable declarations");
                var_type = PrimitiveType::get_error();
            }
        }
    }
    else if (decl->expression) {
        var_type = infer_expression_type(decl->expression.get());
        if (var_type->is_error()) {
            report_error(decl->location, "Cannot infer type for variable '" + decl->name + "'");
        }
    } else {
        report_error(decl->location, "Variable '" + decl->name + "' must have a type or an initializer");
        var_type = PrimitiveType::get_error();
    }

    VariableSymbol sym(decl->name, var_type, false, decl->location);
    m_current_scope->declare_variable(sym);
}

void DeclarationAnalyzer::analyze(RoutineDeclaration* decl) {
    if (!decl) return;

    if (!m_current_scope->is_global()) {
        report_error(decl->location, "Nested routines are not allowed (routine '" + decl->name + "')");
        return;
    }

    std::vector<VariableSymbol> parameters;
    std::unordered_set<std::string> param_names;

    for (const auto& param_node : decl->parameters) {
        if (param_names.find(param_node->name) != param_names.end()) {
            report_error(param_node->location, "Duplicate parameter name '" + param_node->name + "' in routine '" + decl->name + "'");
            continue;
        }
        param_names.insert(param_node->name);

        auto param_type = resolve_type(param_node->type.get());
        parameters.emplace_back(param_node->name, param_type, false, param_node->location);
    }

    std::shared_ptr<Type> return_type = decl->return_type
        ? resolve_type(decl->return_type.get())
        : PrimitiveType::get_void();

    bool is_forward_decl = (decl->body_type == RoutineBodyType::Forward);

    auto* existing = m_current_scope->lookup_routine(decl->name, false);

    if (existing) {
        if (existing->is_defined) {
            report_error(decl->location, "Routine '" + decl->name + "' is already defined");
            return;
        }

        if (is_forward_decl) {
            report_error(decl->location, "Duplicate forward declaration for routine '" + decl->name + "'");
            return;
        }

        if (parameters.size() != existing->parameters.size()) {
            report_error(decl->location, "Routine '" + decl->name + "' parameter count does not match forward declaration");
            return;
        }

        for (size_t i = 0; i < parameters.size(); ++i) {
            if (!parameters[i].type->equals(*existing->parameters[i].type)) {
                report_error(decl->location, "Parameter '" + parameters[i].name + "' type does not match forward declaration in routine '" + decl->name + "'");
                return;
            }
        }

        if (!return_type->equals(*existing->return_type)) {
            report_error(decl->location, "Return type of routine '" + decl->name + "' does not match forward declaration");
            return;
        }

        existing->is_defined = true;
    } else {
        RoutineSymbol sym(decl->name, parameters, return_type, is_forward_decl, !is_forward_decl, decl->location);
        m_current_scope->declare_routine(sym);
    }


    if (!is_forward_decl) {
        auto routine_scope = m_current_scope->create_child();

        for (const auto& param : parameters) {
            routine_scope->declare_variable(param);
        }

        auto prev_scope = m_current_scope;
        m_current_scope = routine_scope;

        if (decl->body_type == RoutineBodyType::Block) {
            for (const auto& item : decl->body) {
                if (auto* var_decl = dynamic_cast<VariableDeclaration*>(item.get())) {
                    analyze(var_decl);
                } else if (auto* type_decl = dynamic_cast<TypeDeclaration*>(item.get())) {
                    analyze(type_decl);
                }
            }
        }

        m_current_scope = prev_scope;
    }
}


void DeclarationAnalyzer::analyze(Program* program) {
    if (!program) return;

    for (const auto& item : program->declarations) {
        if (auto* var_decl = dynamic_cast<VariableDeclaration*>(item.get())) {
            analyze(var_decl);
        } else if (auto* type_decl = dynamic_cast<TypeDeclaration*>(item.get())) {
            analyze(type_decl);
        } else if (auto* routine_decl = dynamic_cast<RoutineDeclaration*>(item.get())) {
            analyze(routine_decl);
        }
    }

    for (const auto& [name, routine] : m_current_scope->get_routines()) {
        if (routine.is_forward && !routine.is_defined) {
            report_error(routine.location, "Routine '" + name + "' was forward-declared but never defined");
        }
    }
}

std::shared_ptr<Type> DeclarationAnalyzer::infer_expression_type(Expression* expr) {
    ExpressionAnalyzer analyzer(m_current_scope, m_errors);
    return analyzer.analyze(expr);
}