#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"
#include "SourceManager.hpp"
#include "WasmStorage.hpp"
#include "symbols/Symbol.hpp"
#include "types/TypeFactory.hpp"

#include <cassert>
#include <stdexcept>

// A bound parameter resolves to its named local slot.
static void test_parameter_resolves_to_local_slot() {
    VariableSymbol parameter("x", PrimitiveType::get_integer());
    WasmStorage storage;
    storage.bind_local(parameter, "$p0");

    const auto* place = storage.find_place(parameter);
    assert(place);
    assert(place->kind == PlaceKind::Local);
    assert(place->wat_name == "$p0");
}

// Distinct symbols with the same name have independent slot bindings.
static void test_same_name_symbols_have_distinct_slots() {
    VariableSymbol parameter("x", PrimitiveType::get_integer());
    VariableSymbol shadow("x", PrimitiveType::get_integer());
    WasmStorage storage;
    storage.bind_local(parameter, "$p0");

    assert(storage.find_place(shadow) == nullptr);

    storage.bind_local(shadow, "$local0");
    const auto* parameter_place = storage.find_place(parameter);
    const auto* shadow_place = storage.find_place(shadow);
    assert(parameter_place && parameter_place->wat_name == "$p0");
    assert(shadow_place && shadow_place->wat_name == "$local0");
}

// Reading an analyzed parameter finds the slot assigned to its declaration.
static void test_parameter_read_resolves_to_declaration_slot() {
    SourceManager source("routine identity(x : integer) : integer => x\n");
    Lexer lexer(source);
    ParserDriver parser;
    assert(parser.parse(lexer));

    auto* program = dynamic_cast<Program*>(parser.get_ast());
    assert(program && program->declarations.size() == 1);
    SemanticAnalyzer analyzer;
    assert(analyzer.analyze(program));

    const auto* routine = dynamic_cast<const RoutineDeclaration*>(program->declarations.front().get());
    assert(routine && routine->parameters.size() == 1);
    const auto* read = dynamic_cast<const Identifier*>(routine->expression_body.get());
    assert(read);

    const auto& model = analyzer.get_model();
    const auto* declaration_symbol = model.variable_of(routine->parameters.front().get());
    assert(declaration_symbol);

    WasmStorage storage;
    storage.bind_local(*declaration_symbol, "$p0");
    const auto* place = storage.place_of(*read, model);
    assert(place && place->kind == PlaceKind::Local && place->wat_name == "$p0");
}

// An explicit initializer is emitted once and stored in the slot read by return.
static void test_local_variable_with_initializer() {
    SourceManager source(R"(routine value() : integer is
    var x : integer is 7
    return x
end
)");
    Lexer lexer(source);
    ParserDriver parser;
    const bool parsed = parser.parse(lexer);
    assert(parsed);

    auto* program = dynamic_cast<Program*>(parser.get_ast());
    assert(program && program->declarations.size() == 1);
    SemanticAnalyzer analyzer;
    const bool analyzed = analyzer.analyze(program);
    assert(analyzed);

    const auto* routine = dynamic_cast<const RoutineDeclaration*>(program->declarations.front().get());
    assert(routine && routine->body.size() == 2);
    const auto* declaration = dynamic_cast<const VariableDeclaration*>(routine->body[0].get());
    const auto* result = dynamic_cast<const ReturnStatement*>(routine->body[1].get());
    assert(declaration && result);
    const auto* read = dynamic_cast<const Identifier*>(result->expression.get());
    assert(read);

    const auto& model = analyzer.get_model();
    const auto* symbol = model.variable_of(declaration);
    assert(symbol);

    WasmStorage storage;
    std::vector<std::string> locals;
    std::vector<std::string> instructions;
    int initializer_calls = 0;
    storage.emit_local_variable(*declaration, model, "$local0", locals, instructions,
        [&](const Expression& expression) {
            assert(&expression == declaration->expression.get());
            ++initializer_calls;
            return "i32.const 7";
        });

    assert(initializer_calls == 1);
    assert((locals == std::vector<std::string>{"(local $local0 i32)"}));
    assert((instructions == std::vector<std::string>{"i32.const 7", "local.set $local0"}));
    const auto* place = storage.place_of(*read, model);
    assert(place && place->kind == PlaceKind::Local && place->wat_name == "$local0");
}

