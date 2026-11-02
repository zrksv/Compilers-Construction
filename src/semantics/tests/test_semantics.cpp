#include <iostream>
#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "types/Type.hpp"
#include "types/TypeFactory.hpp"
#include "types/TypeCompatibility.hpp"
#include "symbols/Symbol.hpp"
#include "symbols/Scope.hpp"
#include "SemanticModel.hpp"
#include "Diagnostics.hpp"
#include "Results.hpp"
#include "Context.hpp"
#include "Services.hpp"
#include "AstDispatcher.hpp"
#include "RuleBase.hpp"

#include "rules/common/CallChecker.hpp"
#include "rules/types/PrimitiveTypeRule.hpp"
#include "rules/types/IdentifierTypeRule.hpp"
#include "rules/types/ArrayTypeRule.hpp"
#include "rules/types/RecordTypeRule.hpp"

#include "rules/expressions/LiteralRules.hpp"
#include "rules/expressions/IdentifierRule.hpp"
#include "rules/expressions/UnaryRule.hpp"
#include "rules/expressions/BinaryRule.hpp"
#include "rules/expressions/FieldAccessRule.hpp"
#include "rules/expressions/ElementAccessRule.hpp"
#include "rules/expressions/RoutineCallExprRule.hpp"

#include "rules/declarations/ProgramRule.hpp"
#include "rules/declarations/VariableDeclarationRule.hpp"
#include "rules/declarations/TypeDeclarationRule.hpp"
#include "rules/declarations/RoutineDeclarationRule.hpp"
#include "ast/AST.hpp"
#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"

// Verifies properties and equality comparisons of primitive types
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

// Verifies type alias resolution and transparent unaliasing
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

// Verifies nominal equivalence of fixed arrays and matching open array parameters
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

// Verifies nominal equivalence of record types and field lookup
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

// Verifies nested lexical scopes, variable shadowing, and non-local lookup
void test_scope_hierarchy_and_shadowing() {
    auto global_scope = std::make_shared<Scope>();
    assert(global_scope->is_global());

    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();

    VariableSymbol global_x("x", t_int, false, {1, 1});
    assert(global_scope->declare_variable(global_x));
    assert(!global_scope->declare_variable(global_x));

    auto& child_scope = global_scope->create_child();
    assert(!child_scope.is_global());
    assert(child_scope.get_parent() == global_scope.get());

    VariableSymbol local_x("x", t_real, false, {5, 5});
    assert(child_scope.declare_variable(local_x));

    auto* found_child = child_scope.lookup_variable("x");
    assert(found_child != nullptr && found_child->type == t_real);

    auto* found_global = global_scope->lookup_variable("x");
    assert(found_global != nullptr && found_global->type == t_int);

    VariableSymbol global_y("y", t_int, false, {2, 1});
    global_scope->declare_variable(global_y);
    assert(child_scope.lookup_variable("y") != nullptr);
    assert(child_scope.lookup_variable("y", /*check_parents=*/false) == nullptr);

    assert(!global_scope->declare_type(TypeSymbol("y", t_int, {3, 1})));
    assert(!global_scope->declare_routine(RoutineSymbol("y", {}, t_int, false, true, {4, 1})));
    assert(!global_scope->declare_variable(VariableSymbol("y", t_int, false, {5, 1})));

    std::cout << "[PASS] test_scope_hierarchy_and_shadowing\n";
}

// Verifies type declarations, duplicate type checks, and typed variable declarations
void test_declaration_analyzer_types_and_vars() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);

    // Valid type declaration
    auto type_node = std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1});
    auto type_decl = std::make_unique<TypeDeclaration>("count", std::move(type_node), Location{1, 1});
    disp.statement(type_decl.get(), ctx);
    assert(!diag.has_errors());
    assert(ctx.scope->lookup_type("count") != nullptr);

    // Duplicate type declaration in same scope fails
    auto dup_type_node = std::make_unique<PrimitiveTypeNode>("real", Location{2, 1});
    auto dup_type_decl = std::make_unique<TypeDeclaration>("count", std::move(dup_type_node), Location{2, 1});
    disp.statement(dup_type_decl.get(), ctx);
    assert(diag.has_errors());
    diag.clear();

    // Valid variable declaration
    auto var_type_node = std::make_unique<IdentifierTypeNode>("count", Location{3, 1});
    auto var_expr = std::make_unique<IntLiteral>(10, Location{3, 15});
    auto var_decl = std::make_unique<VariableDeclaration>("c", std::move(var_type_node), std::move(var_expr), Location{3, 1});
    disp.statement(var_decl.get(), ctx);
    assert(!diag.has_errors());
    assert(ctx.scope->lookup_variable("c") != nullptr);

    // Variable declaration with unsized array fails
    auto unsized_arr_node = std::make_unique<ArrayTypeNode>(nullptr, std::make_unique<PrimitiveTypeNode>("integer", Location{4, 1}), Location{4, 1});
    auto bad_var = std::make_unique<VariableDeclaration>("arr", std::move(unsized_arr_node), nullptr, Location{4, 1});
    disp.statement(bad_var.get(), ctx);
    assert(diag.has_errors());

    std::cout << "[PASS] test_declaration_analyzer_types_and_vars\n";
}

