#include "WasmStorage.hpp"
#include "SemanticModel.hpp"

#include <stdexcept>
#include <limits>

// ============================================================================
// Slot bindings and lookup
// ============================================================================

void WasmStorage::begin_function() {
    locals_.clear();
    next_temporary_ = 0;
}

std::string WasmStorage::add_i32_temporary(std::vector<std::string>& local_declarations) {
    const auto name = "$tmp" + std::to_string(next_temporary_++);
    local_declarations.push_back("(local " + name + " i32)");
    return name;
}

void WasmStorage::bind_local(const VariableSymbol& symbol,const std::string& wat_name) {
    locals_.emplace(&symbol, Place{PlaceKind::Local, wat_name});
}

void WasmStorage::bind_global(const VariableSymbol& symbol, const std::string& wat_name) {
    globals_.emplace(&symbol, Place{PlaceKind::Global, wat_name});
}

const Place* WasmStorage::find_place(const VariableSymbol& symbol) const {
    const auto local = locals_.find(&symbol);
    if (local != locals_.end()) {
        return &local->second;
    }

    const auto global = globals_.find(&symbol);
    if (global != globals_.end()) {
        return &global->second;
    }

    return nullptr;
}

const Place* WasmStorage::place_of(const Identifier& identifier, const SemanticModel& model) const {
    const auto* symbol = model.variable_of(&identifier);
    if (!symbol) {
        return nullptr;
    }
    return find_place(*symbol);
}

Place WasmStorage::emit_place(const Expression& expression, const SemanticModel& model,
    std::vector<std::string>& local_declarations, std::vector<std::string>& instructions,
    const std::function<std::string(const Expression&)>& emit_value) {
    if (const auto* identifier = dynamic_cast<const Identifier*>(&expression)) {
        const auto* place = place_of(*identifier, model);
        if (!place) {
            throw std::logic_error("Missing WASM slot for variable '" + identifier->name + "'");
        }
        return *place;
    }

    if (const auto* access = dynamic_cast<const ElementAccessExpr*>(&expression)) {
        const auto type = model.type_of(access);
        if (!type) {
            throw std::logic_error("Missing element type for array access");
        }

        emit_array_element_address(*access, model, local_declarations, instructions, emit_value);
        return {PlaceKind::Memory, {}, type.get()};
    }

    if (const auto* access = dynamic_cast<const FieldAccessExpr*>(&expression)) {
        return emit_record_field_address(*access, model, instructions, emit_value);
    }

    throw std::logic_error("Unsupported storage expression");
}

// ============================================================================
// WAT declarations and instructions
// ============================================================================

std::string WasmStorage::wat_value_type(const VariableSymbol& symbol) {
    if (!symbol.type) {
        throw std::logic_error("Missing type for variable '" + symbol.name + "'");
    }
    return wat_value_type(*symbol.type);
}

std::string WasmStorage::wat_value_type(const Type& type) {
    if (type.is_real()) {
        return "f64";
    }
    if (type.is_integer() || type.is_boolean() || type.is_array() || type.is_record()) {
        return "i32";
    }
    throw std::logic_error("Unsupported WASM value type: " + type.to_string());
}

std::string WasmStorage::memory_load_instruction(const Type& type) {
    return wat_value_type(type) + ".load";
}

std::string WasmStorage::memory_store_instruction(const Type& type) {
    return wat_value_type(type) + ".store";
}

std::string WasmStorage::local_declaration(const VariableSymbol& symbol, const std::string& wat_name) {
    return "(local " + wat_name + " " + wat_value_type(symbol) + ")";
}

std::string WasmStorage::global_declaration(const VariableSymbol& symbol, const std::string& wat_name) {
    const auto type = wat_value_type(symbol);
    return "(global " + wat_name + " (mut " + type + ") (" + type + ".const 0))";
}

std::string WasmStorage::load_instruction(const Place& place) {
    if (place.kind == PlaceKind::Memory) {
        return memory_load_instruction(*place.value_type);
    }
    if (place.kind == PlaceKind::Local) {
        return "local.get " + place.wat_name;
    }
    return "global.get " + place.wat_name;
}

std::string WasmStorage::store_instruction(const Place& place) {
    if (place.kind == PlaceKind::Memory) {
        return memory_store_instruction(*place.value_type);
    }
    if (place.kind == PlaceKind::Local) {
        return "local.set " + place.wat_name;
    }
    return "global.set " + place.wat_name;
}

std::string WasmStorage::default_value_instruction(const VariableSymbol& symbol) {
    const auto& type = symbol.type;
    if (type && type->is_real()) {
        return "f64.const 0";
    }
    if (type && (type->is_integer() || type->is_boolean())) {
        return "i32.const 0";
    }
    throw std::logic_error("Expected a primitive type for variable '" + symbol.name + "'");
}

