#include "WasmStorage.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"
#include "SourceManager.hpp"
#include "types/TypeFactory.hpp"
#include "AstOptimizer.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <stdexcept>

// Mixed fields retain source order, with aligned f64 values and four-byte references.
static void test_record_layout() {
    TypeFactory types;
    const auto nested = types.make_record("Nested", {});
    const auto array = types.make_array(PrimitiveType::get_integer(), 2);
    const auto real = std::make_shared<AliasType>("Real", PrimitiveType::get_real());
    const RecordType record("Mixed", {{"id", PrimitiveType::get_integer()}, {"weight", real},
        {"flag", PrimitiveType::get_boolean()}, {"items", array}, {"nested", nested}});
    const auto layout = WasmStorage::record_layout(record);
    assert((layout.field_offsets == std::vector<std::uint64_t>{0, 8, 16, 20, 24}));
    assert(layout.allocation_size == 32);
}

// An empty record still reserves space for its own object identity.
static void test_empty_record_layout() {
    const RecordType record("Empty", {});
    const auto layout = WasmStorage::record_layout(record);
    assert(layout.field_offsets.empty() && layout.allocation_size == 8);
}

// Resolving a field evaluates its object once and returns a typed memory location.
static void test_record_field_place() {
    TypeFactory types;
    const auto record = types.make_record("Pair", {{"id", PrimitiveType::get_integer()},
        {"weight", PrimitiveType::get_real()}});
    FieldAccessExpr access(std::make_unique<Identifier>("pair", Location{}), "weight", Location{});
    SemanticModel model;
    model.set_type(access.object.get(), std::make_shared<AliasType>("PairAlias", record));
    model.set_type(&access, PrimitiveType::get_real());
    WasmStorage storage;
    std::vector<std::string> locals, instructions;
    int calls = 0;
    const auto place = storage.emit_place(access, model, locals, instructions, [&](const Expression& expression) {
        assert(&expression == access.object.get());
        ++calls;
        return "local.get $pair";
    });
    assert(calls == 1 && locals.empty());
    assert((instructions == std::vector<std::string>{"local.get $pair", "i32.const 8", "i32.add"}));
    assert(place.kind == PlaceKind::Memory);
    assert(WasmStorage::load_instruction(place) == "f64.load");
    assert(WasmStorage::store_instruction(place) == "f64.store");
}

// Field assignment computes the object and RHS once and uses the field's store type.
static void test_record_field_assignment() {
    TypeFactory types;
    const auto record = types.make_record("Pair", {{"flag", PrimitiveType::get_boolean()}});
    auto access = std::make_unique<FieldAccessExpr>(std::make_unique<Identifier>("pair", Location{}), "flag", Location{});
    const auto* object = access->object.get();
    SemanticModel model;
    model.set_type(object, record);
    model.set_type(access.get(), PrimitiveType::get_boolean());
    AssignStatement assignment(std::move(access), std::make_unique<BoolLiteral>(true, Location{}), Location{});
    WasmStorage storage;
    std::vector<std::string> locals, instructions;
    std::vector<const Expression*> evaluated;
    storage.emit_assignment(assignment, model, locals, instructions, [&](const Expression& expression) {
        evaluated.push_back(&expression);
        return &expression == object ? "local.get $pair" : "i32.const 1";
    });
    assert((evaluated == std::vector<const Expression*>{object, assignment.value.get()}));
    assert(instructions.back() == "i32.store");
}

// Array size is read-only and cannot become a writable record-field place.
static void test_array_size_is_not_record_storage() {
    TypeFactory types;
    FieldAccessExpr access(std::make_unique<Identifier>("items", Location{}), "size", Location{});
    SemanticModel model;
    model.set_type(access.object.get(), types.make_array(PrimitiveType::get_integer(), 2));
    WasmStorage storage;
    std::vector<std::string> locals, instructions;
    bool rejected = false;
    try {
        storage.emit_place(access, model, locals, instructions, {});
    } catch (const std::logic_error&) {
        rejected = true;
    }
    assert(rejected && instructions.empty());
}

