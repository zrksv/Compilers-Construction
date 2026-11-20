#include "WasmCodegen.hpp"
#include "SemanticModel.hpp"

#include <stdexcept>

const SemanticModel& WasmCodegen::model() const {
    if (!model_) { throw std::logic_error("WASM generation requires a semantic model"); }
    return *model_;
}

void WasmCodegen::emit_initialized_value(Expression& expression) {
    emit_expr(expression);
    emit_conversion(model().conversion_of(&expression));
}

void WasmCodegen::emit_trap(const Location& location, WasmTrapKind kind) {
    std::vector<std::string> instructions;
    WasmStorage::emit_trap(location, kind, instructions);
    append_storage({}, instructions);
}

std::string WasmCodegen::capture_expr(const Expression& expression, bool convert) {
    return writer_.capture_instructions([&] {
        auto& node = const_cast<Expression&>(expression);
        if (convert) { emit_initialized_value(node); }
        else { emit_expr(node); }
    });
}

void WasmCodegen::append_storage(const std::vector<std::string>& locals, const std::vector<std::string>& instructions) {
    for (const auto& local : locals) { writer_.local_declaration(local); }
    for (const auto& instruction : instructions) { writer_.instruction(instruction); }
}

Place WasmCodegen::emit_place(Expression& expression) {
    std::vector<std::string> locals, instructions;
    const auto place = storage_.emit_place(expression, model(), locals, instructions,
        [&](const Expression& value) { return capture_expr(value, false); });
    append_storage(locals, instructions);
    return place;
}

void WasmCodegen::emit_load(Expression& expression) {
    const auto place = emit_place(expression);
    writer_.instruction(WasmStorage::load_instruction(place));
}

void WasmCodegen::emit_globals(Program& program) {
    std::vector<std::string> globals, locals, instructions;
    storage_.emit_globals(program, model(), globals, locals, instructions,
        [&](const Expression& value) { return capture_expr(value, true); });
    for (const auto& global : globals) { writer_.module_item(global); }
    append_storage(locals, instructions);
}

void WasmCodegen::visit(Identifier& node) { emit_load(node); }
void WasmCodegen::visit(ElementAccessExpr& node) { emit_load(node); }

void WasmCodegen::visit(FieldAccessExpr& node) {
    const auto object_type = model().type_of(node.object.get());
    if (object_type && object_type->is_array() && node.field_name == "size") {
        emit_expr(*node.object);
        std::vector<std::string> instructions;
        WasmStorage::emit_array_size(instructions);
        append_storage({}, instructions);
    } else {
        emit_load(node);
    }
}

void WasmCodegen::visit(VariableDeclaration& node) {
    std::vector<std::string> locals, instructions;
    const auto name = "$local" + std::to_string(function_.next_local++);
    storage_.emit_local_variable(node, model(), name, locals, instructions,
        [&](const Expression& value) { return capture_expr(value, true); });
    append_storage(locals, instructions);
}

void WasmCodegen::visit(AssignStatement& node) {
    std::vector<std::string> locals, instructions;
    storage_.emit_assignment(node, model(), locals, instructions,
        [&](const Expression& value) { return capture_expr(value, true); });
    append_storage(locals, instructions);
}

void WasmCodegen::visit(ParameterDeclaration& node) {
    const auto* symbol = model().variable_of(&node);
    if (!symbol) { throw std::logic_error("Missing semantic parameter binding"); }
    storage_.bind_local(*symbol, "$p" + std::to_string(function_.next_parameter++));
}

void WasmCodegen::visit(TypeDeclaration&) {}
void WasmCodegen::visit(PrimitiveTypeNode&) {}
void WasmCodegen::visit(IdentifierTypeNode&) {}
void WasmCodegen::visit(ArrayTypeNode&) {}
void WasmCodegen::visit(RecordTypeNode&) {}
