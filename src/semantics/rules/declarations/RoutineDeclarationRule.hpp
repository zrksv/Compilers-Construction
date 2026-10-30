#pragma once
#include <unordered_set>
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class RoutineDeclarationRule : public StatementRule<RoutineDeclaration> {
private:
    RoutineSymbol* declare_header(RoutineDeclaration& node, const Context& ctx) {
        if (!ctx.scope || !ctx.scope->is_global()) {
            error(node.location, "Nested routines are not allowed (routine '" + node.name + "')");
            return nullptr;
        }

        std::vector<VariableSymbol> parameters;
        std::unordered_set<std::string> param_names;

        for (const auto& param_node : node.parameters) {
            if (param_names.count(param_node->name)) {
                error(param_node->location, "Duplicate parameter name '" + param_node->name +
                      "' in routine '" + node.name + "'");
                continue;
            }
            param_names.insert(param_node->name);

            auto param_type = m_dispatcher.type(param_node->type.get(),
                                                ctx.with_type_use(TypeUse::Parameter));
            parameters.emplace_back(param_node->name, param_type, false, param_node->location);
        }

        TypeRef return_type = node.return_type
            ? m_dispatcher.type(node.return_type.get(), ctx.with_type_use(TypeUse::Result))
            : PrimitiveType::get_void();

        bool is_forward_decl = (node.body_type == RoutineBodyType::Forward);
        auto* existing = ctx.scope->lookup_routine(node.name, false);

        if (!existing && ctx.scope->is_declared_locally(node.name)) {
            error(node.location, "Name '" + node.name + "' is already declared in this scope");
            return nullptr;
        }

        if (existing) {
            if (existing->is_defined) {
                error(node.location, "Routine '" + node.name + "' is already defined");
                return nullptr;
            }

            if (is_forward_decl) {
                error(node.location, "Duplicate forward declaration for routine '" + node.name + "'");
                return nullptr;
            }

            if (parameters.size() != existing->parameters.size()) {
                error(node.location, "Routine '" + node.name + "' parameter count does not match forward declaration");
                return nullptr;
            }

            for (size_t i = 0; i < parameters.size(); ++i) {
                if (!parameters[i].type->equals(*existing->parameters[i].type)) {
                    error(node.location, "Parameter '" + parameters[i].name +
                          "' type does not match forward declaration in routine '" + node.name + "'");
                    return nullptr;
                }
            }

            if (!return_type->equals(*existing->return_type)) {
                error(node.location, "Return type of routine '" + node.name + "' does not match forward declaration");
                return nullptr;
            }

            existing->is_defined = true;
            return existing;
        }

        RoutineSymbol sym(node.name, parameters, return_type, is_forward_decl, !is_forward_decl, node.location);
        ctx.scope->declare_routine(sym);
        return ctx.scope->lookup_routine(node.name, false);
    }

    void analyze_body(RoutineDeclaration& node, const Context& ctx, RoutineSymbol* sym) {
        if (!sym || node.body_type == RoutineBodyType::Forward) {
            return;
        }

        RoutineInfo info{sym->name, sym->return_type};
        Context body_ctx = ctx.enter_routine(&info);

        for (const auto& param : sym->parameters) {
            body_ctx.scope->declare_variable(param);
        }

        if (node.body_type == RoutineBodyType::Expression) {
            if (sym->return_type->is_void()) {
                error(node.location, "Procedure '" + sym->name + "' cannot have an expression body");
                return;
            }
            if (node.expression_body) {
                auto expr_res = m_dispatcher.expression(node.expression_body.get(), body_ctx);
                if (!expr_res.is_error()) {
                    convert(sym->return_type, expr_res.type, node.expression_body.get(), node.location);
                }
            }
        } else if (node.body_type == RoutineBodyType::Block) {
            m_dispatcher.statement_list(node.body, body_ctx);
        }
    }

public:
    using StatementRule::StatementRule;

    StmtResult analyze(RoutineDeclaration& node, const Context& ctx) override {
        RoutineSymbol* sym = declare_header(node, ctx);
        analyze_body(node, ctx, sym);
        return StmtResult{};
    }
};
