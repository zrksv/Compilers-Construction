//
// Created by annaz on 29.10.2026.
//

#include "Type.hpp"

bool Type::is_integer() const {
    return const_cast<Type*>(this)->unalias()->kind() == TypeKind::Integer;
}

bool Type::is_real() const {
    return const_cast<Type*>(this)->unalias()->kind() == TypeKind::Real;
}

bool Type::is_boolean() const {
    return const_cast<Type*>(this)->unalias()->kind() == TypeKind::Boolean;
}

bool Type::is_numeric() const {
    return is_integer() || is_real();
}

bool Type::is_void() const {
    return const_cast<Type*>(this)->unalias()->kind() == TypeKind::Void;
}

bool Type::is_error() const {
    return const_cast<Type*>(this)->unalias()->kind() == TypeKind::Error;
}

bool Type::is_array() const {
    return const_cast<Type*>(this)->unalias()->kind() == TypeKind::Array;
}

bool Type::is_record() const {
    return const_cast<Type*>(this)->unalias()->kind() == TypeKind::Record;
}

std::shared_ptr<Type> Type::unalias() {
    return shared_from_this();
}

// ============================================================================
// PrimitiveType
// ============================================================================
std::string PrimitiveType::to_string() const {
    switch (m_kind) {
        case TypeKind::Integer: return "integer";
        case TypeKind::Real:    return "real";
        case TypeKind::Boolean: return "boolean";
        case TypeKind::Void:    return "void";
        case TypeKind::Error:   return "<error-type>";
        default:                return "<unknown-primitive>";
    }
}

bool PrimitiveType::equals(const Type& other) const {
    auto other_unaliased = const_cast<Type&>(other).unalias();
    return other_unaliased->kind() == m_kind;
}

std::shared_ptr<PrimitiveType> PrimitiveType::get_integer() {
    static auto instance = std::make_shared<PrimitiveType>(TypeKind::Integer);
    return instance;
}

std::shared_ptr<PrimitiveType> PrimitiveType::get_real() {
    static auto instance = std::make_shared<PrimitiveType>(TypeKind::Real);
    return instance;
}

std::shared_ptr<PrimitiveType> PrimitiveType::get_boolean() {
    static auto instance = std::make_shared<PrimitiveType>(TypeKind::Boolean);
    return instance;
}

std::shared_ptr<PrimitiveType> PrimitiveType::get_void() {
    static auto instance = std::make_shared<PrimitiveType>(TypeKind::Void);
    return instance;
}

std::shared_ptr<PrimitiveType> PrimitiveType::get_error() {
    static auto instance = std::make_shared<PrimitiveType>(TypeKind::Error);
    return instance;
}

// ============================================================================
// ArrayType
// ============================================================================
std::string ArrayType::to_string() const {
    std::string res = "array [";
    if (size.has_value()) {
        res += std::to_string(*size);
    }
    res += "] " + (element_type ? element_type->to_string() : "<unknown>");
    return res;
}

bool ArrayType::equals(const Type& other) const {
    auto other_u = const_cast<Type&>(other).unalias();
    if (other_u->kind() != TypeKind::Array) {
        return false;
    }
    auto other_arr = std::static_pointer_cast<ArrayType>(other_u);

    if (type_id != 0 && other_arr->type_id != 0) {
        return type_id == other_arr->type_id;
    }

    if (is_unsized() || other_arr->is_unsized()) {
        return element_type && other_arr->element_type &&
               element_type->equals(*other_arr->element_type);
    }

    return size == other_arr->size &&
           element_type && other_arr->element_type &&
           element_type->equals(*other_arr->element_type);
}

// ============================================================================
// RecordType
// ============================================================================
std::string RecordType::to_string() const {
    if (!name.empty()) {
        return name;
    }

    std::string res = "record { ";
    for (const auto& f : fields) {
        res += f.name + ": " + (f.type ? f.type->to_string() : "<unknown>") + "; ";
    }
    res += "}";
    return res;
}

bool RecordType::equals(const Type& other) const {
    auto other_u = const_cast<Type&>(other).unalias();
    if (other_u->kind() != TypeKind::Record) {
        return false;
    }
    auto other_rec = std::static_pointer_cast<RecordType>(other_u);

    return type_id == other_rec->type_id;
}

const RecordField* RecordType::find_field(const std::string& field_name) const {
    for (const auto& f : fields) {
        if (f.name == field_name) {
            return &f;
        }
    }
    return nullptr;
}

// ============================================================================
// AliasType
// ============================================================================
std::shared_ptr<Type> AliasType::unalias() {
    auto current = shared_from_this();
    int depth = 0;
    const int MAX_DEPTH = 64;

    while (current->kind() == TypeKind::Alias) {
        if (++depth > MAX_DEPTH) {
            return PrimitiveType::get_error();
        }

        auto alias = std::static_pointer_cast<AliasType>(current);
        if (!alias->target_type) {
            break;
        }
        current = alias->target_type;
    }
    return current;
}

bool AliasType::equals(const Type& other) const {
    auto my_base = const_cast<AliasType*>(this)->unalias();
    auto other_base = const_cast<Type&>(other).unalias();
    return my_base->equals(*other_base);
}