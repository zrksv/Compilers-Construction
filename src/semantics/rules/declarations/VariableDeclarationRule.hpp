#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class VariableDeclarationRule : public StatementRule<VariableDeclaration> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(VariableDeclaration& node, const Context& ctx) override {
        if (!ctx.scope) {
            return StmtResult{};
        }

        if (ctx.scope->is_declared_locally(node.name)) {
            error(node.location, "Name '" + node.name + "' is already declared in this scope");
            return StmtResult{};
        }

        // Check initializer before declaring variable in scope (A15)
        ExprResult init_res;
        if (node.expression) {
            init_res = m_dispatcher.expression(node.expression.get(), ctx);
        }

        TypeRef var_type = nullptr;
        if (node.type) {
            var_type = m_dispatcher.type(node.type.get(), ctx.with_type_use(TypeUse::Variable));

            if (var_type && var_type->is_array()) {
                auto arr = std::static_pointer_cast<ArrayType>(var_type->unalias());
                if (arr->is_unsized()) {
                    error(node.location, "Array size cannot be omitted for variable declarations");
                    var_type = PrimitiveType::get_error();
                }
            }

            if (node.expression && var_type && !var_type->is_error()) {
                convert(var_type, init_res.type, node.expression.get());
            }
        } else if (node.expression) {
            var_type = init_res.type;
            if (!var_type || var_type->is_error()) {
                if (!init_res.is_error()) {
                    error(node.location, "Cannot infer type for variable '" + node.name + "'");
                }
                var_type = PrimitiveType::get_error();
            }
        } else {
            error(node.location, "Variable '" + node.name + "' must have a type or an initializer");
            var_type = PrimitiveType::get_error();
        }

        VariableSymbol sym(node.name, var_type, false, node.location);
        ctx.scope->declare_variable(sym);
        return StmtResult{};
    }
};
