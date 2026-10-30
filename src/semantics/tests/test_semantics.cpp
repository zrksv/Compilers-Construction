#include <iostream>
#include <cassert>
#include <memory>
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
#include "Dispatcher.hpp"
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

class TestDispatcher : public Dispatcher {
public:
    Services& services;

    PrimitiveTypeRule prim_rule;
    IdentifierTypeRule id_type_rule;
    ArrayTypeRule array_type_rule;
    RecordTypeRule record_type_rule;

    IntLiteralRule int_lit_rule;
    RealLiteralRule real_lit_rule;
    BoolLiteralRule bool_lit_rule;
    IdentifierRule id_rule;
    UnaryRule unary_rule;
    BinaryRule binary_rule;
    FieldAccessRule field_access_rule;
    ElementAccessRule elem_access_rule;
    RoutineCallExprRule call_expr_rule;

    ProgramRule program_rule;
    VariableDeclarationRule var_decl_rule;
    TypeDeclarationRule type_decl_rule;
    RoutineDeclarationRule routine_decl_rule;

    TestDispatcher(Services& s)
        : services(s),
          prim_rule(*this, s),
          id_type_rule(*this, s),
          array_type_rule(*this, s),
          record_type_rule(*this, s),
          int_lit_rule(*this, s),
          real_lit_rule(*this, s),
          bool_lit_rule(*this, s),
          id_rule(*this, s),
          unary_rule(*this, s),
          binary_rule(*this, s),
          field_access_rule(*this, s),
          elem_access_rule(*this, s),
          call_expr_rule(*this, s),
          program_rule(*this, s),
          var_decl_rule(*this, s),
          type_decl_rule(*this, s),
          routine_decl_rule(*this, s) {}

    ExprResult expression(Expression* node, const Context& ctx) override {
        if (!node) return ExprResult::error();
        if (auto* e = dynamic_cast<IntLiteral*>(node)) return int_lit_rule.analyze(*e, ctx);
        if (auto* e = dynamic_cast<RealLiteral*>(node)) return real_lit_rule.analyze(*e, ctx);
        if (auto* e = dynamic_cast<BoolLiteral*>(node)) return bool_lit_rule.analyze(*e, ctx);
        if (auto* e = dynamic_cast<Identifier*>(node)) return id_rule.analyze(*e, ctx);
        if (auto* e = dynamic_cast<UnaryExpr*>(node)) return unary_rule.analyze(*e, ctx);
        if (auto* e = dynamic_cast<BinaryExpr*>(node)) return binary_rule.analyze(*e, ctx);
        if (auto* e = dynamic_cast<FieldAccessExpr*>(node)) return field_access_rule.analyze(*e, ctx);
        if (auto* e = dynamic_cast<ElementAccessExpr*>(node)) return elem_access_rule.analyze(*e, ctx);
        if (auto* e = dynamic_cast<RoutineCallExpr*>(node)) return call_expr_rule.analyze(*e, ctx);
        return ExprResult::error();
    }

    TypeRef type(TypeNode* node, const Context& ctx) override {
        if (!node) return PrimitiveType::get_error();
        if (auto* t = dynamic_cast<PrimitiveTypeNode*>(node)) return prim_rule.resolve(*t, ctx);
        if (auto* t = dynamic_cast<IdentifierTypeNode*>(node)) return id_type_rule.resolve(*t, ctx);
        if (auto* t = dynamic_cast<ArrayTypeNode*>(node)) return array_type_rule.resolve(*t, ctx);
        if (auto* t = dynamic_cast<RecordTypeNode*>(node)) return record_type_rule.resolve(*t, ctx);
        return PrimitiveType::get_error();
    }

    StmtResult statement(AstNode* node, const Context& ctx) override {
        if (!node) return StmtResult{};
        if (auto* d = dynamic_cast<Program*>(node)) return program_rule.analyze(*d, ctx);
        if (auto* d = dynamic_cast<VariableDeclaration*>(node)) return var_decl_rule.analyze(*d, ctx);
        if (auto* d = dynamic_cast<TypeDeclaration*>(node)) return type_decl_rule.analyze(*d, ctx);
        if (auto* d = dynamic_cast<RoutineDeclaration*>(node)) return routine_decl_rule.analyze(*d, ctx);
        return StmtResult{};
    }

    StmtResult statement_list(const std::vector<std::unique_ptr<AstNode>>& nodes, const Context& ctx) override {
        StmtResult res;
        for (const auto& n : nodes) {
            auto r = statement(n.get(), ctx);
            if (r.returns) res.returns = true;
        }
        return res;
    }
};

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
    TestDispatcher disp(services);
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
        TestDispatcher disp(services);
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
        TestDispatcher disp(services);
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
        TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    TestDispatcher disp(services);
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
    std::cout << "All semantics unit tests passed successfully!\n";
    return 0;
}
