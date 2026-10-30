#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

// Rule for resolving array types with size or open parameter arrays
class ArrayTypeRule : public TypeRule<ArrayTypeNode> {
public:
    using TypeRule::TypeRule;

    TypeRef resolve(ArrayTypeNode& node, const Context& ctx) override {
        auto elem_type = m_dispatcher.type(node.element_type.get(), ctx.with_type_use(TypeUse::Element));

        // Array without explicit size (array [] T) - only valid for routine parameters
        if (!node.size) {
            if (!ctx.allows_unsized_array()) {
                error(node.location, "Array size can only be omitted for routine parameters");
                return PrimitiveType::get_error();
            }
            return m_services.types.make_array(elem_type, std::nullopt);
        }

        // Array with size expression (must be an integer constant > 0)
        if (auto* int_lit = dynamic_cast<IntLiteral*>(node.size.get())) {
            if (int_lit->value <= 0) {
                error(node.size->location, "Array size must be greater than zero");
                return m_services.types.make_array(elem_type, 1);
            }
            return m_services.types.make_array(elem_type, static_cast<size_t>(int_lit->value));
        }

        error(node.size->location, "Array size must be a compile-time integer constant");
        return PrimitiveType::get_error();
    }
};