// A real local without an initializer emits a zero store at the declaration.
static void test_local_variable_without_initializer() {
    SourceManager source(R"(routine reset() is
    var x : real
end
)");
    Lexer lexer(source);
    ParserDriver parser;
    const bool parsed = parser.parse(lexer);
    assert(parsed);
    auto* program = dynamic_cast<Program*>(parser.get_ast());
    assert(program && program->declarations.size() == 1);

    SemanticAnalyzer analyzer;
    const bool analyzed = analyzer.analyze(program);
    assert(analyzed);
    const auto* routine = dynamic_cast<const RoutineDeclaration*>(program->declarations.front().get());
    assert(routine && routine->body.size() == 1);
    const auto* declaration = dynamic_cast<const VariableDeclaration*>(routine->body.front().get());
    assert(declaration && !declaration->expression);

    WasmStorage storage;
    std::vector<std::string> locals;
    std::vector<std::string> instructions;
    storage.emit_local_variable(*declaration, analyzer.get_model(), "$local0", locals, instructions, {});

    assert((locals == std::vector<std::string>{"(local $local0 f64)"}));
    assert((instructions == std::vector<std::string>{"f64.const 0", "local.set $local0"}));
}

// A local slot is read with local.get.
static void test_load_local_instruction() {
    const Place place{PlaceKind::Local, "$p0"};
    assert(WasmStorage::load_instruction(place) == "local.get $p0");
}

// A global slot is read with global.get.
static void test_load_global_instruction() {
    const Place place{PlaceKind::Global, "$g0"};
    assert(WasmStorage::load_instruction(place) == "global.get $g0");
}

// A local slot is written with local.set.
static void test_store_local_instruction() {
    const Place place{PlaceKind::Local, "$local0"};
    assert(WasmStorage::store_instruction(place) == "local.set $local0");
}

// A global slot is written with global.set.
static void test_store_global_instruction() {
    const Place place{PlaceKind::Global, "$g0"};
    assert(WasmStorage::store_instruction(place) == "global.set $g0");
}

// Integer and boolean use i32 slots, while real uses f64.
static void test_wat_primitive_value_types() {
    const VariableSymbol integer("count", PrimitiveType::get_integer());
    const VariableSymbol boolean("flag", PrimitiveType::get_boolean());
    const VariableSymbol real("distance", PrimitiveType::get_real());

    assert(WasmStorage::wat_value_type(integer) == "i32");
    assert(WasmStorage::wat_value_type(boolean) == "i32");
    assert(WasmStorage::wat_value_type(real) == "f64");
}

// Aggregate slots store i32 addresses even when their contents include f64 values.
static void test_wat_aggregate_value_types() {
    TypeFactory types;
    const auto real = PrimitiveType::get_real();
    const VariableSymbol array("items", types.make_array(real, 2));
    const VariableSymbol record("point", types.make_record("Point", {{"x", real}}));

    assert(WasmStorage::wat_value_type(array) == "i32");
    assert(WasmStorage::wat_value_type(record) == "i32");
}

// A type alias uses the same WASM representation as its target.
static void test_wat_alias_value_type() {
    const VariableSymbol distance("length", std::make_shared<AliasType>("Distance", PrimitiveType::get_real()));
    assert(WasmStorage::wat_value_type(distance) == "f64");
}

// Missing types and types without a slot representation raise an internal error.
static void test_wat_rejects_invalid_slot_types() {
    const VariableSymbol missing("missing", nullptr);
    const VariableSymbol unsupported("unsupported", PrimitiveType::get_void());

    for (const auto* symbol : {&missing, &unsupported}) {
        bool rejected = false;
        try {
            WasmStorage::wat_value_type(*symbol);
        } catch (const std::logic_error&) {
            rejected = true;
        }
        assert(rejected);
    }
}

