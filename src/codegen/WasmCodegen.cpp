#include "WasmCodegen.hpp"
#include "SemanticModel.hpp"

#include <stdexcept>

// ============================================================================
// Slot bindings and lookup
// ============================================================================

void WasmCodegen::begin_function() {
    locals_.clear();
}

void WasmCodegen::bind_local(const VariableSymbol& symbol,const std::string& wat_name) {
    locals_.emplace(&symbol, Place{PlaceKind::Local, wat_name});
}

void WasmCodegen::bind_global(const VariableSymbol& symbol, const std::string& wat_name) {
    globals_.emplace(&symbol, Place{PlaceKind::Global, wat_name});
}

const Place* WasmCodegen::find_place(const VariableSymbol& symbol) const {
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

const Place* WasmCodegen::place_of(const Identifier& identifier, const SemanticModel& model) const {
    const auto* symbol = model.variable_of(&identifier);
    if (!symbol) {
        return nullptr;
    }
    return find_place(*symbol);
}

// ============================================================================
// WAT declarations and instructions
// ============================================================================

std::string WasmCodegen::wat_value_type(const VariableSymbol& symbol) {
    const auto& type = symbol.type;
    if (!type) {
        throw std::logic_error("Missing type for variable '" + symbol.name + "'");
    }

    if (type->is_real()) {
        return "f64";
    }

    if (type->is_integer() || type->is_boolean() ||
        type->is_array() || type->is_record()) {
        return "i32";
    }

    throw std::logic_error("Unsupported WASM slot type for variable '" + symbol.name + "'");
}

std::string WasmCodegen::local_declaration(const VariableSymbol& symbol, const std::string& wat_name) {
    return "(local " + wat_name + " " + wat_value_type(symbol) + ")";
}

std::string WasmCodegen::global_declaration(const VariableSymbol& symbol, const std::string& wat_name) {
    const auto type = wat_value_type(symbol);
    return "(global " + wat_name + " (mut " + type + ") (" + type + ".const 0))";
}

std::string WasmCodegen::load_instruction(const Place& place) {
    if (place.kind == PlaceKind::Local) {
        return "local.get " + place.wat_name;
    }
    return "global.get " + place.wat_name;
}

std::string WasmCodegen::store_instruction(const Place& place) {
    if (place.kind == PlaceKind::Local) {
        return "local.set " + place.wat_name;
    }
    return "global.set " + place.wat_name;
}

std::string WasmCodegen::default_value_instruction(const VariableSymbol& symbol) {
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


void WasmCodegen::emit_local_variable(const VariableDeclaration& declaration, const SemanticModel& model,
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
        instructions.push_back(default_value_instruction(*symbol));
    }

    bind_local(*symbol, wat_name);
    local_declarations.push_back(slot_declaration);
    instructions.push_back(store_instruction(place));
}

void WasmCodegen::emit_assignment(const AssignStatement& assignment, const SemanticModel& model,
    std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_value) {
    const auto* target = dynamic_cast<const Identifier*>(assignment.target.get());
    if (!target) {
        throw std::logic_error("Assignment target must be a variable identifier");
    }

    const auto* place = place_of(*target, model);
    if (!place) {
        throw std::logic_error("Missing WASM slot for variable '" + target->name + "'");
    }

    instructions.push_back(emit_value(*assignment.value));
    instructions.push_back(store_instruction(*place));
}

void WasmCodegen::emit_global_variable(const VariableDeclaration& declaration, const SemanticModel& model,
    const std::string& wat_name, std::vector<std::string>& global_declarations,
    std::vector<std::string>& init_instructions, const std::function<std::string(const Expression&)>& emit_initializer) {
    const auto* symbol = model.variable_of(&declaration);
    if (!symbol) {
        throw std::logic_error(
            "Missing symbol for global variable '" + declaration.name + "'");
    }

    const auto slot_declaration = global_declaration(*symbol, wat_name);
    const Place place{PlaceKind::Global, wat_name};
    const auto initial_value = declaration.expression
        ? emit_initializer(*declaration.expression)
        : default_value_instruction(*symbol);

    bind_global(*symbol, wat_name);
    global_declarations.push_back(slot_declaration);
    init_instructions.push_back(initial_value);
    init_instructions.push_back(store_instruction(place));
}

void WasmCodegen::emit_globals(const Program& program, const SemanticModel& model,
    std::vector<std::string>& global_declarations, std::vector<std::string>& init_instructions,
    const std::function<std::string(const Expression&)>& emit_initializer) {
    for (const auto& declaration : program.declarations) {
        const auto* variable = dynamic_cast<const VariableDeclaration*>(declaration.get());
        if (!variable) {
            continue;
        }

        const auto wat_name = "$g" + std::to_string(global_declarations.size());
        emit_global_variable(
            *variable, model, wat_name, global_declarations,
            init_instructions, emit_initializer);
    }
}

