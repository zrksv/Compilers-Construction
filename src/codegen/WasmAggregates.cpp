#include "WasmStorage.hpp"
#include "SemanticModel.hpp"

#include <stdexcept>

RecordLayout WasmStorage::record_layout(const RecordType& record) {
    RecordLayout layout{};
    std::uint64_t offset = 0;
    for (const auto& field : record.fields) {
        if (!field.type) {
            throw std::logic_error("Missing type for record field '" + field.name + "'");
        }
        const std::uint64_t width = wat_value_type(*field.type) == "f64" ? 8 : 4;
        offset = (offset + width - 1) / width * width;
        layout.field_offsets.push_back(offset);
        offset += width;
    }
    // Empty records must still receive distinct non-null object addresses.
    layout.allocation_size = offset == 0 ? 8 : (offset + 7) / 8 * 8;
    return layout;
}

Place WasmStorage::emit_record_field_address(const FieldAccessExpr& access, const SemanticModel& model,
    std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_value) {
    const auto type = model.type_of(access.object.get());
    const auto resolved = type ? type->unalias() : nullptr;
    const auto* record = dynamic_cast<const RecordType*>(resolved.get());
    if (!record) {
        throw std::logic_error("Missing record type for field access");
    }
    const auto layout = record_layout(*record);
    for (std::size_t i = 0; i < record->fields.size(); ++i) {
        const auto& field = record->fields[i];
        if (field.name != access.field_name) {
            continue;
        }
        instructions.push_back(emit_value(*access.object));
        // An object larger than memory32 cannot pass $alloc, so this cast is
        // only reached at runtime for representable field offsets.
        instructions.push_back("i32.const " + std::to_string(static_cast<std::uint32_t>(layout.field_offsets[i])));
        instructions.push_back("i32.add");
        return {PlaceKind::Memory, {}, field.type.get()};
    }
    throw std::logic_error("Missing record field '" + access.field_name + "'");
}

// Every creation leaves one value: a primitive value or an aggregate reference.
void WasmStorage::emit_default_value(Type& type, std::vector<std::string>& local_declarations,
    std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_initializer) {
    const auto resolved = type.unalias();
    if (const auto* array = dynamic_cast<const ArrayType*>(resolved.get())) {
        emit_array_creation(*array, local_declarations, instructions, emit_initializer);
    } else if (const auto* record = dynamic_cast<const RecordType*>(resolved.get())) {
        emit_record_creation(*record, local_declarations, instructions, emit_initializer);
    } else {
        const auto value_type = wat_value_type(*resolved);
        instructions.push_back(value_type + ".const 0");
    }
}

void WasmStorage::emit_record_creation(const RecordType& record, std::vector<std::string>& local_declarations,
    std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_initializer) {
    const auto layout = record_layout(record);
    const auto address = add_i32_temporary(local_declarations);
    instructions.push_back("i64.const " + std::to_string(layout.allocation_size));
    instructions.push_back("call $alloc");
    instructions.push_back("local.set " + address);

    for (std::size_t i = 0; i < record.fields.size(); ++i) {
        const auto& field = record.fields[i];
        instructions.push_back("local.get " + address);
        instructions.push_back("i32.const " + std::to_string(static_cast<std::uint32_t>(layout.field_offsets[i])));
        instructions.push_back("i32.add");

        if (field.declaration && field.declaration->expression) {
            instructions.push_back(emit_initializer(*field.declaration->expression));
        } else {
            emit_default_value(*field.type, local_declarations, instructions, emit_initializer);
        }
        instructions.push_back(memory_store_instruction(*field.type));
    }
    instructions.push_back("local.get " + address);
}

void WasmStorage::emit_array_allocation(const ArrayType& array, const std::string& address_local,
    std::vector<std::string>& instructions) {
    const auto bytes = array_allocation_size(array);
    instructions.push_back("i64.const " + std::to_string(bytes));
    instructions.push_back("call $alloc");
    instructions.push_back("local.tee " + address_local);
    instructions.push_back("i32.const " + std::to_string(*array.size));
    instructions.push_back("i32.store");
}

void WasmStorage::emit_array_creation(const ArrayType& array, std::vector<std::string>& local_declarations,
    std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_initializer) {
    const auto layout = array_layout(array);
    const auto address = add_i32_temporary(local_declarations);
    if (!array.element_type->is_array() && !array.element_type->is_record()) {
        emit_primitive_array_creation(array, address, instructions);
        return;
    }
    emit_array_allocation(array, address, instructions);
    const auto index = add_i32_temporary(local_declarations);
    instructions.push_back("i32.const 0");
    instructions.push_back("local.set " + index);
    instructions.push_back("block");
    instructions.push_back("loop");
    instructions.push_back("local.get " + index);
    instructions.push_back("i32.const " + std::to_string(*array.size));
    instructions.push_back("i32.ge_u");
    instructions.push_back("br_if 1");

    instructions.push_back("local.get " + address);
    instructions.push_back("i32.const " + std::to_string(layout.elements_offset));
    instructions.push_back("i32.add");
    instructions.push_back("local.get " + index);
    instructions.push_back("i32.const " + std::to_string(layout.element_stride));
    instructions.push_back("i32.mul");
    instructions.push_back("i32.add");
    emit_default_value(*array.element_type, local_declarations, instructions, emit_initializer);
    instructions.push_back("i32.store");

    instructions.push_back("local.get " + index);
    instructions.push_back("i32.const 1");
    instructions.push_back("i32.add");
    instructions.push_back("local.set " + index);
    instructions.push_back("br 0");
    instructions.push_back("end");
    instructions.push_back("end");
    instructions.push_back("local.get " + address);
}
