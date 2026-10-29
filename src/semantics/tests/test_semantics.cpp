#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

#include "Type.hpp"
#include "Symbol.hpp"
#include "Scope.hpp"
#include "DeclarationAnalyzer.hpp"
#include "ExpressionAnalyzer.hpp"
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
    assert(unsized_int->equals(*arr1));

    auto unsized_real = std::make_shared<ArrayType>(t_real, std::nullopt, 0);
    assert(!unsized_real->equals(*arr1));

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

    assert(point->equals(*point));
    assert(!point->equals(*vector2d));

    std::cout << "[PASS] test_records_and_nominal_equivalence\n";
}

void test_scope_hierarchy_and_shadowing() {
    auto global_scope = std::make_shared<Scope>();
    assert(global_scope->is_global());

    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();

    VariableSymbol global_x("x", t_int, false, {1, 1});
    assert(global_scope->declare_variable(global_x));
    assert(!global_scope->declare_variable(global_x));

    auto child_scope = global_scope->create_child();
    assert(!child_scope->is_global());
    assert(child_scope->get_parent() == global_scope);

    VariableSymbol local_x("x", t_real, false, {5, 5});
    assert(child_scope->declare_variable(local_x));

    auto* found_child = child_scope->lookup_variable("x");
    assert(found_child != nullptr && found_child->type == t_real);

    auto* found_global = global_scope->lookup_variable("x");
    assert(found_global != nullptr && found_global->type == t_int);

    std::cout << "[PASS] test_scope_hierarchy_and_shadowing\n";
}

void test_declaration_analyzer_types_and_vars() {
    auto global_scope = std::make_shared<Scope>();
    DeclarationAnalyzer analyzer(global_scope);

    auto type_node = std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1});
    auto type_decl = std::make_unique<TypeDeclaration>("count", std::move(type_node), Location{1, 1});
    analyzer.analyze(type_decl.get());
    assert(!analyzer.has_errors());
    assert(global_scope->lookup_type("count") != nullptr);

    auto dup_type_node = std::make_unique<PrimitiveTypeNode>("real", Location{2, 1});
    auto dup_type_decl = std::make_unique<TypeDeclaration>("count", std::move(dup_type_node), Location{2, 1});
    analyzer.analyze(dup_type_decl.get());
    assert(analyzer.has_errors());

    auto var_type_node = std::make_unique<IdentifierTypeNode>("count", Location{3, 1});
    auto var_expr = std::make_unique<IntLiteral>(10, Location{3, 15});
    auto var_decl = std::make_unique<VariableDeclaration>("c", std::move(var_type_node), std::move(var_expr), Location{3, 1});
    analyzer.analyze(var_decl.get());
    assert(global_scope->lookup_variable("c") != nullptr);

    std::cout << "[PASS] test_declaration_analyzer_types_and_vars\n";
}

void test_declaration_analyzer_routines_and_forward() {
    auto scope = std::make_shared<Scope>();
    DeclarationAnalyzer analyzer(scope);

    // Forward
    std::vector<std::unique_ptr<ParameterDeclaration>> params_fwd;
    params_fwd.push_back(std::make_unique<ParameterDeclaration>("a", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1}), Location{1, 1}));
    auto ret_type_fwd = std::make_unique<PrimitiveTypeNode>("integer", Location{1, 30});
    auto fwd_decl = std::make_unique<RoutineDeclaration>("sum", std::move(params_fwd), std::move(ret_type_fwd), Location{1, 1});
    analyzer.analyze(fwd_decl.get());
    assert(!analyzer.has_errors());

    // Implementation
    std::vector<std::unique_ptr<ParameterDeclaration>> params_def;
    params_def.push_back(std::make_unique<ParameterDeclaration>("a", std::make_unique<PrimitiveTypeNode>("integer", Location{5, 1}), Location{5, 1}));
    auto ret_type_def = std::make_unique<PrimitiveTypeNode>("integer", Location{5, 30});
    std::vector<std::unique_ptr<AstNode>> body;
    auto def_decl = std::make_unique<RoutineDeclaration>("sum", std::move(params_def), std::move(ret_type_def), std::move(body), Location{5, 1});
    analyzer.analyze(def_decl.get());
    assert(!analyzer.has_errors());

    auto* sym = scope->lookup_routine("sum");
    assert(sym != nullptr && sym->is_defined);

    std::cout << "[PASS] test_declaration_analyzer_routines_and_forward\n";
}

