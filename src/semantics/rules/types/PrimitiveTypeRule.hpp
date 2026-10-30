#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class PrimitiveTypeRule : public TypeRule<PrimitiveTypeNode> {
public:
    using TypeRule::TypeRule;

    TypeRef resolve(PrimitiveTypeNode& node, const Context& ctx) override {
        (void)ctx;
        if (node.type_name == "integer") {
            return PrimitiveType::get_integer();
        }
        if (node.type_name == "real") {
            return PrimitiveType::get_real();
        }
        if (node.type_name == "boolean") {
            return PrimitiveType::get_boolean();
        }

        error(node.location, "Unknown primitive type: " + node.type_name);
        return PrimitiveType::get_error();
    }
};