// Local declarations include the slot name and its WASM value type.
static void test_local_declaration() {
    const VariableSymbol integer("count", PrimitiveType::get_integer());
    const VariableSymbol real("distance", PrimitiveType::get_real());

    assert(WasmStorage::local_declaration(integer, "$local0") == "(local $local0 i32)");
    assert(WasmStorage::local_declaration(real, "$local1") == "(local $local1 f64)");
}

// Primitive defaults push zero using the slot's WASM representation.
static void test_default_primitive_values() {
    const VariableSymbol integer("count", PrimitiveType::get_integer());
    const VariableSymbol real("distance", PrimitiveType::get_real());
    const VariableSymbol boolean("flag", PrimitiveType::get_boolean());

    assert(WasmStorage::default_value_instruction(integer) == "i32.const 0");
    assert(WasmStorage::default_value_instruction(real) == "f64.const 0");
    assert(WasmStorage::default_value_instruction(boolean) == "i32.const 0");
}

// Assignment appends one value and writes it to the existing slot.
static void test_assignment_updates_existing_slot() {
    const VariableSymbol symbol("x", PrimitiveType::get_integer());
    AssignStatement assignment("x", std::make_unique<IntLiteral>(9, Location{}), Location{});
    SemanticModel model;
    model.bind_variable(assignment.target.get(), &symbol);

    WasmStorage storage;
    storage.bind_local(symbol, "$local0");
    std::vector<std::string> locals;
    std::vector<std::string> instructions{"i32.const 7", "local.set $local0"};
    int value_calls = 0;
    storage.emit_assignment(assignment, model, locals, instructions, [&](const Expression& value) {
        assert(&value == assignment.value.get());
        ++value_calls;
        return "i32.const 9";
    });

    assert(value_calls == 1);
    assert(locals.empty());
    assert((instructions == std::vector<std::string>{
        "i32.const 7", "local.set $local0", "i32.const 9", "local.set $local0"}));
}

// A new function can reuse a slot name without retaining the previous binding.
static void test_begin_function_clears_local_bindings() {
    const VariableSymbol first("x", PrimitiveType::get_integer());
    const VariableSymbol second("x", PrimitiveType::get_integer());
    WasmStorage storage;
    storage.bind_local(first, "$local0");
    assert(storage.find_place(first));

    storage.begin_function();
    storage.bind_local(second, "$local0");

    assert(storage.find_place(first) == nullptr);
    const auto* place = storage.find_place(second);
    assert(place && place->kind == PlaceKind::Local && place->wat_name == "$local0");
}

// Global declarations use mutable slots with typed zero initializers.
static void test_global_declaration() {
    const VariableSymbol integer("count", PrimitiveType::get_integer());
    const VariableSymbol real("distance", PrimitiveType::get_real());

    assert(WasmStorage::global_declaration(integer, "$g0") ==
           "(global $g0 (mut i32) (i32.const 0))");
    assert(WasmStorage::global_declaration(real, "$g1") ==
           "(global $g1 (mut f64) (f64.const 0))");
}

// A routine reads the module's global slot after its local bindings are reset.
static void test_global_read_from_routine() {
    SourceManager source("var count : integer is 7\nroutine read() : integer => count\n");
    Lexer lexer(source);
    ParserDriver parser;
    const bool parsed = parser.parse(lexer);
    assert(parsed);

    auto* program = dynamic_cast<Program*>(parser.get_ast());
    assert(program && program->declarations.size() == 2);
    SemanticAnalyzer analyzer;
    const bool analyzed = analyzer.analyze(program);
    assert(analyzed);

    const auto* routine = dynamic_cast<const RoutineDeclaration*>(program->declarations[1].get());
    assert(routine);
    const auto* read = dynamic_cast<const Identifier*>(routine->expression_body.get());
    assert(read);

    const auto& model = analyzer.get_model();
    const auto* symbol = model.variable_of(program->declarations.front().get());
    assert(symbol);

    WasmStorage storage;
    storage.bind_global(*symbol, "$g0");
    storage.begin_function();
    const auto* place = storage.place_of(*read, model);
    assert(place && place->kind == PlaceKind::Global);
    assert(WasmStorage::load_instruction(*place) == "global.get $g0");
}

