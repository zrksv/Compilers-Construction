#include "WasmCodegen.hpp"
#include "SemanticModel.hpp"

#include <stdexcept>
#include <utility>

const SemanticModel& WasmCodegen::model() const {
    if (!model_) {
        throw std::logic_error("WASM generation requires a semantic model");
    }
    return *model_;
}

void WasmCodegen::prepare(Program& program, const SemanticModel& semantic_model) {
    writer_.reset();
    storage_ = WasmStorage{};
    function_ids_.clear();
    model_ = &semantic_model;

    std::size_t function_index = 0;
    for (const auto& declaration : program.declarations) {
        const auto* routine_node = dynamic_cast<const RoutineDeclaration*>(declaration.get());
        if (!routine_node || routine_node->body_type == RoutineBodyType::Forward) {
            continue;
        }

        const auto* routine = semantic_model.routine_of(routine_node);
        if (routine && !function_ids_.contains(routine)) {
            function_ids_.emplace(routine, "$routine" + std::to_string(function_index++));
        }
    }
}

const std::string& WasmCodegen::function_id(const RoutineSymbol& routine) const {
    const auto found = function_ids_.find(&routine);
    if (found == function_ids_.end()) {
        throw std::logic_error("Missing function id for routine '" + routine.name + "'");
    }
    return found->second;
}

void WasmCodegen::begin_function_state(const RoutineSymbol* routine) {
    function_ = {};
    function_.routine = routine;
    function_.result_type = routine ? routine->return_type.get() : nullptr;
    storage_.begin_function();
}

std::string WasmCodegen::new_label() {
    return "$label" + std::to_string(function_.next_label++);
}

void WasmCodegen::emit_body(const std::vector<std::unique_ptr<AstNode>>& body) {
    for (const auto& node : body) {
        node->accept(*this);
    }
}

std::string WasmCodegen::generate_wat(Program& program, const SemanticModel& semantic_model) {
    prepare(program, semantic_model);
    writer_.module_item("(import \"env\" \"print_i32\" (func " + std::string(WasmHost::print_i32) + " (param i32)))");
    writer_.module_item("(import \"env\" \"print_f64\" (func " + std::string(WasmHost::print_f64) + " (param f64)))");
    writer_.module_item("(import \"env\" \"print_bool\" (func " + std::string(WasmHost::print_bool) + " (param i32)))");
    writer_.module_item("(import \"env\" \"print_space\" (func " + std::string(WasmHost::print_space) + "))");
    writer_.module_item("(import \"env\" \"print_newline\" (func " + std::string(WasmHost::print_newline) + "))");
    writer_.module_item(WasmStorage::memory_runtime());
    program.accept(*this);
    return writer_.finish_module();
}

void WasmCodegen::visit(Program& node) {
    begin_function_state();
    writer_.begin_function("$init");
    emit_globals(node);
    writer_.end_function();

    for (const auto& declaration : node.declarations) {
        if (auto* routine = dynamic_cast<RoutineDeclaration*>(declaration.get())) {
            routine->accept(*this);
        }
    }
    writer_.module_item("(start $init)");
}

void WasmCodegen::visit(RoutineDeclaration& node) {
    if (node.body_type == RoutineBodyType::Forward) {
        return;
    }

    const auto* routine = model().routine_of(&node);
    if (!routine) {
        throw std::logic_error("Missing routine symbol '" + node.name + "'");
    }

    begin_function_state(routine);
    std::string header = function_id(*routine);
    for (std::size_t index = 0; index < node.parameters.size(); ++index) {
        const auto* parameter = model().variable_of(node.parameters[index].get());
        if (!parameter) {
            throw std::logic_error("Missing parameter binding");
        }
        header += " (param $p" + std::to_string(index) + " " + WasmStorage::wat_value_type(*parameter) + ")";
    }
    if (routine->return_type && !routine->return_type->is_void()) {
        header += " (result " + WasmStorage::wat_value_type(*routine->return_type) + ")";
    }

    writer_.begin_function(header);
    for (const auto& parameter : node.parameters) {
        parameter->accept(*this);
    }
    if (node.body_type == RoutineBodyType::Expression) {
        emit_expr(*node.expression_body);
        emit_conversion(model().conversion_of(node.expression_body.get()));
        writer_.instruction("return");
    } else {
        emit_body(node.body);
    }
    if (routine->return_type && !routine->return_type->is_void()) {
        writer_.instruction("unreachable");
    }
    writer_.end_function();
    writer_.module_item("(export \"" + node.name + "\" (func " + function_id(*routine) + "))");
}