// Verifies routine declarations, forward matching, and unfulfilled forward detection
void test_declaration_analyzer_routines_and_forward() {
    // Matched forward and implementation
    {
        Diagnostics diag;
        TypeFactory factory;
        SemanticModel model;
        Services services{diag, factory, model};
        AstDispatcher disp(services);
        Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);

        std::vector<std::unique_ptr<ParameterDeclaration>> params_fwd;
        params_fwd.push_back(std::make_unique<ParameterDeclaration>("a", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1}), Location{1, 1}));
        params_fwd.push_back(std::make_unique<ParameterDeclaration>("b", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 15}), Location{1, 15}));
        auto ret_type_fwd = std::make_unique<PrimitiveTypeNode>("integer", Location{1, 30});
        auto fwd_decl = std::make_unique<RoutineDeclaration>("sum", std::move(params_fwd), std::move(ret_type_fwd), Location{1, 1});

        disp.statement(fwd_decl.get(), ctx);
        assert(!diag.has_errors());

        auto* sym = ctx.scope->lookup_routine("sum");
        assert(sym != nullptr && sym->is_forward && !sym->is_defined);

        std::vector<std::unique_ptr<ParameterDeclaration>> params_def;
        params_def.push_back(std::make_unique<ParameterDeclaration>("a", std::make_unique<PrimitiveTypeNode>("integer", Location{5, 1}), Location{5, 1}));
        params_def.push_back(std::make_unique<ParameterDeclaration>("b", std::make_unique<PrimitiveTypeNode>("integer", Location{5, 15}), Location{5, 15}));
        auto ret_type_def = std::make_unique<PrimitiveTypeNode>("integer", Location{5, 30});
        std::vector<std::unique_ptr<AstNode>> body;
        body.push_back(std::make_unique<ReturnStatement>(std::make_unique<IntLiteral>(0, Location{6, 5}), Location{6, 5}));
        auto def_decl = std::make_unique<RoutineDeclaration>("sum", std::move(params_def), std::move(ret_type_def), std::move(body), Location{5, 1});

        disp.statement(def_decl.get(), ctx);
        assert(!diag.has_errors());
        assert(sym->is_defined);
    }

    // Mismatched signature with forward declaration fails
    {
        Diagnostics diag;
        TypeFactory factory;
        SemanticModel model;
        Services services{diag, factory, model};
        AstDispatcher disp(services);
        Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);

        std::vector<std::unique_ptr<ParameterDeclaration>> p1;
        p1.push_back(std::make_unique<ParameterDeclaration>("x", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1}), Location{1, 1}));
        auto fwd = std::make_unique<RoutineDeclaration>("foo", std::move(p1), std::make_unique<PrimitiveTypeNode>("integer", Location{1, 20}), Location{1, 1});
        disp.statement(fwd.get(), ctx);

        std::vector<std::unique_ptr<ParameterDeclaration>> p2;
        p2.push_back(std::make_unique<ParameterDeclaration>("x", std::make_unique<PrimitiveTypeNode>("real", Location{3, 1}), Location{3, 1}));
        auto def = std::make_unique<RoutineDeclaration>("foo", std::move(p2), std::make_unique<PrimitiveTypeNode>("integer", Location{3, 20}), std::vector<std::unique_ptr<AstNode>>{}, Location{3, 1});
        disp.statement(def.get(), ctx);

        assert(diag.has_errors());
    }

    // Unclosed forward at end of program fails
    {
        Diagnostics diag;
        TypeFactory factory;
        SemanticModel model;
        Services services{diag, factory, model};
        AstDispatcher disp(services);
        Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);

        std::vector<std::unique_ptr<ParameterDeclaration>> p;
        p.push_back(std::make_unique<ParameterDeclaration>("x", std::make_unique<PrimitiveTypeNode>("integer", Location{1, 1}), Location{1, 1}));
        auto fwd = std::make_unique<RoutineDeclaration>("lingering", std::move(p), std::make_unique<PrimitiveTypeNode>("integer", Location{1, 20}), Location{1, 1});

        std::vector<std::unique_ptr<AstNode>> decls;
        decls.push_back(std::move(fwd));
        auto prog = std::make_unique<Program>(std::move(decls), Location{1, 1});

        disp.statement(prog.get(), ctx);
        assert(diag.has_errors());
    }

    std::cout << "[PASS] test_declaration_analyzer_routines_and_forward\n";
}

