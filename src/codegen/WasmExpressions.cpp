#include "WasmCodegen.hpp"
#include "SemanticModel.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {

// Formats a double as a WAT f64 literal that round-trips the value exactly.
// Seventeen significant decimal digits are enough to round-trip any double.
std::string wat_real(double value) {
    std::ostringstream out;
    out << std::setprecision(17) << value;
    std::string text = out.str();
    const bool looks_integral =
        text.find('.') == std::string::npos &&
        text.find('e') == std::string::npos &&
        text.find('E') == std::string::npos &&
        text.find("inf") == std::string::npos &&
        text.find("nan") == std::string::npos;
    if (looks_integral) {
        text += ".0";
    }
    return text;
}

// Emits the two operands of a binary expression in source order, applying the
// conversions recorded by the semantic analyzer.
void emit_operands(WasmCodegen& codegen, const SemanticModel& model, BinaryExpr& binary) {
    codegen.emit_expr(*binary.left);
    codegen.emit_conversion(model.conversion_of(binary.left.get()));
    codegen.emit_expr(*binary.right);
    codegen.emit_conversion(model.conversion_of(binary.right.get()));
}

void emit_binary(WasmCodegen& codegen, WatWriter& writer,
                 const SemanticModel& model, BinaryExpr& binary) {
    const auto left_type = model.type_of(binary.left.get());
    const auto right_type = model.type_of(binary.right.get());
    const bool is_real =
        (left_type && left_type->is_real()) || (right_type && right_type->is_real());

    // and/or evaluate the right operand only when the left one decides it.
    if (binary.op == BinOp::And) {
        codegen.emit_expr(*binary.left);
        codegen.emit_conversion(model.conversion_of(binary.left.get()));
        writer.instruction("if (result i32)");
        codegen.emit_expr(*binary.right);
        codegen.emit_conversion(model.conversion_of(binary.right.get()));
        writer.instruction("else");
        writer.instruction("i32.const 0");
        writer.instruction("end");
        return;
    }

    if (binary.op == BinOp::Or) {
        codegen.emit_expr(*binary.left);
        codegen.emit_conversion(model.conversion_of(binary.left.get()));
        writer.instruction("if (result i32)");
        writer.instruction("i32.const 1");
        writer.instruction("else");
        codegen.emit_expr(*binary.right);
        codegen.emit_conversion(model.conversion_of(binary.right.get()));
        writer.instruction("end");
        return;
    }

    emit_operands(codegen, model, binary);

    switch (binary.op) {
        case BinOp::Plus:
            writer.instruction(is_real ? "f64.add" : "i32.add");
            break;
        case BinOp::Minus:
            writer.instruction(is_real ? "f64.sub" : "i32.sub");
            break;
        case BinOp::Mul:
            writer.instruction(is_real ? "f64.mul" : "i32.mul");
            break;
        case BinOp::Div:
            writer.instruction(is_real ? "f64.div" : "i32.div_s");
            break;
        case BinOp::Mod:
            writer.instruction("i32.rem_s");
            break;
        case BinOp::Less:
            writer.instruction(is_real ? "f64.lt" : "i32.lt_s");
            break;
        case BinOp::LessEqual:
            writer.instruction(is_real ? "f64.le" : "i32.le_s");
            break;
        case BinOp::Greater:
            writer.instruction(is_real ? "f64.gt" : "i32.gt_s");
            break;
        case BinOp::GreaterEqual:
            writer.instruction(is_real ? "f64.ge" : "i32.ge_s");
            break;
        case BinOp::Equal:
            writer.instruction(is_real ? "f64.eq" : "i32.eq");
            break;
        case BinOp::NotEqual:
            writer.instruction(is_real ? "f64.ne" : "i32.ne");
            break;
        case BinOp::Xor:
            writer.instruction("i32.xor");
            break;
        case BinOp::And:
        case BinOp::Or:
            // Handled above with short-circuiting.
            break;
    }
}

void emit_unary(WasmCodegen& codegen, WatWriter& writer,
                const SemanticModel& model, UnaryExpr& unary) {
    codegen.emit_expr(*unary.child);
    codegen.emit_conversion(model.conversion_of(unary.child.get()));

    switch (unary.op) {
        case UnaryOp::Not:
            writer.instruction("i32.eqz");
            break;
        case UnaryOp::Minus: {
            const auto child_type = model.type_of(unary.child.get());
            if (child_type && child_type->is_real()) {
                writer.instruction("f64.neg");
            } else {
                writer.instruction("i32.const -1");
                writer.instruction("i32.mul");
            }
            break;
        }
        case UnaryOp::Plus:
            // Unary plus keeps the value unchanged.
            break;
    }
}

void emit_call_expression(WasmCodegen& codegen, const SemanticModel& model,
                          RoutineCallExpr& call) {
    const auto* routine = model.routine_of(&call);
    if (!routine) {
        throw std::logic_error("Missing routine binding for call '" + call.routine_name + "'");
    }

    std::vector<Expression*> arguments;
    arguments.reserve(call.arguments.size());
    for (const auto& argument : call.arguments) {
        arguments.push_back(argument.get());
    }
    codegen.emit_call(*routine, arguments);
}

void emit_field_access(WasmCodegen& codegen, WatWriter& writer,
                       const SemanticModel& model, FieldAccessExpr& access) {
    if (access.field_name == "size") {
        const auto array_type = model.type_of(access.object.get());
        const auto resolved = array_type ? array_type->unalias() : nullptr;
        if (!resolved || !resolved->is_array()) {
            throw std::logic_error("'size' is only valid on arrays");
        }
        codegen.emit_expr(*access.object);
        writer.instruction("i32.load"); // header stores the element count
        return;
    }

    throw std::logic_error(
        "Record field access is not implemented yet (no record layout in WasmStorage)");
}