// A large array of records emits a loop rather than unrolling a million creations.
static void test_large_aggregate_array_emits_bounded_code() {
    TypeFactory types;
    const auto record = types.make_record("Item", {{"value", PrimitiveType::get_integer()}});
    const auto array = types.make_array(record, 1000000);
    VariableDeclaration declaration("items", nullptr, nullptr, Location{});
    VariableSymbol symbol("items", array);
    SemanticModel model;
    model.bind_variable(&declaration, &symbol);
    WasmStorage storage;
    std::vector<std::string> locals, instructions;
    storage.emit_local_variable(declaration, model, "$items", locals, instructions, {});
    assert(instructions.size() < 100);
    assert(std::find(instructions.begin(), instructions.end(), "loop") != instructions.end());
    assert(std::find(instructions.begin(), instructions.end(), "i32.const 1000000") != instructions.end());
}

static const char* record_source = R"(routine next() : integer => 1
type Leaf is record
    var serial : integer is next()
    var weight : real is 1 + 2
    var enabled : boolean
end
type Leaves is array [2] Leaf
type Empty is record end
var seed : Leaf
type Box is record
    var tag : integer
    var child : Leaf
    var items : Leaves
    var matrix : array [2] array [2] integer
    var shared : Leaf is seed
    var empty : Empty
end
var box : Box
var alias : Box is box
var boxes : array [2] Box
routine create() : Box is
    var fresh : Box
    var shared : Box is fresh
    shared.child.weight := 9.5
    shared.items[2].serial := 77
    shared.matrix[1][2] := 42
    return fresh
end
routine new_empty() : Empty is
    var value : Empty
    return value
end
routine read_weight() : real => box.child.weight
routine read_item(index : integer) : integer => box.items[index].serial
routine write_weight() is
    alias.child.weight := 6.25
end
routine replace_child() is
    alias.child := seed
end
routine rebind(value : Box) is
    value.child.weight := 11.5
    value := box
    value.tag := 123
end
)";

// Only the few expression forms needed by this storage fixture are stubbed here.
// Production expression generation remains in the expression backend component.
static std::string fixture_value(const Expression& expression, const SemanticModel& model, WasmStorage& storage,
    std::vector<std::string>& locals, std::vector<std::string>& instructions) {
    std::string result;
    if (const auto* literal = dynamic_cast<const IntLiteral*>(&expression)) {
        result = "i32.const " + std::to_string(literal->value);
    } else if (const auto* literal = dynamic_cast<const RealLiteral*>(&expression)) {
        result = "f64.const " + std::to_string(literal->value);
    } else if (const auto* literal = dynamic_cast<const BoolLiteral*>(&expression)) {
        result = literal->value ? "i32.const 1" : "i32.const 0";
    } else if (const auto* call = dynamic_cast<const RoutineCallExpr*>(&expression)) {
        assert(call->routine_name == "next" && call->arguments.empty());
        result = "call $next";
    } else if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        assert(binary->op == BinOp::Plus);
        result = fixture_value(*binary->left, model, storage, locals, instructions) + '\n';
        result += fixture_value(*binary->right, model, storage, locals, instructions) + "\ni32.add";
    } else {
        // Keep recursively emitted instructions together in the returned fragment.
        std::vector<std::string> nested;
        const auto place = storage.emit_place(expression, model, locals, nested, [&](const Expression& value) {
            return fixture_value(value, model, storage, locals, nested);
        });
        for (const auto& line : nested) { result += line + '\n'; }
        result += WasmStorage::load_instruction(place);
    }
    const auto conversion = model.conversion_of(&expression);
    if (conversion == ConversionKind::IntegerToReal) {
        result += "\nf64.convert_i32_s";
    } else {
        assert(conversion == ConversionKind::None);
    }
    return result;
}

