#pragma once
#include <unordered_set>
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class RecordTypeRule : public TypeRule<RecordTypeNode> {
public:
    using TypeRule::TypeRule;

    TypeRef resolve(RecordTypeNode& node, const Context& ctx) override {
        std::vector<RecordField> fields;
        std::unordered_set<std::string> seen_fields;

        for (const auto& decl : node.declarations) {
            if (seen_fields.find(decl->name) != seen_fields.end()) {
                error(decl->location, "Duplicate field '" + decl->name + "' in record");
                continue;
            }
            seen_fields.insert(decl->name);

            TypeRef field_type = nullptr;
            if (decl->type) {
                field_type = m_dispatcher.type(decl->type.get(), ctx.with_type_use(TypeUse::Field));
                if (decl->expression) {
                    auto expr_res = m_dispatcher.expression(decl->expression.get(), ctx);
                    if (!expr_res.is_error() && field_type && !field_type->is_error()) {
                        convert(field_type, expr_res.type, decl->expression.get(), decl->expression->location);
                    }
                }
            } else if (decl->expression) {
                auto expr_res = m_dispatcher.expression(decl->expression.get(), ctx);
                field_type = expr_res.type;
                if (!field_type || field_type->is_error()) {
                    if (!expr_res.is_error()) {
                        error(decl->location, "Cannot infer type for field '" + decl->name + "'");
                    }
                    field_type = PrimitiveType::get_error();
                }
            } else {
                error(decl->location, "Field '" + decl->name + "' must have a type or an initializer");
                field_type = PrimitiveType::get_error();
            }

            if (field_type && field_type->is_array()) {
                auto arr = std::static_pointer_cast<ArrayType>(field_type->unalias());
                if (arr->is_unsized()) {
                    error(decl->location, "Record field cannot be an unsized array");
                }
            }

            fields.push_back(RecordField{decl->name, field_type});
        }

        return m_services.types.make_record("", std::move(fields));
    }
};
