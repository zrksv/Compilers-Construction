#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

#include "Type.hpp"
#include "Symbol.hpp"
#include "Scope.hpp"
#include "DeclarationAnalyzer.hpp"
#include "AST.hpp"

void test_primitive_types() {
    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();
    auto t_bool = PrimitiveType::get_boolean();
    auto t_void = PrimitiveType::get_void();
    auto t_err = PrimitiveType::get_error();

    assert(t_int == PrimitiveType::get_integer());
    assert(t_real == PrimitiveType::get_real());
    assert(t_bool == PrimitiveType::get_boolean());
    assert(t_void == PrimitiveType::get_void());
    assert(t_err == PrimitiveType::get_error());

    assert(t_int->is_integer() && t_int->is_numeric());
    assert(t_real->is_real() && t_real->is_numeric());
    assert(t_bool->is_boolean() && !t_bool->is_numeric());
    assert(t_void->is_void());
    assert(t_err->is_error());

    assert(t_int->equals(*t_int));
    assert(!t_int->equals(*t_real));
    assert(!t_int->equals(*t_bool));

    std::cout << "[PASS] test_primitive_types\n";
}

void test_aliases_and_unalias() {
    auto t_int = PrimitiveType::get_integer();
    auto alias_a = std::make_shared<AliasType>("Count", t_int);
    auto alias_b = std::make_shared<AliasType>("SpecialCount", alias_a);
    auto alias_c = std::make_shared<AliasType>("SuperCount", alias_b);

    assert(alias_a->to_string() == "Count");
    assert(alias_a->is_integer());
    assert(alias_a->is_numeric());

    assert(alias_c->unalias() == t_int);
    assert(alias_b->unalias() == t_int);
    assert(alias_a->unalias() == t_int);

    assert(alias_c->equals(*t_int));
    assert(t_int->equals(*alias_c));
    assert(alias_a->equals(*alias_b));

    std::cout << "[PASS] test_aliases_and_unalias\n";
}

void test_arrays_and_nominal_equivalence() {
    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();

    auto arr1 = std::make_shared<ArrayType>(t_int, 10, 1);
    auto arr2 = std::make_shared<ArrayType>(t_int, 10, 2);

    assert(arr1->equals(*arr1));
    assert(!arr1->equals(*arr2));

    auto unsized_int = std::make_shared<ArrayType>(t_int, std::nullopt, 0);
    assert(unsized_int->is_unsized());
    assert(unsized_int->equals(*arr1)); // Unsized matches fixed array of same element type

    auto unsized_real = std::make_shared<ArrayType>(t_real, std::nullopt, 0);
    assert(!unsized_real->equals(*arr1)); // Element types must match

    std::cout << "[PASS] test_arrays_and_nominal_equivalence\n";
}

void test_records_and_nominal_equivalence() {
    auto t_real = PrimitiveType::get_real();

    std::vector<RecordField> fields = {
        {"x", t_real},
        {"y", t_real}
    };

    auto point = std::make_shared<RecordType>("Point", fields, 100);
    auto vector2d = std::make_shared<RecordType>("Vector2D", fields, 200);

    assert(point->find_field("x") != nullptr);
    assert(point->find_field("y") != nullptr);
    assert(point->find_field("z") == nullptr);

    // Nominal typing: same fields, but different IDs => not equal!
    assert(point->equals(*point));
    assert(!point->equals(*vector2d));

    std::cout << "[PASS] test_records_and_nominal_equivalence\n";
}

void test_scope_hierarchy_and_shadowing() {
    auto global_scope = std::make_shared<Scope>();
    assert(global_scope->is_global());

    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();

    // Global variable x : integer
    VariableSymbol global_x("x", t_int, false, {1, 1});
    assert(global_scope->declare_variable(global_x));
    assert(!global_scope->declare_variable(global_x)); // Duplicate in same scope fails

    // Enter child scope (e.g. function or block)
    auto child_scope = global_scope->create_child();
    assert(!child_scope->is_global());
    assert(child_scope->get_parent() == global_scope);

    // Shadowing: declare local x : real
    VariableSymbol local_x("x", t_real, false, {5, 5});
    assert(child_scope->declare_variable(local_x));

    // Lookup in child finds shadowed local x (real)
    auto* found_child = child_scope->lookup_variable("x");
    assert(found_child != nullptr && found_child->type == t_real);

    // Lookup in global still finds original global x (integer)
    auto* found_global = global_scope->lookup_variable("x");
    assert(found_global != nullptr && found_global->type == t_int);

    // Non-local lookup ascends to parent
    VariableSymbol global_y("y", t_int, false, {2, 1});
    global_scope->declare_variable(global_y);
    assert(child_scope->lookup_variable("y") != nullptr);

    // Local-only lookup does not ascend
    assert(child_scope->lookup_variable("y", /*check_parents=*/false) == nullptr);

    std::cout << "[PASS] test_scope_hierarchy_and_shadowing\n";
}