// ============================================================================
// Variable declarations and assignments
// ============================================================================


void WasmStorage::emit_local_variable(const VariableDeclaration& declaration, const SemanticModel& model,
        const std::string& wat_name, std::vector<std::string>& local_declarations,
        std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_initializer) {
    const auto* symbol = model.variable_of(&declaration);
    if (!symbol) {
        throw std::logic_error(
            "Missing symbol for local variable '" + declaration.name + "'");
    }

    const Place place{PlaceKind::Local, wat_name};
    const auto slot_declaration = local_declaration(*symbol, wat_name);

    if (declaration.expression) {
        instructions.push_back(emit_initializer(*declaration.expression));
    } else {
        if (!symbol->type) {
            throw std::logic_error("Missing type for variable '" + symbol->name + "'");
        }
        emit_default_value(*symbol->type, declaration.location, local_declarations, instructions, emit_initializer);
    }

    bind_local(*symbol, wat_name);
    local_declarations.push_back(slot_declaration);
    instructions.push_back(store_instruction(place));
}

void WasmStorage::emit_assignment(const AssignStatement& assignment, const SemanticModel& model,
    std::vector<std::string>& local_declarations, std::vector<std::string>& instructions,
    const std::function<std::string(const Expression&)>& emit_value) {
    const auto place = emit_place(*assignment.target, model, local_declarations, instructions, emit_value);
    instructions.push_back(emit_value(*assignment.value));
    instructions.push_back(store_instruction(place));
}

void WasmStorage::emit_global_variable(const VariableDeclaration& declaration, const SemanticModel& model,
    const std::string& wat_name, std::vector<std::string>& global_declarations,
    std::vector<std::string>& init_local_declarations, std::vector<std::string>& init_instructions,
    const std::function<std::string(const Expression&)>& emit_initializer) {
    const auto* symbol = model.variable_of(&declaration);
    if (!symbol) {
        throw std::logic_error(
            "Missing symbol for global variable '" + declaration.name + "'");
    }

    const auto slot_declaration = global_declaration(*symbol, wat_name);
    const Place place{PlaceKind::Global, wat_name};
    if (declaration.expression) {
        init_instructions.push_back(emit_initializer(*declaration.expression));
    } else {
        if (!symbol->type) {
            throw std::logic_error("Missing type for variable '" + symbol->name + "'");
        }
        emit_default_value(*symbol->type, declaration.location, init_local_declarations, init_instructions, emit_initializer);
    }

    bind_global(*symbol, wat_name);
    global_declarations.push_back(slot_declaration);
    init_instructions.push_back(store_instruction(place));
}

void WasmStorage::emit_globals(const Program& program, const SemanticModel& model,
    std::vector<std::string>& global_declarations, std::vector<std::string>& init_local_declarations,
    std::vector<std::string>& init_instructions,
    const std::function<std::string(const Expression&)>& emit_initializer) {
    for (const auto& declaration : program.declarations) {
        const auto* variable = dynamic_cast<const VariableDeclaration*>(declaration.get());
        if (!variable) {
            continue;
        }

        const auto wat_name = "$g" + std::to_string(global_declarations.size());
        emit_global_variable(*variable, model, wat_name, global_declarations,
            init_local_declarations, init_instructions, emit_initializer);
    }
}

// ============================================================================
// Array layout in linear memory
// ============================================================================


ArrayLayout WasmStorage::array_layout(const ArrayType& array) {
    const auto& element = array.element_type;
    if (element && element->is_real()) {
        return {8, 8};
    }
    if (element && (element->is_integer() || element->is_boolean() ||
                    element->is_array() || element->is_record())) {
        return {8, 4};
                    }
    throw std::logic_error("Unsupported array element type");
}

std::uint64_t WasmStorage::array_allocation_size(const ArrayType& array) {
    if (!array.size) {
        throw std::logic_error("Cannot allocate an array without a fixed length");
    }

    const auto layout = array_layout(array);
    return layout.elements_offset +
           static_cast<std::uint64_t>(*array.size) * layout.element_stride;
}

void WasmStorage::emit_source_location(const Location& location, std::vector<std::string>& instructions) {
    if (location.row > std::numeric_limits<std::uint32_t>::max() ||
        location.column > std::numeric_limits<std::uint32_t>::max()) {
        throw std::logic_error("Source location exceeds the WASM diagnostic range");
    }
    instructions.push_back("i32.const " + std::to_string(location.row));
    instructions.push_back("i32.const " + std::to_string(location.column));
}