void WasmCodegen::visit(IfStatement& node) {
    emit_expr(*node.condition);
    emit_conversion(model().conversion_of(node.condition.get()));
    writer_.instruction("if");
    emit_body(node.then_body);
    if (!node.else_body.empty()) {
        writer_.instruction("else");
        emit_body(node.else_body);
    }
    writer_.instruction("end");
}

void WasmCodegen::visit(WhileStatement& node) {
    const auto exit_label = new_label();
    const auto repeat_label = new_label();
    writer_.instruction("block " + exit_label);
    writer_.instruction("loop " + repeat_label);
    emit_expr(*node.condition);
    emit_conversion(model().conversion_of(node.condition.get()));
    writer_.instruction("i32.eqz");
    writer_.instruction("br_if " + exit_label);
    emit_body(node.body);
    writer_.instruction("br " + repeat_label);
    writer_.instruction("end");
    writer_.instruction("end");
}

void WasmCodegen::visit(ForStatement& node) {
    const auto* iterator = model().variable_of(&node);
    if (!iterator) {
        throw std::logic_error("Missing for iterator binding");
    }

    const auto iterator_local = "$for" + std::to_string(function_.next_local++);
    writer_.local_declaration(WasmStorage::local_declaration(*iterator, iterator_local));
    storage_.bind_local(*iterator, iterator_local);

    const auto exit_label = new_label();
    const auto repeat_label = new_label();
    if (node.range_end) {
        emit_range_for(node, iterator_local, exit_label, repeat_label);
    } else {
        emit_array_for(node, iterator_local, exit_label, repeat_label);
    }
}

void WasmCodegen::emit_range_for(ForStatement& node, const std::string& iterator_local,
                                 const std::string& exit_label, const std::string& repeat_label) {
    const auto lower_local = "$for_lower" + std::to_string(function_.next_local++);
    const auto upper_local = "$for_upper" + std::to_string(function_.next_local++);
    writer_.local_declaration("(local " + lower_local + " i32)");
    writer_.local_declaration("(local " + upper_local + " i32)");

    emit_expr(*node.range_start);
    emit_conversion(model().conversion_of(node.range_start.get()));
    writer_.instruction("local.set " + lower_local);
    emit_expr(*node.range_end);
    emit_conversion(model().conversion_of(node.range_end.get()));
    writer_.instruction("local.set " + upper_local);

    writer_.instruction("block " + exit_label);
    writer_.instruction("local.get " + lower_local);
    writer_.instruction("local.get " + upper_local);
    writer_.instruction("i32.gt_s");
    writer_.instruction("br_if " + exit_label);
    writer_.instruction("local.get " + (node.is_reverse ? upper_local : lower_local));
    writer_.instruction("local.set " + iterator_local);
    writer_.instruction("loop " + repeat_label);
    emit_body(node.body);
    writer_.instruction("local.get " + iterator_local);
    writer_.instruction("local.get " + (node.is_reverse ? lower_local : upper_local));
    writer_.instruction("i32.eq");
    writer_.instruction("br_if " + exit_label);
    writer_.instruction("local.get " + iterator_local);
    writer_.instruction("i32.const 1");
    writer_.instruction(node.is_reverse ? "i32.sub" : "i32.add");
    writer_.instruction("local.set " + iterator_local);
    writer_.instruction("br " + repeat_label);
    writer_.instruction("end");
    writer_.instruction("end");
}