void test_assignment_matrix_l07() {
    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();
    auto t_bool = PrimitiveType::get_boolean();

    assert(ExpressionAnalyzer::can_assign(t_int, t_int));
    assert(ExpressionAnalyzer::can_assign(t_int, t_real));
    assert(ExpressionAnalyzer::can_assign(t_int, t_bool));

    assert(ExpressionAnalyzer::can_assign(t_real, t_real));
    assert(ExpressionAnalyzer::can_assign(t_real, t_int));
    assert(ExpressionAnalyzer::can_assign(t_real, t_bool));

    assert(ExpressionAnalyzer::can_assign(t_bool, t_bool));
    assert(ExpressionAnalyzer::can_assign(t_bool, t_int));
    assert(!ExpressionAnalyzer::can_assign(t_bool, t_real));

    std::vector<RecordField> fields = {{"x", t_int}};
    auto rec_a = std::make_shared<RecordType>("RecA", fields, 1);
    auto rec_b = std::make_shared<RecordType>("RecB", fields, 2);
    assert(ExpressionAnalyzer::can_assign(rec_a, rec_a));
    assert(!ExpressionAnalyzer::can_assign(rec_a, rec_b));

    auto fixed_arr = std::make_shared<ArrayType>(t_int, 5, 10);
    auto unsized_arr = std::make_shared<ArrayType>(t_int, std::nullopt, 0);
    assert(ExpressionAnalyzer::can_assign(unsized_arr, fixed_arr));

    std::cout << "[PASS] test_assignment_matrix_l07\n";
}

void test_expression_operators() {
    auto scope = std::make_shared<Scope>();
    std::vector<SemanticError> errors;
    ExpressionAnalyzer analyzer(scope, errors);
    Location loc{1, 1};

    auto e_int = std::make_unique<IntLiteral>(5, loc);
    auto u_minus = std::make_unique<UnaryExpr>(UnaryOp::Minus, std::move(e_int), loc);
    assert(analyzer.analyze(u_minus.get())->is_integer());

    auto e_bool = std::make_unique<BoolLiteral>(true, loc);
    auto u_not = std::make_unique<UnaryExpr>(UnaryOp::Not, std::move(e_bool), loc);
    assert(analyzer.analyze(u_not.get())->is_boolean());

    auto e_bad_not = std::make_unique<UnaryExpr>(UnaryOp::Not, std::make_unique<IntLiteral>(5, loc), loc);
    assert(analyzer.analyze(e_bad_not.get())->is_error());
    assert(!errors.empty());
    errors.clear();

    auto add_mixed = std::make_unique<BinaryExpr>(BinOp::Plus,
                                                  std::make_unique<IntLiteral>(1, loc),
                                                  std::make_unique<RealLiteral>(2.5, loc), loc);
    assert(analyzer.analyze(add_mixed.get())->is_real());

    auto mod_valid = std::make_unique<BinaryExpr>(BinOp::Mod,
                                                  std::make_unique<IntLiteral>(5, loc),
                                                  std::make_unique<IntLiteral>(2, loc), loc);
    assert(analyzer.analyze(mod_valid.get())->is_integer());

    auto mod_bad = std::make_unique<BinaryExpr>(BinOp::Mod,
                                                std::make_unique<RealLiteral>(5.5, loc),
                                                std::make_unique<IntLiteral>(2, loc), loc);
    assert(analyzer.analyze(mod_bad.get())->is_error());
    assert(!errors.empty());
    errors.clear();

    auto log_and = std::make_unique<BinaryExpr>(BinOp::And,
                                                std::make_unique<BoolLiteral>(true, loc),
                                                std::make_unique<BoolLiteral>(false, loc), loc);
    assert(analyzer.analyze(log_and.get())->is_boolean());

    auto comp_less = std::make_unique<BinaryExpr>(BinOp::Less,
                                                  std::make_unique<IntLiteral>(3, loc),
                                                  std::make_unique<RealLiteral>(4.5, loc), loc);
    assert(analyzer.analyze(comp_less.get())->is_boolean());

    auto arr_t = std::make_shared<ArrayType>(PrimitiveType::get_integer(), 5, 1);
    scope->declare_variable(VariableSymbol("arr", arr_t, false, loc));
    auto comp_bad = std::make_unique<BinaryExpr>(BinOp::Equal,
                                                 std::make_unique<Identifier>("arr", loc),
                                                 std::make_unique<IntLiteral>(5, loc), loc);
    assert(analyzer.analyze(comp_bad.get())->is_error());
    assert(!errors.empty());

    std::cout << "[PASS] test_expression_operators\n";
}