// Verifies type compatibility and assignment rules across primitive and aggregate types
void test_assignment_matrix() {
    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();
    auto t_bool = PrimitiveType::get_boolean();

    assert(TypeCompatibility::can_assign(t_int, t_int));
    assert(TypeCompatibility::conversion(t_int, t_int) == ConversionKind::None);

    assert(TypeCompatibility::can_assign(t_int, t_real));
    assert(TypeCompatibility::conversion(t_int, t_real) == ConversionKind::RealToInteger);

    assert(TypeCompatibility::can_assign(t_int, t_bool));
    assert(TypeCompatibility::conversion(t_int, t_bool) == ConversionKind::BooleanToInteger);

    assert(TypeCompatibility::can_assign(t_real, t_real));
    assert(TypeCompatibility::conversion(t_real, t_real) == ConversionKind::None);

    assert(TypeCompatibility::can_assign(t_real, t_int));
    assert(TypeCompatibility::conversion(t_real, t_int) == ConversionKind::IntegerToReal);

    assert(TypeCompatibility::can_assign(t_real, t_bool));
    assert(TypeCompatibility::conversion(t_real, t_bool) == ConversionKind::BooleanToReal);

    assert(TypeCompatibility::can_assign(t_bool, t_bool));
    assert(TypeCompatibility::conversion(t_bool, t_bool) == ConversionKind::None);

    assert(TypeCompatibility::can_assign(t_bool, t_int));
    assert(TypeCompatibility::conversion(t_bool, t_int) == ConversionKind::IntegerToBoolean);

    assert(!TypeCompatibility::can_assign(t_bool, t_real));

    std::vector<RecordField> fields = {{"x", t_int}};
    auto rec_a = std::make_shared<RecordType>("RecA", fields, 1);
    auto rec_b = std::make_shared<RecordType>("RecB", fields, 2);
    assert(TypeCompatibility::can_assign(rec_a, rec_a));
    assert(TypeCompatibility::conversion(rec_a, rec_a) == ConversionKind::None);
    assert(!TypeCompatibility::can_assign(rec_a, rec_b));

    auto fixed_arr = std::make_shared<ArrayType>(t_int, 5, 10);
    auto unsized_arr = std::make_shared<ArrayType>(t_int, std::nullopt, 0);
    assert(TypeCompatibility::can_assign(unsized_arr, fixed_arr));
    assert(TypeCompatibility::conversion(unsized_arr, fixed_arr) == ConversionKind::None);

    std::cout << "[PASS] test_assignment_matrix\n";
}

// Verifies operator typing, operand requirements, and prohibition of aggregate comparisons
void test_expression_operators() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    auto e_int = std::make_unique<IntLiteral>(5, loc);
    auto u_minus = std::make_unique<UnaryExpr>(UnaryOp::Minus, std::move(e_int), loc);
    assert(disp.expression(u_minus.get(), ctx).type->is_integer());

    auto e_bool = std::make_unique<BoolLiteral>(true, loc);
    auto u_not = std::make_unique<UnaryExpr>(UnaryOp::Not, std::move(e_bool), loc);
    assert(disp.expression(u_not.get(), ctx).type->is_boolean());

    auto e_bad_not = std::make_unique<UnaryExpr>(UnaryOp::Not, std::make_unique<IntLiteral>(5, loc), loc);
    assert(disp.expression(e_bad_not.get(), ctx).is_error());
    assert(diag.has_errors());
    diag.clear();

    auto add_mixed = std::make_unique<BinaryExpr>(BinOp::Plus,
                                                  std::make_unique<IntLiteral>(1, loc),
                                                  std::make_unique<RealLiteral>(2.5, loc), loc);
    assert(disp.expression(add_mixed.get(), ctx).type->is_real());

    auto mod_valid = std::make_unique<BinaryExpr>(BinOp::Mod,
                                                  std::make_unique<IntLiteral>(5, loc),
                                                  std::make_unique<IntLiteral>(2, loc), loc);
    assert(disp.expression(mod_valid.get(), ctx).type->is_integer());

    auto mod_bad = std::make_unique<BinaryExpr>(BinOp::Mod,
                                                std::make_unique<RealLiteral>(5.5, loc),
                                                std::make_unique<IntLiteral>(2, loc), loc);
    assert(disp.expression(mod_bad.get(), ctx).is_error());
    assert(diag.has_errors());
    diag.clear();

    auto log_and = std::make_unique<BinaryExpr>(BinOp::And,
                                                std::make_unique<BoolLiteral>(true, loc),
                                                std::make_unique<BoolLiteral>(false, loc), loc);
    assert(disp.expression(log_and.get(), ctx).type->is_boolean());

    auto comp_less = std::make_unique<BinaryExpr>(BinOp::Less,
                                                  std::make_unique<IntLiteral>(3, loc),
                                                  std::make_unique<RealLiteral>(4.5, loc), loc);
    assert(disp.expression(comp_less.get(), ctx).type->is_boolean());

    // Comparison of aggregate types (arrays/records) is forbidden
    auto arr_t = factory.make_array(PrimitiveType::get_integer(), 5);
    ctx.scope->declare_variable(VariableSymbol("arr", arr_t, false, loc));
    auto comp_bad = std::make_unique<BinaryExpr>(BinOp::Equal,
                                                 std::make_unique<Identifier>("arr", loc),
                                                 std::make_unique<IntLiteral>(5, loc), loc);
    assert(disp.expression(comp_bad.get(), ctx).is_error());
    assert(diag.has_errors());

    std::cout << "[PASS] test_expression_operators\n";
}