// The global pass skips type declarations and emits dependent initializers in source order.
static void test_global_initializers_preserve_source_order() {
    SourceManager source("var count : integer is 7\ntype Count is integer\nvar next : Count is count\n");
    Lexer lexer(source);
    ParserDriver parser;
    const bool parsed = parser.parse(lexer);
    assert(parsed);
    auto* program = dynamic_cast<Program*>(parser.get_ast());
    assert(program && program->declarations.size() == 3);
    SemanticAnalyzer analyzer;
    const bool analyzed = analyzer.analyze(program);
    assert(analyzed);

    WasmStorage storage;
    std::vector<std::string> globals;
    std::vector<std::string> instructions;
    const auto& model = analyzer.get_model();
    storage.emit_globals(*program, model, globals, instructions,
        [&](const Expression& expression) {
            if (const auto* literal = dynamic_cast<const IntLiteral*>(&expression)) {
                return "i32.const " + std::to_string(literal->value);
            }
            const auto* identifier = dynamic_cast<const Identifier*>(&expression);
            assert(identifier);
            const auto* place = storage.place_of(*identifier, model);
            assert(place && place->kind == PlaceKind::Global);
            return WasmStorage::load_instruction(*place);
        });

    assert((globals == std::vector<std::string>{
        "(global $g0 (mut i32) (i32.const 0))", "(global $g1 (mut i32) (i32.const 0))"}));
    assert((instructions == std::vector<std::string>{
        "i32.const 7", "global.set $g0", "global.get $g0", "global.set $g1"}));
}

// A real global without an explicit initializer receives a zero of type f64.
static void test_global_variable_without_initializer() {
    VariableDeclaration declaration("distance", nullptr, nullptr, Location{});
    const VariableSymbol symbol("distance", PrimitiveType::get_real());
    SemanticModel model;
    model.bind_variable(&declaration, &symbol);

    WasmStorage storage;
    std::vector<std::string> globals;
    std::vector<std::string> instructions;
    storage.emit_global_variable(declaration, model, "$g0", globals, instructions, {});

    assert((globals == std::vector<std::string>{"(global $g0 (mut f64) (f64.const 0))"}));
    assert((instructions == std::vector<std::string>{"f64.const 0", "global.set $g0"}));
}

// Integer elements start after the header and occupy four bytes each.
static void test_integer_array_layout() {
    const ArrayType array(PrimitiveType::get_integer(), 3);
    const auto layout = WasmStorage::array_layout(array);
    assert(layout.elements_offset == 8 && layout.element_stride == 4);
    assert(WasmStorage::array_allocation_size(array) == 20);
}

// Real elements start at an eight-byte boundary and use an eight-byte stride.
static void test_real_array_layout() {
    const ArrayType array(PrimitiveType::get_real(), 3);
    const auto layout = WasmStorage::array_layout(array);
    assert(layout.elements_offset == 8 && layout.element_stride == 8);
    assert(WasmStorage::array_allocation_size(array) == 32);
}

// Nested array elements store four-byte references to separate objects.
static void test_nested_array_layout() {
    TypeFactory types;
    const ArrayType array(types.make_array(PrimitiveType::get_real(), 3), 2);
    const auto layout = WasmStorage::array_layout(array);
    assert(layout.elements_offset == 8 && layout.element_stride == 4);
    assert(WasmStorage::array_allocation_size(array) == 16);
}

// Array creation allocates the full block, stores the length, and leaves the address on the stack.
static void test_primitive_array_creation() {
    const ArrayType array(PrimitiveType::get_real(), 3);
    std::vector<std::string> instructions;
    WasmStorage::emit_primitive_array_creation(array, "$array_addr", instructions);

    assert((instructions == std::vector<std::string>{
        "i64.const 32", "call $alloc", "local.tee $array_addr",
        "i32.const 3", "i32.store", "local.get $array_addr"}));
}

// Array length is read from the first four bytes at the address on the stack.
static void test_array_size_loads_header() {
    std::vector<std::string> instructions;
    WasmStorage::emit_array_size(instructions);
    assert((instructions == std::vector<std::string>{"i32.load"}));
}