void test_member_and_element_access() {
    auto scope = std::make_shared<Scope>();
    std::vector<SemanticError> errors;
    ExpressionAnalyzer analyzer(scope, errors);
    Location loc{1, 1};

    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();

    std::vector<RecordField> pt_fields = {{"x", t_int}, {"y", t_int}};
    auto pt_type = std::make_shared<RecordType>("Point", pt_fields, 10);
    scope->declare_variable(VariableSymbol("pt", pt_type, false, loc));

    auto field_x = std::make_unique<FieldAccessExpr>(std::make_unique<Identifier>("pt", loc), "x", loc);
    assert(analyzer.analyze(field_x.get())->is_integer());

    auto field_bad = std::make_unique<FieldAccessExpr>(std::make_unique<Identifier>("pt", loc), "z", loc);
    assert(analyzer.analyze(field_bad.get())->is_error());
    assert(!errors.empty());
    errors.clear();

    auto arr_type = std::make_shared<ArrayType>(t_real, 10, 20);
    scope->declare_variable(VariableSymbol("arr", arr_type, false, loc));

    auto elem_valid = std::make_unique<ElementAccessExpr>(
            std::make_unique<Identifier>("arr", loc),
            std::make_unique<IntLiteral>(1, loc), loc);
    assert(analyzer.analyze(elem_valid.get())->is_real());

    auto elem_bad = std::make_unique<ElementAccessExpr>(
            std::make_unique<Identifier>("arr", loc),
            std::make_unique<BoolLiteral>(true, loc), loc);
    assert(analyzer.analyze(elem_bad.get())->is_error());
    assert(!errors.empty());
    errors.clear();

    auto size_access = std::make_unique<FieldAccessExpr>(std::make_unique<Identifier>("arr", loc), "size", loc);
    assert(analyzer.analyze(size_access.get())->is_integer());

    auto row_type = std::make_shared<ArrayType>(t_int, 3, 30);
    auto matrix_type = std::make_shared<ArrayType>(row_type, 3, 31);
    scope->declare_variable(VariableSymbol("matrix", matrix_type, false, loc));

    auto row_access = std::make_unique<ElementAccessExpr>(
            std::make_unique<Identifier>("matrix", loc),
            std::make_unique<IntLiteral>(1, loc), loc);
    auto cell_access = std::make_unique<ElementAccessExpr>(
            std::move(row_access),
            std::make_unique<IntLiteral>(2, loc), loc);
    assert(analyzer.analyze(cell_access.get())->is_integer());

    std::cout << "[PASS] test_member_and_element_access\n";
}

void test_routine_call_expressions() {
    auto scope = std::make_shared<Scope>();
    std::vector<SemanticError> errors;
    ExpressionAnalyzer analyzer(scope, errors);
    Location loc{1, 1};

    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();

    std::vector<VariableSymbol> params = {
            VariableSymbol("a", t_int, false, loc),
            VariableSymbol("b", t_int, false, loc)
    };
    scope->declare_routine(RoutineSymbol("add", params, t_int, false, true, loc));

    std::vector<std::unique_ptr<Expression>> valid_args;
    valid_args.push_back(std::make_unique<IntLiteral>(1, loc));
    valid_args.push_back(std::make_unique<IntLiteral>(2, loc));
    auto call_valid = std::make_unique<RoutineCallExpr>("add", std::move(valid_args), loc);
    assert(analyzer.analyze(call_valid.get())->is_integer());

    std::vector<std::unique_ptr<Expression>> few_args;
    few_args.push_back(std::make_unique<IntLiteral>(1, loc));
    auto call_few = std::make_unique<RoutineCallExpr>("add", std::move(few_args), loc);
    assert(analyzer.analyze(call_few.get())->is_integer());
    assert(!errors.empty());
    errors.clear();

    std::vector<VariableSymbol> proc_params = { VariableSymbol("val", t_int, false, loc) };
    scope->declare_routine(RoutineSymbol("log_msg", proc_params, PrimitiveType::get_void(), false, true, loc));

    std::vector<std::unique_ptr<Expression>> proc_args;
    proc_args.push_back(std::make_unique<IntLiteral>(10, loc));
    auto call_proc = std::make_unique<RoutineCallExpr>("log_msg", std::move(proc_args), loc);
    assert(analyzer.analyze(call_proc.get())->is_error());
    assert(!errors.empty());

    std::cout << "[PASS] test_routine_call_expressions\n";
}

void test_declaration_expression_inference() {
    auto scope = std::make_shared<Scope>();
    DeclarationAnalyzer decl_analyzer(scope);
    Location loc{1, 1};

    auto mul = std::make_unique<BinaryExpr>(BinOp::Mul,
                                            std::make_unique<IntLiteral>(3, loc),
                                            std::make_unique<IntLiteral>(4, loc), loc);
    auto add = std::make_unique<BinaryExpr>(BinOp::Plus,
                                            std::make_unique<IntLiteral>(2, loc),
                                            std::move(mul), loc);
    auto var_x = std::make_unique<VariableDeclaration>("x", nullptr, std::move(add), loc);

    decl_analyzer.analyze(var_x.get());
    assert(!decl_analyzer.has_errors());
    auto* sym_x = scope->lookup_variable("x");
    assert(sym_x != nullptr);
    assert(sym_x->type->is_integer());

    std::cout << "[PASS] test_declaration_expression_inference\n";
}

int main() {
    std::cout << "=== Running All Semantics Tests ===\n";

    test_primitive_types();
    test_aliases_and_unalias();
    test_arrays_and_nominal_equivalence();
    test_records_and_nominal_equivalence();
    test_scope_hierarchy_and_shadowing();
    test_declaration_analyzer_types_and_vars();
    test_declaration_analyzer_routines_and_forward();

    test_assignment_matrix_l07();
    test_expression_operators();
    test_member_and_element_access();
    test_routine_call_expressions();
    test_declaration_expression_inference();

    std::cout << "\nAll 12 semantic unit test suites passed successfully!\n";
    return 0;
}