// Verifies record field access, array indexing, and read-only array size property
void test_member_and_element_access() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    auto t_int = PrimitiveType::get_integer();
    auto t_real = PrimitiveType::get_real();

    std::vector<RecordField> pt_fields = {{"x", t_int}, {"y", t_int}};
    auto pt_type = factory.make_record("Point", pt_fields);
    ctx.scope->declare_variable(VariableSymbol("pt", pt_type, false, loc));

    auto field_x = std::make_unique<FieldAccessExpr>(std::make_unique<Identifier>("pt", loc), "x", loc);
    assert(disp.expression(field_x.get(), ctx).type->is_integer());

    auto field_bad = std::make_unique<FieldAccessExpr>(std::make_unique<Identifier>("pt", loc), "z", loc);
    assert(disp.expression(field_bad.get(), ctx).is_error());
    assert(diag.has_errors());
    diag.clear();

    auto arr_type = factory.make_array(t_real, 10);
    ctx.scope->declare_variable(VariableSymbol("arr", arr_type, false, loc));

    auto elem_valid = std::make_unique<ElementAccessExpr>(
            std::make_unique<Identifier>("arr", loc),
            std::make_unique<IntLiteral>(1, loc), loc);
    assert(disp.expression(elem_valid.get(), ctx).type->is_real());

    auto elem_bad = std::make_unique<ElementAccessExpr>(
            std::make_unique<Identifier>("arr", loc),
            std::make_unique<BoolLiteral>(true, loc), loc);
    assert(disp.expression(elem_bad.get(), ctx).is_error());
    assert(diag.has_errors());
    diag.clear();

    auto size_access = std::make_unique<FieldAccessExpr>(std::make_unique<Identifier>("arr", loc), "size", loc);
    auto size_res = disp.expression(size_access.get(), ctx);
    assert(size_res.type->is_integer());
    assert(size_res.access == Access::ReadOnly);

    auto row_type = factory.make_array(t_int, 3);
    auto matrix_type = factory.make_array(row_type, 3);
    ctx.scope->declare_variable(VariableSymbol("matrix", matrix_type, false, loc));

    auto row_access = std::make_unique<ElementAccessExpr>(
            std::make_unique<Identifier>("matrix", loc),
            std::make_unique<IntLiteral>(1, loc), loc);
    auto cell_access = std::make_unique<ElementAccessExpr>(
            std::move(row_access),
            std::make_unique<IntLiteral>(2, loc), loc);
    assert(disp.expression(cell_access.get(), ctx).type->is_integer());

    std::cout << "[PASS] test_member_and_element_access\n";
}

// Verifies routine calls in expressions, argument checks, and forbidding procedures in expressions
void test_routine_call_expressions() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    auto t_int = PrimitiveType::get_integer();

    std::vector<VariableSymbol> params = {
            VariableSymbol("a", t_int, false, loc),
            VariableSymbol("b", t_int, false, loc)
    };
    ctx.scope->declare_routine(RoutineSymbol("add", params, t_int, false, true, loc));

    std::vector<std::unique_ptr<Expression>> valid_args;
    valid_args.push_back(std::make_unique<IntLiteral>(1, loc));
    valid_args.push_back(std::make_unique<IntLiteral>(2, loc));
    auto call_valid = std::make_unique<RoutineCallExpr>("add", std::move(valid_args), loc);
    assert(disp.expression(call_valid.get(), ctx).type->is_integer());

    std::vector<std::unique_ptr<Expression>> few_args;
    few_args.push_back(std::make_unique<IntLiteral>(1, loc));
    auto call_few = std::make_unique<RoutineCallExpr>("add", std::move(few_args), loc);
    assert(disp.expression(call_few.get(), ctx).type->is_integer());
    assert(diag.has_errors());
    diag.clear();

    std::vector<VariableSymbol> proc_params = { VariableSymbol("val", t_int, false, loc) };
    ctx.scope->declare_routine(RoutineSymbol("log_msg", proc_params, PrimitiveType::get_void(), false, true, loc));

    std::vector<std::unique_ptr<Expression>> proc_args;
    proc_args.push_back(std::make_unique<IntLiteral>(10, loc));
    auto call_proc = std::make_unique<RoutineCallExpr>("log_msg", std::move(proc_args), loc);
    assert(disp.expression(call_proc.get(), ctx).is_error());
    assert(diag.has_errors());

    std::cout << "[PASS] test_routine_call_expressions\n";
}

// Verifies variable type inference from an initializer expression when explicit type is omitted
void test_declaration_expression_inference() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    auto mul = std::make_unique<BinaryExpr>(BinOp::Mul,
                                            std::make_unique<IntLiteral>(3, loc),
                                            std::make_unique<IntLiteral>(4, loc), loc);
    auto add = std::make_unique<BinaryExpr>(BinOp::Plus,
                                            std::make_unique<IntLiteral>(2, loc),
                                            std::move(mul), loc);
    auto var_x = std::make_unique<VariableDeclaration>("x", nullptr, std::move(add), loc);

    disp.statement(var_x.get(), ctx);
    assert(!diag.has_errors());
    auto* sym_x = ctx.scope->lookup_variable("x");
    assert(sym_x != nullptr);
    assert(sym_x->type->is_integer());

    std::cout << "[PASS] test_declaration_expression_inference\n";
}

// Verifies that unsized arrays are rejected in variables and only permitted as routine parameters
void test_unsized_array_restrictions() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    // Unsized array in variable declaration fails
    auto elem_node = std::make_unique<PrimitiveTypeNode>("integer", loc);
    auto unsized_arr = std::make_unique<ArrayTypeNode>(nullptr, std::move(elem_node), loc);
    auto bad_var = std::make_unique<VariableDeclaration>("bad_var", std::move(unsized_arr), nullptr, loc);
    disp.statement(bad_var.get(), ctx);
    assert(diag.has_errors());
    diag.clear();

    // Unsized array directly as routine parameter succeeds
    std::vector<std::unique_ptr<ParameterDeclaration>> params;
    params.push_back(std::make_unique<ParameterDeclaration>(
        "arr_param",
        std::make_unique<ArrayTypeNode>(nullptr, std::make_unique<PrimitiveTypeNode>("integer", loc), loc),
        loc
    ));
    auto good_routine = std::make_unique<RoutineDeclaration>("open_arr_proc", std::move(params), nullptr, loc);
    disp.statement(good_routine.get(), ctx);
    assert(!diag.has_errors());

    std::cout << "[PASS] test_unsized_array_restrictions\n";
}

