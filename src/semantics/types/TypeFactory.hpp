#pragma once
#include "Type.hpp"
#include <string>
#include <vector>
#include <optional>

class TypeFactory {
private:
    size_t m_next_id = 1;

public:
    TypeRef make_array(TypeRef element_type, std::optional<size_t> size = std::nullopt) {
        size_t id = size.has_value() ? m_next_id++ : 0;
        return std::make_shared<ArrayType>(std::move(element_type), size, id);
    }

    TypeRef make_record(std::string name, std::vector<RecordField> fields) {
        return std::make_shared<RecordType>(std::move(name), std::move(fields), m_next_id++);
    }
};