void emit_element_access(WasmCodegen& codegen, WatWriter& writer,
                         const SemanticModel& model, ElementAccessExpr& access) {
    const auto array_type = model.type_of(access.array.get());
    const auto resolved = array_type ? array_type->unalias() : nullptr;
    const auto* array = dynamic_cast<const ArrayType*>(resolved.get());
    if (!array) {
        throw std::logic_error("Missing array type for element access");
    }
    const auto element_type = model.type_of(&access);
    if (!element_type) {
        throw std::logic_error("Missing element type for array access");
    }

    const auto address = writer.temporary("i32");
    const auto index = writer.temporary("i32");

    codegen.emit_expr(*access.array);
    writer.instruction("local.set " + address);
    codegen.emit_expr(*access.index);
    writer.instruction("local.set " + index);

    std::vector<std::string> address_instructions;
    WasmStorage::emit_array_element_address(*array, address, index, address_instructions);
    for (const auto& instruction : address_instructions) {
        writer.instruction(instruction);
    }

    writer.instruction(WasmStorage::memory_load_instruction(*element_type));
}

} // namespace

void WasmCodegen::emit_expr(Expression& expression) {
    if (auto* literal = dynamic_cast<IntLiteral*>(&expression)) {
        writer_.instruction("i32.const " + std::to_string(literal->value));
        return;
    }
    if (auto* literal = dynamic_cast<RealLiteral*>(&expression)) {
        writer_.instruction("f64.const " + wat_real(literal->value));
        return;
    }
    if (auto* literal = dynamic_cast<BoolLiteral*>(&expression)) {
        writer_.instruction(literal->value ? "i32.const 1" : "i32.const 0");
        return;
    }
    if (auto* identifier = dynamic_cast<Identifier*>(&expression)) {
        const auto* place = storage_.place_of(*identifier, *model_);
        if (!place) {
            throw std::logic_error("Missing WASM slot for variable '" + identifier->name + "'");
        }
        writer_.instruction(WasmStorage::load_instruction(*place));
        return;
    }
    if (auto* unary = dynamic_cast<UnaryExpr*>(&expression)) {
        emit_unary(*this, writer_, *model_, *unary);
        return;
    }
    if (auto* binary = dynamic_cast<BinaryExpr*>(&expression)) {
        emit_binary(*this, writer_, *model_, *binary);
        return;
    }
    if (auto* call = dynamic_cast<RoutineCallExpr*>(&expression)) {
        emit_call_expression(*this, *model_, *call);
        return;
    }
    if (auto* access = dynamic_cast<FieldAccessExpr*>(&expression)) {
        emit_field_access(*this, writer_, *model_, *access);
        return;
    }
    if (auto* access = dynamic_cast<ElementAccessExpr*>(&expression)) {
        emit_element_access(*this, writer_, *model_, *access);
        return;
    }

    throw std::logic_error("Unsupported expression in WASM codegen");
}

void WasmCodegen::emit_conversion(ConversionKind conversion) {
    switch (conversion) {
        case ConversionKind::None:
        case ConversionKind::BooleanToInteger:
            // Boolean is already represented as 0/1 in i32.
            return;

        case ConversionKind::IntegerToReal:
        case ConversionKind::BooleanToReal:
            writer_.instruction("f64.convert_i32_s");
            return;

        case ConversionKind::IntegerToBoolean: {
            const auto temporary = writer_.temporary("i32");
            writer_.instruction("local.set " + temporary);
            writer_.instruction("local.get " + temporary);
            writer_.instruction("i32.eqz"); // value == 0
            writer_.instruction("local.get " + temporary);
            writer_.instruction("i32.const 1");
            writer_.instruction("i32.eq"); // value == 1
            writer_.instruction("i32.or"); // (value == 0) || (value == 1)
            writer_.instruction("if");
            writer_.instruction("unreachable");
            writer_.instruction("end");
            writer_.instruction("local.get " + temporary);
            return;
        }

        case ConversionKind::RealToInteger: {
            // Round half away from zero (A10), then trap on out-of-range values.
            const auto temporary = writer_.temporary("f64");
            writer_.instruction("local.set " + temporary);
            writer_.instruction("local.get " + temporary);
            writer_.instruction("f64.const 0");
            writer_.instruction("f64.lt"); // value < 0
            writer_.instruction("if (result f64)");
            writer_.instruction("local.get " + temporary);
            writer_.instruction("f64.const 0.5");
            writer_.instruction("f64.sub");
            writer_.instruction("f64.ceil");
            writer_.instruction("else");
            writer_.instruction("local.get " + temporary);
            writer_.instruction("f64.const 0.5");
            writer_.instruction("f64.add");
            writer_.instruction("f64.floor");
            writer_.instruction("end");
            writer_.instruction("i32.trunc_f64_s");
            return;
        }
    }
}

void WasmCodegen::emit_call(const RoutineSymbol& routine,
                            const std::vector<Expression*>& arguments) {
    for (auto* argument : arguments) {
        emit_expr(*argument);
        emit_conversion(model_->conversion_of(argument));
    }

    const auto found = function_ids_.find(&routine);
    if (found == function_ids_.end()) {
        throw std::logic_error("Missing function id for routine '" + routine.name + "'");
    }
    writer_.instruction("call " + found->second);
}