// Verifies that variable initializer expressions are evaluated in outer scope before declaring the new variable
void test_initializer_precedence() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    // Outer variable: var x : integer is 10
    auto outer_var = std::make_unique<VariableDeclaration>(
        "x",
        std::make_unique<PrimitiveTypeNode>("integer", loc),
        std::make_unique<IntLiteral>(10, loc),
        loc
    );
    disp.statement(outer_var.get(), ctx);
    assert(!diag.has_errors());

    // Inner variable: var x : integer is x + 1
    // The initializer 'x + 1' must resolve 'x' to outer scope
    Context child_ctx = ctx.with_child_scope();
    auto inner_var = std::make_unique<VariableDeclaration>(
        "x",
        std::make_unique<PrimitiveTypeNode>("integer", loc),
        std::make_unique<BinaryExpr>(BinOp::Plus,
                                    std::make_unique<Identifier>("x", loc),
                                    std::make_unique<IntLiteral>(1, loc),
                                    loc),
        loc
    );
    disp.statement(inner_var.get(), child_ctx);
    assert(!diag.has_errors());

    std::cout << "[PASS] test_initializer_precedence\n";
}

void test_equality_operator_types() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    auto int_eq_int = std::make_unique<BinaryExpr>(BinOp::Equal, std::make_unique<IntLiteral>(5, loc), std::make_unique<IntLiteral>(10, loc), loc);
    auto res1 = disp.expression(int_eq_int.get(), ctx);
    assert(!res1.is_error() && res1.type->is_boolean());

    auto int_eq_real = std::make_unique<BinaryExpr>(BinOp::Equal, std::make_unique<IntLiteral>(5, loc), std::make_unique<RealLiteral>(5.0, loc), loc);
    auto res2 = disp.expression(int_eq_real.get(), ctx);
    assert(!res2.is_error() && res2.type->is_boolean());
    assert(model.conversion_of(int_eq_real->left.get()) == ConversionKind::IntegerToReal);

    auto bool_eq_bool = std::make_unique<BinaryExpr>(BinOp::Equal, std::make_unique<BoolLiteral>(true, loc), std::make_unique<BoolLiteral>(false, loc), loc);
    auto res3 = disp.expression(bool_eq_bool.get(), ctx);
    assert(!res3.is_error() && res3.type->is_boolean());

    auto int_eq_bool = std::make_unique<BinaryExpr>(BinOp::Equal, std::make_unique<IntLiteral>(5, loc), std::make_unique<BoolLiteral>(true, loc), loc);
    auto res4 = disp.expression(int_eq_bool.get(), ctx);
    assert(!res4.is_error() && res4.type->is_boolean());
    assert(model.conversion_of(int_eq_bool->right.get()) == ConversionKind::BooleanToInteger);

    auto bool_eq_real = std::make_unique<BinaryExpr>(BinOp::Equal, std::make_unique<BoolLiteral>(true, loc), std::make_unique<RealLiteral>(1.0, loc), loc);
    auto res5 = disp.expression(bool_eq_real.get(), ctx);
    assert(!res5.is_error() && res5.type->is_boolean());
    assert(model.conversion_of(bool_eq_real->left.get()) == ConversionKind::BooleanToReal);

    assert(!diag.has_errors());

    std::cout << "[PASS] test_equality_operator_types\n";
}

void test_unsized_array_only_in_parameters() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    auto type_decl = std::make_unique<TypeDeclaration>(
        "Arr",
        std::make_unique<ArrayTypeNode>(nullptr, std::make_unique<PrimitiveTypeNode>("integer", loc), loc),
        loc
    );
    disp.statement(type_decl.get(), ctx);
    assert(diag.has_errors());

    std::cout << "[PASS] test_unsized_array_only_in_parameters\n";
}

void test_secondary_error_suppression() {
    Diagnostics diag;
    TypeFactory factory;
    SemanticModel model;
    Services services{diag, factory, model};
    AstDispatcher disp(services);
    Context ctx(&model.global_scope(), nullptr, TypeUse::Variable);
    Location loc{1, 1};

    auto bad_var = std::make_unique<VariableDeclaration>(
        "x",
        nullptr,
        std::make_unique<BinaryExpr>(BinOp::Plus, std::make_unique<Identifier>("undefined_var", loc), std::make_unique<IntLiteral>(1, loc), loc),
        loc
    );
    disp.statement(bad_var.get(), ctx);
    assert(diag.get_errors().size() == 1);
    assert(diag.get_errors()[0].message.find("not declared") != std::string::npos);

    std::cout << "[PASS] test_secondary_error_suppression\n";
}

// ============================================================================
// Tests on source text (parser + SemanticAnalyzer)
// ============================================================================

struct Run {
    ParserDriver driver;
    SemanticAnalyzer analyzer;
    bool ok = false;

    explicit Run(const std::string& code) {
        bool parsed = driver.parse(code);
        assert(parsed && "Test program must be syntactically valid");
        ok = analyzer.analyze(dynamic_cast<Program*>(driver.get_ast()));
    }