void test_declaration_analyzer_types_and_vars() {
    auto global_scope = std::make_shared<Scope>();
    DeclarationAnalyzer analyzer(global_scope);

    // Valid type declaration: type count is integer
    auto type_node = std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1});
    auto type_decl = std::make_unique<TypeDeclaration>("count", std::move(type_node), Location{1, 1});
    analyzer.analyze(type_decl.get());
    assert(!analyzer.has_errors());
    assert(global_scope->lookup_type("count") != nullptr);

    // Duplicate type declaration fails
    auto dup_type_node = std::make_unique<PrimitiveTypeNode>("real", Location{2, 1});
    auto dup_type_decl = std::make_unique<TypeDeclaration>("count", std::move(dup_type_node), Location{2, 1});
    analyzer.analyze(dup_type_decl.get());
    assert(analyzer.has_errors());

    // Valid variable declaration: var c : count is 10
    auto var_type_node = std::make_unique<IdentifierTypeNode>("count", Location{3, 1});
    auto var_expr = std::make_unique<IntLiteral>(10, Location{3, 15});
    auto var_decl = std::make_unique<VariableDeclaration>("c", std::move(var_type_node), std::move(var_expr), Location{3, 1});
    analyzer.analyze(var_decl.get());
    assert(global_scope->lookup_variable("c") != nullptr);

    // Invalid variable with unsized array fails
    auto unsized_arr_node = std::make_unique<ArrayTypeNode>(nullptr, std::make_unique<PrimitiveTypeNode>("integer", Location{4, 1}), Location{4, 1});
    auto bad_var = std::make_unique<VariableDeclaration>("arr", std::move(unsized_arr_node), nullptr, Location{4, 1});
    size_t err_count_before = analyzer.get_errors().size();
    analyzer.analyze(bad_var.get());
    assert(analyzer.get_errors().size() > err_count_before);

    std::cout << "[PASS] test_declaration_analyzer_types_and_vars\n";
}

void test_declaration_analyzer_routines_and_forward() {
    // Test matched forward and implementation
    {
        auto scope = std::make_shared<Scope>();
        DeclarationAnalyzer analyzer(scope);

        // Forward: routine sum(a : integer, b : integer) : integer;
        std::vector<std::unique_ptr<ParameterDeclaration>> params_fwd;
        params_fwd.push_back(std::make_unique<ParameterDeclaration>("a", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1}), Location{1, 1}));
        params_fwd.push_back(std::make_unique<ParameterDeclaration>("b", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 15}), Location{1, 15}));
        auto ret_type_fwd = std::make_unique<PrimitiveTypeNode>("integer", Location{1, 30});
        auto fwd_decl = std::make_unique<RoutineDeclaration>("sum", std::move(params_fwd), std::move(ret_type_fwd), Location{1, 1});

        analyzer.analyze(fwd_decl.get());
        assert(!analyzer.has_errors());

        auto* sym = scope->lookup_routine("sum");
        assert(sym != nullptr && sym->is_forward && !sym->is_defined);

        // Implementation: routine sum(a : integer, b : integer) : integer is ... end
        std::vector<std::unique_ptr<ParameterDeclaration>> params_def;
        params_def.push_back(std::make_unique<ParameterDeclaration>("a", std::make_unique<PrimitiveTypeNode>("integer", Location{5, 1}), Location{5, 1}));
        params_def.push_back(std::make_unique<ParameterDeclaration>("b", std::make_unique<PrimitiveTypeNode>("integer", Location{5, 15}), Location{5, 15}));
        auto ret_type_def = std::make_unique<PrimitiveTypeNode>("integer", Location{5, 30});
        std::vector<std::unique_ptr<AstNode>> body;
        auto def_decl = std::make_unique<RoutineDeclaration>("sum", std::move(params_def), std::move(ret_type_def), std::move(body), Location{5, 1});

        analyzer.analyze(def_decl.get());
        assert(!analyzer.has_errors());
        assert(sym->is_defined); // Correctly marked as defined!
    }

    // Test mismatched forward definition fails
    {
        auto scope = std::make_shared<Scope>();
        DeclarationAnalyzer analyzer(scope);

        // Forward: routine foo(x : integer) : integer;
        std::vector<std::unique_ptr<ParameterDeclaration>> p1;
        p1.push_back(std::make_unique<ParameterDeclaration>("x", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1}), Location{1, 1}));
        auto fwd = std::make_unique<RoutineDeclaration>("foo", std::move(p1), std::make_unique<PrimitiveTypeNode>("integer", Location{1, 20}), Location{1, 1});
        analyzer.analyze(fwd.get());

        // Mismatched implementation: routine foo(x : real) : integer is ... end
        std::vector<std::unique_ptr<ParameterDeclaration>> p2;
        p2.push_back(std::make_unique<ParameterDeclaration>("x", std::make_unique<PrimitiveTypeNode>("real", Location{3, 1}), Location{3, 1}));
        auto def = std::make_unique<RoutineDeclaration>("foo", std::move(p2), std::make_unique<PrimitiveTypeNode>("integer", Location{3, 20}), std::vector<std::unique_ptr<AstNode>>{}, Location{3, 1});
        analyzer.analyze(def.get());

        assert(analyzer.has_errors());
    }

    // Test unclosed forward at end of program fails
    {
        auto scope = std::make_shared<Scope>();
        DeclarationAnalyzer analyzer(scope);

        std::vector<std::unique_ptr<ParameterDeclaration>> p;
        p.push_back(std::make_unique<ParameterDeclaration>("x", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1}), Location{1, 1}));
        auto fwd = std::make_unique<RoutineDeclaration>("lingering", std::move(p), std::make_unique<PrimitiveTypeNode>("integer", Location{1, 20}), Location{1, 1});

        std::vector<std::unique_ptr<AstNode>> decls;
        decls.push_back(std::move(fwd));
        auto prog = std::make_unique<Program>(std::move(decls), Location{1, 1});

        analyzer.analyze(prog.get());
        assert(analyzer.has_errors()); // Error: forward-declared but never defined!
    }

    std::cout << "[PASS] test_declaration_analyzer_routines_and_forward\n";
}

int main() {
    std::cout << "=== Running Semantics Unit Tests ===\n";
    test_primitive_types();
    test_aliases_and_unalias();
    test_arrays_and_nominal_equivalence();
    test_records_and_nominal_equivalence();
    test_scope_hierarchy_and_shadowing();
    test_declaration_analyzer_types_and_vars();
    test_declaration_analyzer_routines_and_forward();
    std::cout << "All semantics unit tests passed successfully!\n";
    return 0;
}
