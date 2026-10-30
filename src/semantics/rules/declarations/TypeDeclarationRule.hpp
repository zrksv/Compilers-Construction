#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class TypeDeclarationRule : public StatementRule<TypeDeclaration> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(TypeDeclaration& node, const Context& ctx) override {
        if (!ctx.scope) {
            return StmtResult{};
        }

        if (ctx.scope->lookup_type(node.name, false)) {
            error(node.location, "Type '" + node.name + "' is already declared in this scope");
            return StmtResult{};
        }

        auto resolved_type = m_dispatcher.type(node.type.get(), ctx);
        if (resolved_type->is_record()) {
            auto rec = std::static_pointer_cast<RecordType>(resolved_type->unalias());
            if (rec->name.empty()) {
                rec->name = node.name;
            }
        } else {
            resolved_type = std::make_shared<AliasType>(node.name, resolved_type);
        }

        TypeSymbol sym(node.name, resolved_type, node.location);
        ctx.scope->declare_type(sym);
        return StmtResult{};
    }
};