    const std::vector<SemanticError>& errors() {
        return analyzer.get_diagnostics().get_errors();
    }

    Program& program() {
        return *dynamic_cast<Program*>(driver.get_ast());
    }
};

void print_errors(const std::vector<SemanticError>& errors) {
    for (const auto& err : errors) {
        std::cerr << "    " << err.location.row << ":" << err.location.column << " " << err.message << "\n";
    }
}

void expect_valid(const std::string& code) {
    Run run(code);
    if (!run.errors().empty()) {
        std::cerr << "Unexpected semantic errors in:\n" << code << "\n";
        print_errors(run.errors());
    }
    assert(run.ok && run.errors().empty());
}

void expect_error_at(const std::string& code, size_t row) {
    Run run(code);
    bool found = false;
    for (const auto& err : run.errors()) {
        if (err.location.row == row) {
            found = true;
        }
    }
    if (!found) {
        std::cerr << "Expected a semantic error on line " << row << " in:\n" << code << "\n";
        print_errors(run.errors());
    }
    assert(!run.ok && found);
}

void test_unsized_parameter_accepts_any_array() {
    expect_valid(R"(type five is array [5] integer
routine total(a : array [] integer) : integer is
    return a.size
end
routine main() is
    var x : array [3] integer
    var y : five
    print total(x)
    print total(y)
end)");

    expect_error_at(R"(routine total(a : array [] integer) : integer is
    return a.size
end
routine main() is
    var r : array [3] real
    print total(r)
end)", 6);

    std::cout << "[PASS] test_unsized_parameter_accepts_any_array\n";
}

void test_explicit_type_initializer_from_source() {
    expect_error_at("var b : boolean is 2.5", 1);
    expect_valid("var b : boolean is 1");
    expect_valid("var r : real is 2");
    expect_error_at(R"(var a : array [2] integer
var i : integer is a)", 2);

    std::cout << "[PASS] test_explicit_type_initializer_from_source\n";
}

void test_unsized_array_positions_from_source() {
    expect_error_at("var v : array [] integer", 1);
    expect_error_at(R"(routine f() : array [] integer is
end)", 1);
    expect_error_at("var v : array [3] array [] integer", 1);
    expect_error_at(R"(type open is array [] integer
routine f(a : open) is
end)", 1);
    expect_error_at(R"(type R is record
    var items : array [] integer
end)", 2);

    std::cout << "[PASS] test_unsized_array_positions_from_source\n";
}

void test_aggregates_are_not_comparable() {
    expect_error_at(R"(type R is record var x : integer end
routine main() is
    var a : R
    var b : R
    print a = b
end)", 5);

    expect_error_at(R"(routine main() is
    var a : array [2] integer
    var b : array [2] integer
    print a /= b
end)", 4);

    std::cout << "[PASS] test_aggregates_are_not_comparable\n";
}

void test_record_field_initializers() {
    expect_valid(R"(type R is record
    var x is 5
    var y : real is 1
end
routine main() is
    var r : R
    r.x := 7
end)");

    expect_error_at(R"(type R is record
    var x is 5
end
routine main() is
    var r : R
    var a : array [2] integer
    r.x := a
end)", 7);

    expect_error_at(R"(type R is record
    var x : integer is undefined_name
end)", 2);

    Run run(R"(type R is record
    var x is undefined_name
end)");
    assert(run.errors().size() == 1);
    assert(run.errors()[0].message.find("not declared") != std::string::npos);

    std::cout << "[PASS] test_record_field_initializers\n";
}

void test_records_from_different_scopes_are_distinct() {
    expect_valid(R"(type R is record var x : integer end
routine main() is
    var a : R
    var b : R
    a := b
end)");

    expect_error_at(R"(type R is record var x : integer end
var outer : R
routine main() is
    type R is record var x : integer end
    var inner : R
    outer := inner
end)", 6);

    expect_error_at(R"(type A is record var x : integer end
type B is record var x : integer end
routine main() is
    var a : A
    var b : B
    a := b
end)", 6);

    std::cout << "[PASS] test_records_from_different_scopes_are_distinct\n";
}

void test_model_records_types_and_conversions() {
    Run run(R"(var r : real is 2
var a is 1
var b is a + 1
routine twice(n : integer) : integer => n * 2
var c is twice(a))");
    assert(run.ok);

    auto& decls = run.program().declarations;
    SemanticModel& model = run.analyzer.get_model();

    auto* r_decl = dynamic_cast<VariableDeclaration*>(decls[0].get());
    assert(r_decl && r_decl->expression);
    assert(model.type_of(r_decl->expression.get())->is_integer());
    assert(model.conversion_of(r_decl->expression.get()) == ConversionKind::IntegerToReal);

    auto* b_decl = dynamic_cast<VariableDeclaration*>(decls[2].get());
    auto* sum = dynamic_cast<BinaryExpr*>(b_decl->expression.get());
    assert(sum);
    assert(model.type_of(sum)->is_integer());
    auto* a_use = dynamic_cast<Identifier*>(sum->left.get());
    assert(a_use);
    assert(model.type_of(a_use)->is_integer());
    const VariableSymbol* bound = model.variable_of(a_use);
    assert(bound && bound->name == "a");

    auto* c_decl = dynamic_cast<VariableDeclaration*>(decls[4].get());
    auto* call = dynamic_cast<RoutineCallExpr*>(c_decl->expression.get());
    assert(call);
    const RoutineSymbol* callee = model.routine_of(call);
    assert(callee && callee->name == "twice");
    assert(model.type_of(call)->is_integer());

    std::cout << "[PASS] test_model_records_types_and_conversions\n";
}