// The index check traps below one or above the length stored in the header.
static void test_array_index_check() {
    std::vector<std::string> instructions;
    WasmStorage::emit_array_index_check("$array_addr", "$index", instructions);
    assert((instructions == std::vector<std::string>{
        "local.get $index", "i32.const 1", "i32.lt_s",
        "local.get $index", "local.get $array_addr", "i32.load", "i32.gt_s",
        "i32.or", "if", "unreachable", "end"}));
}

// Element address generation checks the index before applying the array layout.
static void test_array_element_address() {
    const ArrayType array(PrimitiveType::get_real(), 3);
    std::vector<std::string> instructions;
    WasmStorage::emit_array_element_address(array, "$array_addr", "$index", instructions);

    assert((instructions == std::vector<std::string>{
        "local.get $index", "i32.const 1", "i32.lt_s",
        "local.get $index", "local.get $array_addr", "i32.load", "i32.gt_s",
        "i32.or", "if", "unreachable", "end",
        "local.get $array_addr", "i32.const 8", "i32.add",
        "local.get $index", "i32.const 1", "i32.sub", "i32.const 8", "i32.mul", "i32.add"}));
}

// Nested indexing evaluates each operand once and keeps the outer array's temporary slots separate.
static void test_nested_array_element_address_from_ast() {
    auto inner = std::make_unique<ElementAccessExpr>(
        std::make_unique<Identifier>("indices", Location{}), std::make_unique<IntLiteral>(1, Location{}), Location{});
    ElementAccessExpr outer(std::make_unique<Identifier>("values", Location{}), std::move(inner), Location{});
    const auto& index = static_cast<const ElementAccessExpr&>(*outer.index);

    TypeFactory types;
    SemanticModel model;
    model.set_type(outer.array.get(), types.make_array(PrimitiveType::get_real(), 3));
    model.set_type(index.array.get(), types.make_array(PrimitiveType::get_integer(), 2));

    WasmStorage storage;
    std::vector<std::string> locals;
    std::vector<std::string> instructions;
    std::vector<const Expression*> evaluation_order;
    storage.emit_array_element_address(outer, model, locals, instructions,
        [&](const Expression& expression) -> std::string {
            evaluation_order.push_back(&expression);
            if (&expression == &index) {
                std::vector<std::string> nested;
                storage.emit_array_element_address(index, model, locals, nested,
                    [&](const Expression& operand) -> std::string {
                        evaluation_order.push_back(&operand);
                        return &operand == index.array.get() ? "i32.const 128" : "i32.const 1";
                    });

                assert(nested.size() >= 4);
                assert(nested[1] == "local.set $tmp2");
                assert(nested[3] == "local.set $tmp3");
                std::string fragment;
                for (const auto& instruction : nested) {
                    fragment += instruction + '\n';
                }
                return fragment + "i32.load";
            }
            assert(&expression == outer.array.get());
            return "i32.const 64";
        });

    assert((evaluation_order == std::vector<const Expression*>{
        outer.array.get(), &index, index.array.get(), index.index.get()}));
    assert((locals == std::vector<std::string>{
        "(local $tmp0 i32)", "(local $tmp1 i32)", "(local $tmp2 i32)", "(local $tmp3 i32)"}));
    assert(instructions.size() >= 5);
    assert(instructions[1] == "local.set $tmp0");
    assert(instructions[3] == "local.set $tmp1");
    assert(instructions[4] == "local.get $tmp1");
}

// Real elements are read from linear memory as f64.
static void test_memory_load_instruction() {
    const auto real = PrimitiveType::get_real();
    assert(WasmStorage::memory_load_instruction(*real) == "f64.load");
}

// Aggregate elements store their references as i32 values.
static void test_memory_store_instruction() {
    TypeFactory types;
    const auto nested = types.make_array(PrimitiveType::get_real(), 3);
    assert(WasmStorage::memory_store_instruction(*nested) == "i32.store");
}