void WasmStorage::emit_trap(const Location& location, WasmTrapKind kind, std::vector<std::string>& instructions) {
    emit_source_location(location, instructions);
    instructions.push_back("i32.const " + std::to_string(static_cast<std::uint32_t>(kind)));
    instructions.push_back("call $trap");
    instructions.push_back("unreachable");
}

void WasmStorage::emit_primitive_array_creation(const ArrayType& array,
    const std::string& address_local, std::vector<std::string>& instructions, const Location& location) {
    if (array.element_type->is_array() || array.element_type->is_record()) {
        throw std::logic_error("Expected a primitive array element type");
    }

    emit_array_allocation(array, address_local, instructions, location);
    instructions.push_back("local.get " + address_local);
}

void WasmStorage::emit_array_size(std::vector<std::string>& instructions) {
    instructions.push_back("i32.load");
}

void WasmStorage::emit_array_index_check(const std::string& address_local,
    const std::string& index_local, std::vector<std::string>& instructions, const Location& location) {
    instructions.push_back("local.get " + index_local);
    instructions.push_back("i32.const 1");
    instructions.push_back("i32.lt_s");

    instructions.push_back("local.get " + index_local);
    instructions.push_back("local.get " + address_local);
    emit_array_size(instructions);
    instructions.push_back("i32.gt_s");
    instructions.push_back("i32.or");

    instructions.push_back("if");
    emit_trap(location, WasmTrapKind::ArrayBounds, instructions);
    instructions.push_back("end");
}

void WasmStorage::emit_array_element_address(const ArrayType& array, const std::string& address_local,
    const std::string& index_local, std::vector<std::string>& instructions, const Location& location) {
    const auto layout = array_layout(array);
    emit_array_index_check(address_local, index_local, instructions, location);

    instructions.push_back("local.get " + address_local);
    instructions.push_back("i32.const " + std::to_string(layout.elements_offset));
    instructions.push_back("i32.add");

    instructions.push_back("local.get " + index_local);
    instructions.push_back("i32.const 1");
    instructions.push_back("i32.sub");
    instructions.push_back("i32.const " + std::to_string(layout.element_stride));
    instructions.push_back("i32.mul");

    instructions.push_back("i32.add");
}

void WasmStorage::emit_array_element_address(const ElementAccessExpr& access, const SemanticModel& model,
    std::vector<std::string>& local_declarations, std::vector<std::string>& instructions,
    const std::function<std::string(const Expression&)>& emit_value) {
    const auto type = model.type_of(access.array.get());
    const auto resolved_type = type ? type->unalias() : nullptr;
    const auto* array_type = dynamic_cast<const ArrayType*>(resolved_type.get());
    if (!array_type) {
        throw std::logic_error("Missing array type for element access");
    }

    const auto address_local = add_i32_temporary(local_declarations);
    const auto index_local = add_i32_temporary(local_declarations);

    instructions.push_back(emit_value(*access.array));
    instructions.push_back("local.set " + address_local);
    instructions.push_back(emit_value(*access.index));
    instructions.push_back("local.set " + index_local);

    emit_array_element_address(*array_type, address_local, index_local, instructions, access.location);
}

std::string WasmStorage::memory_runtime() {
    return R"wat(
(import "host" "report_trap" (func $report_trap (param i32 i32 i32)))
(func $trap (param $row i32) (param $column i32) (param $kind i32)
    local.get $row
    local.get $column
    local.get $kind
    call $report_trap
    unreachable
)
(memory 1)
(global $heap (mut i64) (i64.const 8))

(func $alloc (param $bytes i64) (param $row i32) (param $column i32) (result i32)
    (local $address i64)
    (local $end i64)
    (local $required_pages i32)

    local.get $bytes
    i64.eqz
    local.get $bytes
    i64.const 4294967296
    i64.gt_u
    i32.or
    if
        local.get $row
        local.get $column
        i32.const 2
        call $trap
        unreachable
    end

    global.get $heap
    local.set $address

    local.get $address
    local.get $bytes
    i64.add
    i64.const 7
    i64.add
    i64.const -8
    i64.and
    local.set $end

    local.get $end
    i64.const 4294967296
    i64.gt_u
    if
        local.get $row
        local.get $column
        i32.const 2
        call $trap
        unreachable
    end

    local.get $end
    i64.const 65535
    i64.add
    i64.const 16
    i64.shr_u
    i32.wrap_i64
    local.set $required_pages

    local.get $required_pages
    memory.size
    i32.gt_u
    if
        local.get $required_pages
        memory.size
        i32.sub
        memory.grow
        i32.const -1
        i32.eq
        if
            local.get $row
            local.get $column
            i32.const 2
            call $trap
            unreachable
        end
    end

    local.get $end
    global.set $heap

    local.get $address
    i32.wrap_i64
)
)wat";
}