void test_assignment_conversions() {
    expect_valid(R"(routine main() is
    var i : integer
    var r : real
    var b : boolean
    i := 2.5
    i := true
    r := 1
    r := false
    b := 1
    b := true
end)");

    expect_error_at(R"(routine main() is
    var b : boolean
    b := 2.5
end)", 3);

    expect_error_at(R"(routine main() is
    x := 1
end)", 2);

    expect_error_at(R"(routine main() is
    x := 1
    var x : integer
end)", 2);

    std::cout << "[PASS] test_assignment_conversions\n";
}

void test_aggregate_assignment() {
    expect_valid(R"(type A is record var x : integer end
type C is A
routine main() is
    var a : A
    var a2 : A
    var c : C
    a := a2
    a := c
end)");

    expect_error_at(R"(type A is record var x : integer end
type B is record var x : integer end
routine main() is
    var a : A
    var b : B
    a := b
end)", 6);

    std::cout << "[PASS] test_aggregate_assignment\n";
}

void test_assignment_targets() {
    expect_valid(R"(type P is record var x : integer end
routine main() is
    var ps : array [2] P
    ps[1].x := 5
    for p in ps loop
        p.x := 1
    end
end)");

    expect_error_at(R"(routine main() is
    var a : array [3] integer
    a.size := 5
end)", 3);

    expect_error_at(R"(routine main() is
    for i in 1 .. 3 loop
        i := 2
    end
end)", 3);

    std::cout << "[PASS] test_assignment_targets\n";
}

void test_conditions() {
    expect_valid(R"(routine main() is
    var n : integer
    if true then
    end
    if 1 then
    else
    end
    while false loop
    end
    while n loop
    end
end)");

    expect_error_at(R"(routine main() is
    if 2.5 then
    end
end)", 2);

    expect_error_at(R"(type R is record var x : integer end
routine main() is
    var r : R
    while r loop
    end
end)", 4);

    std::cout << "[PASS] test_conditions\n";
}

void test_for_loops() {
    expect_valid(R"(routine main() is
    for i in 1 .. 10 loop
        print i
    end
    for i in 1 .. 10 reverse loop
        var j : integer is i
        if true then
            var i is 5
        end
    end
end)");

    expect_error_at(R"(routine main() is
    var a : array [3] real
    for x in a loop
        var b : boolean
        b := x
    end
end)", 5);

    expect_error_at(R"(routine main() is
    for i in 1 .. 3 loop
    end
    print i
end)", 4);

    expect_error_at(R"(routine main() is
    for x in 5 loop
    end
end)", 2);

    expect_error_at(R"(type R is record var x : integer end
routine main() is
    var r : R
    for i in 1 .. r loop
    end
end)", 4);

    expect_error_at(R"(routine main() is
    for i in 1 .. 3 loop
        var i is 5
    end
end)", 3);

    std::cout << "[PASS] test_for_loops\n";
}

void test_returns() {
    expect_valid(R"(routine p(x : integer) is
    if x > 0 then
        return
    end
    print x
end
routine sign(x : integer) : integer is
    if x > 0 then
        return 1
    else
        if x < 0 then
            return -1
        else
            return 0
        end
    end
end
routine find(x : integer) : integer is
    for i in 1 .. 10 loop
        if i = x then
            return i
        end
    end
    return 0
end
routine twice(x : integer) : integer => x * 2)");

    expect_error_at(R"(routine p() is
    return 1
end)", 2);

    expect_error_at(R"(routine f() : integer is
    return
end)", 2);

    expect_error_at(R"(routine f() : boolean is
    return 2.5
end)", 2);

    expect_error_at(R"(routine f() : integer is
    var x is 1
end)", 1);

    expect_error_at(R"(routine f(x : integer) : integer is
    if x > 0 then
        return 1
    end
end)", 1);

    expect_error_at(R"(routine f() : integer is
    while true loop
        return 1
    end
end)", 1);

    expect_error_at("routine f() => 5", 1);
    expect_error_at("routine f() : boolean => 2.5", 1);

    std::cout << "[PASS] test_returns\n";
}

void test_routine_call_statements() {
    expect_valid(R"(routine g(x : integer)
routine h() : integer => 1
routine countdown(n : integer) is
    if n > 0 then
        countdown(n - 1)
    end
end
routine main() is
    g(1)
    h()
    countdown(3)
end
routine g(x : integer) is
    print x
end)");

    expect_error_at(R"(routine main() is
    g(1)
end
routine g(x : integer) is
    print x
end)", 2);

    expect_error_at(R"(routine g(x : integer, y : integer) is
end
routine main() is
    g(1)
end)", 4);

    expect_error_at(R"(routine g(flag : boolean) is
end
routine main() is
    g(2.5)
end)", 4);

    std::cout << "[PASS] test_routine_call_statements\n";
}

void test_print() {
    expect_valid(R"(routine main() is
    print 1, 2.5, true
end)");

    expect_error_at(R"(type R is record var x : integer end
routine main() is
    var r : R
    print r
end)", 4);

    expect_error_at(R"(routine main() is
    var a : array [2] integer
    print 1, a
end)", 3);

    std::cout << "[PASS] test_print\n";
}