// Generate a fixture through semantic bindings, record creation, and actual place emission.
static void write_record_fixture(const std::string& path, bool optimized) {
    SourceManager source(record_source);
    Lexer lexer(source);
    ParserDriver parser;
    assert(parser.parse(lexer));
    auto* program = dynamic_cast<Program*>(parser.get_ast());
    SemanticAnalyzer analyzer;
    assert(program && analyzer.analyze(program));
    if (optimized) {
        AstOptimizer optimizer(analyzer.get_model());
        assert(optimizer.optimize(program));
    }
    const auto& model = analyzer.get_model();
    WasmStorage storage;
    storage.begin_function();
    std::vector<std::string> globals, locals, instructions;
    storage.emit_globals(*program, model, globals, locals, instructions, [&](const Expression& expression) {
        return fixture_value(expression, model, storage, locals, instructions);
    });

    std::ofstream output(path);
    if (!output) { throw std::runtime_error("Cannot write record fixture"); }
    const auto emit_lines = [&](const std::vector<std::string>& lines) {
        for (const auto& line : lines) { output << line << '\n'; }
    };
    output << "(module\n" << WasmStorage::memory_runtime();
    output << R"((export "memory" (memory 0))
(global $ticks (mut i32) (i32.const 0))
(func $next (result i32)
    global.get $ticks i32.const 1 i32.add global.set $ticks global.get $ticks)
(func (export "ticks") (result i32) global.get $ticks)
)";
    emit_lines(globals);
    output << "(func $init\n";
    emit_lines(locals);
    emit_lines(instructions);
    output << ")\n(start $init)\n";
    for (const auto& declaration : program->declarations) {
        if (const auto* variable = dynamic_cast<const VariableDeclaration*>(declaration.get())) {
            const auto* symbol = model.variable_of(variable);
            assert(symbol);
            const auto* place = storage.find_place(*symbol);
            assert(place);
            output << "(func (export \"" << variable->name << "\") (result i32) "
                   << WasmStorage::load_instruction(*place) << ")\n";
        }
        const auto* routine = dynamic_cast<const RoutineDeclaration*>(declaration.get());
        if (!routine || routine->name == "next") { continue; }
        storage.begin_function();
        locals.clear();
        instructions.clear();
        output << "(func (export \"" << routine->name << "\")";
        for (std::size_t i = 0; i < routine->parameters.size(); ++i) {
            const auto* symbol = model.variable_of(routine->parameters[i].get());
            assert(symbol);
            const auto name = "$p" + std::to_string(i);
            storage.bind_local(*symbol, name);
            output << " (param " << name << ' ' << WasmStorage::wat_value_type(*symbol) << ')';
        }
        const auto* symbol = model.routine_of(routine);
        assert(symbol);
        if (symbol->is_function()) {
            output << " (result " << WasmStorage::wat_value_type(*symbol->return_type) << ')';
        }
        output << '\n';
        const auto emit = [&](const Expression& expression) {
            return fixture_value(expression, model, storage, locals, instructions);
        };
        if (routine->expression_body) {
            instructions.push_back(emit(*routine->expression_body));
        }
        for (const auto& statement : routine->body) {
            if (const auto* variable = dynamic_cast<const VariableDeclaration*>(statement.get())) {
                storage.emit_local_variable(*variable, model, "$local" + variable->name, locals, instructions, emit);
            } else if (const auto* assignment = dynamic_cast<const AssignStatement*>(statement.get())) {
                storage.emit_assignment(*assignment, model, locals, instructions, emit);
            } else if (const auto* result = dynamic_cast<const ReturnStatement*>(statement.get())) {
                assert(result->expression);
                instructions.push_back(emit(*result->expression));
                instructions.push_back("return");
            } else {
                throw std::logic_error("Unexpected statement in storage fixture");
            }
        }
        emit_lines(locals);
        emit_lines(instructions);
        output << ")\n";
    }
    output << ")\n";
    output.close();
    if (!output) { throw std::runtime_error("Failed to write record fixture"); }
}

int main(int argc, char** argv) {
    if (argc == 3) {
        const std::string option = argv[1];
        assert(option == "--write-record-fixture" || option == "--write-optimized-record-fixture");
        write_record_fixture(argv[2], option == "--write-optimized-record-fixture");
        return 0;
    }
    test_record_layout();
    test_empty_record_layout();
    test_record_field_place();
    test_record_field_assignment();
    test_array_size_is_not_record_storage();
    test_large_aggregate_array_emits_bounded_code();
}