// An identifier produces a slot read without emitting a memory address.
static void test_read_from_local_place() {
    const VariableSymbol symbol("x", PrimitiveType::get_integer());
    Identifier identifier("x", Location{});
    SemanticModel model;
    model.bind_variable(&identifier, &symbol);

    WasmStorage storage;
    storage.bind_local(symbol, "$local0");
    std::vector<std::string> locals;
    std::vector<std::string> instructions;
    const auto place = storage.emit_place(identifier, model, locals, instructions, {});

    assert(place.kind == PlaceKind::Local);
    assert(locals.empty() && instructions.empty());
    assert(WasmStorage::load_instruction(place) == "local.get $local0");
}

// An array element prepares a checked address and selects the load for its element type.
static void test_read_from_array_element_place() {
    ElementAccessExpr access(std::make_unique<Identifier>("items", Location{}),
        std::make_unique<IntLiteral>(1, Location{}), Location{});
    const auto real = PrimitiveType::get_real();
    TypeFactory types;
    SemanticModel model;
    model.set_type(access.array.get(), types.make_array(real, 3));
    model.set_type(&access, real);

    WasmStorage storage;
    std::vector<std::string> locals;
    std::vector<std::string> instructions;
    const auto place = storage.emit_place(access, model, locals, instructions, [&](const Expression& operand) {
        return &operand == access.array.get() ? "i32.const 64" : "i32.const 1";
    });

    assert(place.kind == PlaceKind::Memory && place.value_type == real.get());
    assert((locals == std::vector<std::string>{"(local $tmp0 i32)", "(local $tmp1 i32)"}));
    assert(!instructions.empty() && instructions.back() == "i32.add");
    assert(WasmStorage::load_instruction(place) == "f64.load");
}

// Array assignment prepares a checked address before writing the converted value.
static void test_assignment_writes_array_element() {
    auto target = std::make_unique<ElementAccessExpr>(std::make_unique<Identifier>("items", Location{}),
        std::make_unique<IntLiteral>(1, Location{}), Location{});
    const auto& access = *target;
    AssignStatement assignment(std::move(target), std::make_unique<IntLiteral>(9, Location{}), Location{});
    SemanticModel model;
    TypeFactory types;
    const auto real = PrimitiveType::get_real();
    model.set_type(access.array.get(), types.make_array(real, 3));
    model.set_type(&access, real);

    WasmStorage storage;
    std::vector<std::string> locals;
    std::vector<std::string> instructions;
    std::vector<const Expression*> operands;
    storage.emit_assignment(assignment, model, locals, instructions, [&](const Expression& value) {
        operands.push_back(&value);
        if (&value == access.array.get()) {
            return "i32.const 64";
        }
        if (&value == access.index.get()) {
            return "i32.const 1";
        }
        assert(&value == assignment.value.get());
        assert(!instructions.empty() && instructions.back() == "i32.add");
        return "f64.const 9";
    });

    assert((operands == std::vector<const Expression*>{access.array.get(), access.index.get(), assignment.value.get()}));
    assert(locals.size() == 2);
    assert(instructions.size() >= 2 && instructions[instructions.size() - 2] == "f64.const 9");
    assert(instructions.back() == "f64.store");
}

int main() {
    test_parameter_resolves_to_local_slot();
    test_same_name_symbols_have_distinct_slots();
    test_parameter_read_resolves_to_declaration_slot();
    test_local_variable_with_initializer();
    test_local_variable_without_initializer();
    test_load_local_instruction();
    test_load_global_instruction();
    test_store_local_instruction();
    test_store_global_instruction();
    test_wat_primitive_value_types();
    test_wat_aggregate_value_types();
    test_wat_alias_value_type();
    test_wat_rejects_invalid_slot_types();
    test_local_declaration();
    test_default_primitive_values();
    test_assignment_updates_existing_slot();
    test_begin_function_clears_local_bindings();
    test_global_declaration();
    test_global_read_from_routine();
    test_global_initializers_preserve_source_order();
    test_global_variable_without_initializer();
    test_integer_array_layout();
    test_real_array_layout();
    test_nested_array_layout();
    test_primitive_array_creation();
    test_array_size_loads_header();
    test_array_index_check();
    test_array_element_address();
    test_nested_array_element_address_from_ast();
    test_memory_load_instruction();
    test_memory_store_instruction();
    test_read_from_local_place();
    test_read_from_array_element_place();
    test_assignment_writes_array_element();
}