void test_scopes() {
    expect_valid(R"(routine main() is
    var x : integer
    if true then
        var x : real is 1.5
        x := 2.5
    end
    x := 1
end)");

    expect_error_at(R"(routine main() is
    if true then
        var y : integer
    end
    y := 1
end)", 5);

    expect_error_at(R"(routine f(a : integer) is
    var a : integer
end)", 2);

    expect_error_at(R"(routine main() is
    while true loop
        var y is z
    end
end)", 3);

    std::cout << "[PASS] test_scopes\n";
}

void test_unified_namespace() {
    expect_error_at(R"(var x : integer
type x is real)", 2);

    expect_error_at(R"(var f : integer
routine f() is
end)", 2);

    expect_error_at(R"(routine f() is
end
type f is integer)", 3);

    expect_valid(R"(type t is integer
var v : t
routine main() is
    var t : integer
    t := 1
end)");

    expect_error_at(R"(var x : integer
var x : real)", 2);

    std::cout << "[PASS] test_unified_namespace\n";
}

void test_diagnostics_order_and_format() {
    Run run(R"(routine main() is
    print undefined_a
end
var bad is undefined_b)");
    const auto& errors = run.errors();

    assert(errors.size() >= 2);
    for (size_t i = 1; i < errors.size(); ++i) {
        assert(errors[i - 1].location.row <= errors[i].location.row);
    }
    assert(errors.front().location.row == 2);
    assert(errors.back().location.row == 4);

    Diagnostics diagnostics;
    diagnostics.report({3, 7}, "second");
    diagnostics.report({1, 2}, "first");
    diagnostics.sort();
    std::ostringstream out;
    diagnostics.print(out);
    assert(out.str() == "Error at 1:2 - first\nError at 3:7 - second\n");

    SemanticAnalyzer analyzer;
    assert(!analyzer.analyze(nullptr));
    assert(analyzer.get_diagnostics().has_errors());

    std::cout << "[PASS] test_diagnostics_order_and_format\n";
}

void test_array_bounds_checking() {
    expect_valid(R"(routine main() is
    var a : array [5] integer
    a[1] := 10
    a[5] := 50
    print a[1], a[5]
end)");

    expect_error_at(R"(routine main() is
    var a : array [5] integer
    a[0] := 10
end)", 3);

    expect_error_at(R"(routine main() is
    var a : array [5] integer
    print a[6]
end)", 3);

    expect_error_at(R"(routine main() is
    var a : array [5] integer
    a[-1] := 10
end)", 3);

    expect_error_at(R"(routine main() is
    var a : array [5] integer
    print a[2 + 4]
end)", 3);

    std::cout << "[PASS] test_array_bounds_checking\n";
}

void test_division_by_zero() {
    expect_valid(R"(routine main() is
    var a is 10 / 2
    var b is 10 % 3
    var c is 10.0 / 2.5
end)");

    expect_error_at(R"(routine main() is
    var a is 10 / 0
end)", 2);

    expect_error_at(R"(routine main() is
    var a is 10 % 0
end)", 2);

    expect_error_at(R"(routine main() is
    var a is 10.0 / 0.0
end)", 2);

    expect_error_at(R"(routine main() is
    var a is 10 / (5 - 5)
end)", 2);

    std::cout << "[PASS] test_division_by_zero\n";
}

void test_strict_int_to_bool() {
    expect_valid(R"(routine main() is
    var b1 : boolean is 0
    var b2 : boolean is 1
    b1 := 1
    b2 := 0
    if 1 then
    end
    while 0 loop
    end
end)");

    expect_error_at(R"(routine main() is
    var b : boolean is 2
end)", 2);

    expect_error_at(R"(routine main() is
    var b : boolean is -1
end)", 2);

    expect_error_at(R"(routine main() is
    var b : boolean
    b := 5
end)", 3);

    expect_error_at(R"(routine main() is
    if 2 then
    end
end)", 2);

    expect_error_at(R"(routine test(b : boolean) is
end
routine main() is
    test(2)
end)", 4);

    std::cout << "[PASS] test_strict_int_to_bool\n";
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
    test_assignment_matrix();
    test_expression_operators();
    test_member_and_element_access();
    test_routine_call_expressions();
    test_declaration_expression_inference();
    test_unsized_array_restrictions();
    test_initializer_precedence();
    test_equality_operator_types();
    test_unsized_array_only_in_parameters();
    test_secondary_error_suppression();
    test_unsized_parameter_accepts_any_array();
    test_explicit_type_initializer_from_source();
    test_unsized_array_positions_from_source();
    test_aggregates_are_not_comparable();
    test_record_field_initializers();
    test_records_from_different_scopes_are_distinct();
    test_model_records_types_and_conversions();
    test_assignment_conversions();
    test_aggregate_assignment();
    test_assignment_targets();
    test_conditions();
    test_for_loops();
    test_returns();
    test_routine_call_statements();
    test_print();
    test_scopes();
    test_unified_namespace();
    test_diagnostics_order_and_format();
    test_array_bounds_checking();
    test_division_by_zero();
    test_strict_int_to_bool();
    std::cout << "All semantics unit tests passed successfully!\n";
    return 0;
}

