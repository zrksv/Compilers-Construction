#pragma once
#include<memory>
#include <string>
#include<optional>
#include<vector>

enum class TypeKind{
    Integer,
    Real,
    Boolean,
    Void,
    Error,
    Array,
    Record,
    Alias
};

class Type : public std::enable_shared_from_this<Type> {
public:
    virtual ~Type() = default;
    virtual TypeKind kind() const = 0;
    virtual std::string to_string() const = 0;

    virtual bool equals(const Type& other) const = 0;
    virtual std::shared_ptr<Type> unalias();
    bool is_integer() const;
    bool is_real() const;
    bool is_boolean() const;
    bool is_numeric() const;
    bool is_void() const;
    bool is_error() const;
    bool is_array() const;
    bool is_record() const;
};

// ============================================================================
// Primitive types (integer, real, boolean, void, error)
// ============================================================================

class PrimitiveType : public Type {
private:
    TypeKind m_kind;

public:
    explicit PrimitiveType(TypeKind kind) : m_kind(kind) {}
    TypeKind kind() const override {
        return m_kind;
    }
    std::string to_string() const override;
    bool equals(const Type& other) const override;

    static std::shared_ptr<PrimitiveType> get_integer();
    static std::shared_ptr<PrimitiveType> get_real();
    static std::shared_ptr<PrimitiveType> get_boolean();
    static std::shared_ptr<PrimitiveType> get_void();
    static std::shared_ptr<PrimitiveType> get_error();
};

// ============================================================================
// Arrays
// ============================================================================
class ArrayType : public Type {
public:
    std::shared_ptr<Type> element_type;
    std::optional<size_t> size;
    size_t type_id;

    ArrayType(std::shared_ptr<Type> elem_type,
              std::optional<size_t> sz = std::nullopt,
              size_t id = 0)
        : element_type(std::move(elem_type)), size(sz), type_id(id) {}

    TypeKind kind() const override { return TypeKind::Array; }
    std::string to_string() const override;
    bool equals(const Type& other) const override;

    bool is_unsized() const {
        return !size.has_value();
    }
};

// ============================================================================
// Record
// ============================================================================
struct RecordField {
    std::string name;
    std::shared_ptr<Type> type;
};

class RecordType : public Type {
public:
    std::string name;
    std::vector<RecordField> fields;
    size_t type_id;

    RecordType(std::string name, std::vector<RecordField> fields, size_t id = 0)
        : name(std::move(name)), fields(std::move(fields)), type_id(id) {}

    TypeKind kind() const override { return TypeKind::Record; }
    std::string to_string() const override;
    bool equals(const Type& other) const override;

    const RecordField* find_field(const std::string& field_name) const;
};

// ============================================================================
// Type Alias
// ============================================================================
class AliasType : public Type {
public:
    std::string name;
    std::shared_ptr<Type> target_type;
    AliasType(std::string name, std::shared_ptr<Type> target)
        : name(std::move(name)), target_type(std::move(target)) {}

    TypeKind kind() const override { return TypeKind::Alias; }
    std::string to_string() const override { return name; }
    bool equals(const Type& other) const override;

    std::shared_ptr<Type> unalias() override;
};