#include "rules/types/IdentifierTypeRule.hpp"

TypeRef IdentifierTypeRule::resolve(IdentifierTypeNode& node, const Context& ctx) {
    if (!ctx.scope) {
        return PrimitiveType::get_error();
    }

    auto* symbol = ctx.scope->lookup_type(node.name);
    if (!symbol) {
        error(node.location, "Type '" + node.name + "' is not declared");
        return PrimitiveType::get_error();
    }

    return symbol->type;
}