void WasmCodegen::emit_array_for(ForStatement& node, const std::string& iterator_local,
                                 const std::string& exit_label, const std::string& repeat_label) {
    const auto array_local = "$for_array" + std::to_string(function_.next_local++);
    const auto index_local = "$for_index" + std::to_string(function_.next_local++);
    const auto length_local = "$for_length" + std::to_string(function_.next_local++);
    writer_.local_declaration("(local " + array_local + " i32)");
    writer_.local_declaration("(local " + index_local + " i32)");
    writer_.local_declaration("(local " + length_local + " i32)");

    emit_expr(*node.range_start);
    writer_.instruction("local.set " + array_local);
    writer_.instruction("local.get " + array_local);
    writer_.instruction("i32.load");
    writer_.instruction("local.set " + length_local);
    if (node.is_reverse) {
        writer_.instruction("local.get " + length_local);
        writer_.instruction("i32.const 1");
        writer_.instruction("i32.sub");
    } else {
        writer_.instruction("i32.const 0");
    }
    writer_.instruction("local.set " + index_local);

    const auto array_type_ref = model().type_of(node.range_start.get());
    const auto resolved_type = array_type_ref ? array_type_ref->unalias() : nullptr;
    const auto* array_type = dynamic_cast<const ArrayType*>(resolved_type.get());
    if (!array_type) {
        throw std::logic_error("Missing array type for for loop");
    }
    const auto layout = WasmStorage::array_layout(*array_type);

    writer_.instruction("block " + exit_label);
    writer_.instruction("loop " + repeat_label);
    writer_.instruction("local.get " + index_local);
    writer_.instruction("local.get " + length_local);
    writer_.instruction("i32.ge_u");
    writer_.instruction("br_if " + exit_label);

    writer_.instruction("local.get " + array_local);
    writer_.instruction("i32.const " + std::to_string(layout.elements_offset));
    writer_.instruction("i32.add");
    writer_.instruction("local.get " + index_local);
    writer_.instruction("i32.const " + std::to_string(layout.element_stride));
    writer_.instruction("i32.mul");
    writer_.instruction("i32.add");
    writer_.instruction(WasmStorage::memory_load_instruction(*array_type->element_type));
    writer_.instruction("local.set " + iterator_local);
    emit_body(node.body);

    if (node.is_reverse) {
        writer_.instruction("local.get " + index_local);
        writer_.instruction("i32.eqz");
        writer_.instruction("br_if " + exit_label);
    }
    writer_.instruction("local.get " + index_local);
    writer_.instruction(node.is_reverse ? "i32.const -1" : "i32.const 1");
    writer_.instruction("i32.add");
    writer_.instruction("local.set " + index_local);
    writer_.instruction("br " + repeat_label);
    writer_.instruction("end");
    writer_.instruction("end");
}

void WasmCodegen::visit(ReturnStatement& node) {
    if (node.expression) {
        emit_expr(*node.expression);
        emit_conversion(model().conversion_of(node.expression.get()));
    }
    writer_.instruction("return");
}

void WasmCodegen::visit(RoutineCallStmt& node) {
    const auto* routine = model().routine_of(&node);
    if (!routine) {
        throw std::logic_error("Missing routine binding for statement call");
    }
    std::vector<Expression*> arguments;
    for (const auto& argument : node.arguments) {
        arguments.push_back(argument.get());
    }
    emit_call(*routine, arguments);
    if (!routine->is_procedure()) {
        writer_.instruction("drop");
    }
}

void WasmCodegen::visit(PrintStatement& node) {
    struct PrintArgument {
        std::string local;
        std::string import;
    };
    std::vector<PrintArgument> arguments;
    arguments.reserve(node.to_print.size());

    for (auto& expression : node.to_print) {
        emit_expr(*expression);
        emit_conversion(model().conversion_of(expression.get()));
        auto type = model().type_of(expression.get());
        if (!type) {
            throw std::logic_error("Missing print type");
        }
        type = type->unalias();
        const auto local = "$printarg" + std::to_string(function_.next_local++);
        writer_.local_declaration("(local " + local + " " + WasmStorage::wat_value_type(*type) + ")");
        writer_.instruction("local.set " + local);

        const auto import = type->is_real() ? WasmHost::print_f64
            : type->is_boolean() ? WasmHost::print_bool : WasmHost::print_i32;
        arguments.push_back({local, import});
    }

    for (std::size_t index = 0; index < arguments.size(); ++index) {
        if (index != 0) {
            writer_.instruction("call " + std::string(WasmHost::print_space));
        }
        writer_.instruction("local.get " + arguments[index].local);
        writer_.instruction("call " + arguments[index].import);
    }
    writer_.instruction("call " + std::string(WasmHost::print_newline));
}
