#include <iostream>
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>
#include "SourceManager.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"
#include "ASTPrinter.hpp"
#include "optimizer/AstOptimizer.hpp"

// Helper to parse, semantically validate and optimize code
struct OptTest {
    ParserDriver driver;
    SemanticAnalyzer analyzer;
    AstOptimizer optimizer{analyzer.get_model()};
    Program* program = nullptr;

    explicit OptTest(const std::string& code) {
        SourceManager sm(code);
        Lexer lexer(sm);
        bool parsed = driver.parse(lexer);
        assert(parsed && "Code must parse successfully");

        program = dynamic_cast<Program*>(driver.get_ast());
        assert(program && "AST must be Program");

        bool sem_ok = analyzer.analyze(program);
        assert(sem_ok && "Code must pass semantic analysis before optimization");
    }

    std::string print_ast() {
        std::ostringstream ss;
        AstPrinter printer(ss);
        program->accept(printer);
        return ss.str();
    }
};

void test_constant_folding_and_algebraic() {
    OptTest test(R"(routine main() is
    var a is 5 + 3
    var b is 3 < 5
    var c is 10 * 4
    var d is true and false
    var e is not false
    var x is 7
    var f is x + 0
    var g is 1 * x
    var h is x * 0
    var i is true and true
    print a, b, c, d, e, f, g, h, i
end)");

    bool changed = test.optimizer.optimize(test.program);
    assert(changed && "Optimizer should perform simplifications");

    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(routine != nullptr);

    // a is 8
    auto* var_a = dynamic_cast<VariableDeclaration*>(routine->body[0].get());
    assert(var_a && dynamic_cast<IntLiteral*>(var_a->expression.get()));
    assert(dynamic_cast<IntLiteral*>(var_a->expression.get())->value == 8);

    // b is true
    auto* var_b = dynamic_cast<VariableDeclaration*>(routine->body[1].get());
    assert(var_b && dynamic_cast<BoolLiteral*>(var_b->expression.get()));
    assert(dynamic_cast<BoolLiteral*>(var_b->expression.get())->value == true);

    // c is 40
    auto* var_c = dynamic_cast<VariableDeclaration*>(routine->body[2].get());
    assert(var_c && dynamic_cast<IntLiteral*>(var_c->expression.get())->value == 40);

    // d is false
    auto* var_d = dynamic_cast<VariableDeclaration*>(routine->body[3].get());
    assert(var_d && dynamic_cast<BoolLiteral*>(var_d->expression.get())->value == false);

    // e is true
    auto* var_e = dynamic_cast<VariableDeclaration*>(routine->body[4].get());
    assert(var_e && dynamic_cast<BoolLiteral*>(var_e->expression.get())->value == true);

    // f is x (x + 0 folded to x)
    auto* var_f = dynamic_cast<VariableDeclaration*>(routine->body[6].get());
    assert(var_f && dynamic_cast<Identifier*>(var_f->expression.get()));
    assert(dynamic_cast<Identifier*>(var_f->expression.get())->name == "x");

    // g is x (1 * x folded to x)
    auto* var_g = dynamic_cast<VariableDeclaration*>(routine->body[7].get());
    assert(var_g && dynamic_cast<Identifier*>(var_g->expression.get()));
    assert(dynamic_cast<Identifier*>(var_g->expression.get())->name == "x");

    // h is 0 (x * 0 folded to 0)
    auto* var_h = dynamic_cast<VariableDeclaration*>(routine->body[8].get());
    assert(var_h && dynamic_cast<IntLiteral*>(var_h->expression.get())->value == 0);

    std::cout << "[PASS] test_constant_folding_and_algebraic\n";
}

void test_dead_code_elimination() {
    OptTest test(R"(routine foo() : integer is
    return 42
    print 1
    print 2
    var dead is 99
end)");

    test.optimizer.eliminate_dead_code(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(routine != nullptr);

    // The routine body should have ONLY 1 statement: the return statement!
    assert(routine->body.size() == 1);
    assert(dynamic_cast<ReturnStatement*>(routine->body[0].get()) != nullptr);

    std::cout << "[PASS] test_dead_code_elimination\n";
}

void test_dead_code_after_both_if_branches_return() {
    OptTest test(R"(routine sign(x : integer) : integer is
    if x > 0 then
        return 1
    else
        return -1
    end
    print 999
    var unreachable is 10
end)");

    test.optimizer.eliminate_dead_code(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(routine != nullptr);

    // Since both branches return, the statements after 'if' must be eliminated
    assert(routine->body.size() == 1);
    assert(dynamic_cast<IfStatement*>(routine->body[0].get()) != nullptr);

    std::cout << "[PASS] test_dead_code_after_both_if_branches_return\n";
}

void test_branch_pruning() {
    OptTest test(R"(routine test_branches() is
    if true then
        print 1
        print 2
    else
        print 3
    end
    if false then
        print 4
    else
        print 5
    end
    if false then
        print 6
    end
    while false loop
        print 7
    end
end)");

    test.optimizer.prune_branches(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(routine != nullptr);

    // Should now contain: print 1, print 2, print 5 (total 3 statements!)
    assert(routine->body.size() == 3);
    for (const auto& s : routine->body) {
        assert(dynamic_cast<PrintStatement*>(s.get()) != nullptr);
    }

    std::cout << "[PASS] test_branch_pruning\n";
}

void test_unused_variable_elimination() {
    OptTest test(R"(routine test_vars() is
    var unused_a is 100
    var unused_b : integer
    var used_c is 50
    print used_c
end)");

    test.optimizer.remove_unused_variables(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(routine != nullptr);

    // Only 'used_c' and 'print used_c' should remain
    assert(routine->body.size() == 2);
    auto* var = dynamic_cast<VariableDeclaration*>(routine->body[0].get());
    assert(var && var->name == "used_c");
    assert(dynamic_cast<PrintStatement*>(routine->body[1].get()) != nullptr);

    std::cout << "[PASS] test_unused_variable_elimination\n";
}

void test_iterative_fixed_point_pipeline() {
    OptTest test(R"(routine compute() : integer is
    var dead_var is 123
    if 2 + 3 > 4 then
        return 1
        print 999
    else
        return 0
    end
end)");

    // Run full optimizer pipeline
    test.optimizer.optimize(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(routine != nullptr);

    // 1. 2 + 3 > 4 folds to true
    // 2. if true replaces if statement with 'return 1; print 999;'
    // 3. print 999 after return 1 is removed
    // 4. dead_var is unused, so it is removed
    // Result: routine body has ONLY 1 statement: return 1!
    assert(routine->body.size() == 1);
    auto* ret = dynamic_cast<ReturnStatement*>(routine->body[0].get());
    assert(ret != nullptr);
    auto* val = dynamic_cast<IntLiteral*>(ret->expression.get());
    assert(val && val->value == 1);

    std::cout << "[PASS] test_iterative_fixed_point_pipeline\n";
}

VariableDeclaration* body_var(RoutineDeclaration* routine, const std::string& name) {
    for (const auto& s : routine->body) {
        if (auto* v = dynamic_cast<VariableDeclaration*>(s.get()); v && v->name == name) return v;
    }
    return nullptr;
}

void test_folding_preserves_types() {
    OptTest test(R"(routine main() is
    var r is 2.5
    var n is 3
    var y is r * 0.0
    var z is n + 0.0
    var k is n * 1
    print y, z, k
end)");

    test.optimizer.fold_constants(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(dynamic_cast<BinaryExpr*>(body_var(routine, "y")->expression.get()));
    assert(dynamic_cast<BinaryExpr*>(body_var(routine, "z")->expression.get()));
    auto* k_init = dynamic_cast<Identifier*>(body_var(routine, "k")->expression.get());
    assert(k_init && k_init->name == "n");

    std::cout << "[PASS] test_folding_preserves_types\n";
}

void test_model_follows_replacements() {
    OptTest test(R"(routine main() is
    var x : integer is 2.0 + 0.5
    var n is 3
    var y : real is n * 1
    print x, y
end)");

    test.optimizer.optimize(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    const SemanticModel& model = test.analyzer.get_model();

    auto* x_init = dynamic_cast<RealLiteral*>(body_var(routine, "x")->expression.get());
    assert(x_init && x_init->value == 2.5);
    assert(model.type_of(x_init) && model.type_of(x_init)->is_real());
    assert(model.conversion_of(x_init) == ConversionKind::RealToInteger);

    auto* y_init = dynamic_cast<Identifier*>(body_var(routine, "y")->expression.get());
    assert(y_init && y_init->name == "n");
    assert(model.type_of(y_init) && model.type_of(y_init)->is_integer());
    assert(model.conversion_of(y_init) == ConversionKind::IntegerToReal);
    assert(model.variable_of(y_init) && model.variable_of(y_init)->name == "n");

    std::cout << "[PASS] test_model_follows_replacements\n";
}

void test_branch_pruning_keeps_scopes() {
    OptTest test(R"(routine main() is
    var x is 1
    if false then
        print 0
    else
        var x is 2
        print x
    end
    if 1 < 2 then
        print 3
    end
    print x
end)");

    test.optimizer.optimize(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(routine->body.size() == 4);
    auto* kept_if = dynamic_cast<IfStatement*>(routine->body[1].get());
    assert(kept_if && kept_if->else_body.empty());
    auto* cond = dynamic_cast<BoolLiteral*>(kept_if->condition.get());
    assert(cond && cond->value);
    assert(dynamic_cast<VariableDeclaration*>(kept_if->then_body[0].get()));
    assert(dynamic_cast<PrintStatement*>(routine->body[2].get()));

    SemanticAnalyzer reanalyzer;
    assert(reanalyzer.analyze(test.program) && "Optimized AST must stay semantically valid");
    assert(!test.optimizer.optimize(test.program) && "Second run must reach a fixed point");

    std::cout << "[PASS] test_branch_pruning_keeps_scopes\n";
}

void test_unused_variables_keep_type_dependencies() {
    OptTest test(R"(routine main() is
    var k is 5
    var m is 6
    type R is record var f is m end
    var r : record var f is k end
    var s : R
    print r.f, s.f
end)");

    test.optimizer.optimize(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(body_var(routine, "k") && body_var(routine, "m"));

    std::cout << "[PASS] test_unused_variables_keep_type_dependencies\n";
}

void test_unused_variables_keep_failing_initializers() {
    OptTest test(R"(routine main() is
    var a : array [3] integer
    var i is 5
    var n is 2
    var t is a[i]
    var constant_index is a[2]
    var q is 7 / (i - 5)
    var b : boolean is n
    var h is a[i] * 0
    var g is false and a[i] = 1
    print 1
end)");

    test.optimizer.optimize(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(body_var(routine, "t") && body_var(routine, "q") && body_var(routine, "b"));
    assert(dynamic_cast<BinaryExpr*>(body_var(routine, "h")->expression.get()));
    assert(body_var(routine, "constant_index") && !body_var(routine, "g"));

    std::cout << "[PASS] test_unused_variables_keep_failing_initializers\n";
}

void test_i32_wraps_between_operations() {
    OptTest test(R"(routine main() is
    print (2147483647 + 1) < 0, (2147483647 + 1) / 2
    print ((-2147483647 - 1) - 1) > 0, -(2147483647 + 1)
    print (65536 * 65536) + 1, (-2147483647 - 1) % -1
end)");

    test.optimizer.fold_constants(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    auto* first = dynamic_cast<PrintStatement*>(routine->body[0].get());
    auto* second = dynamic_cast<PrintStatement*>(routine->body[1].get());
    auto* third = dynamic_cast<PrintStatement*>(routine->body[2].get());
    assert(first && second && third);

    auto* comparison = dynamic_cast<BoolLiteral*>(first->to_print[0].get());
    auto* quotient = dynamic_cast<IntLiteral*>(first->to_print[1].get());
    auto* subtraction = dynamic_cast<BoolLiteral*>(second->to_print[0].get());
    auto* negation = dynamic_cast<IntLiteral*>(second->to_print[1].get());
    auto* product = dynamic_cast<IntLiteral*>(third->to_print[0].get());
    auto* remainder = dynamic_cast<IntLiteral*>(third->to_print[1].get());
    assert(comparison && comparison->value);
    assert(quotient && quotient->value == -1073741824);
    assert(subtraction && subtraction->value);
    assert(negation && negation->value == -2147483647 - 1);
    assert(product && product->value == 1);
    assert(remainder && remainder->value == 0);
}

void test_real_folding_preserves_signed_zero() {
    OptTest test(R"(routine main() is
    var r is -0.0
    var sum is r + 0.0
    var folded_sum is -0.0 + 0.0
    print 1.0 / sum, folded_sum
end)");

    test.optimizer.fold_constants(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    auto* initial = dynamic_cast<RealLiteral*>(body_var(routine, "r")->expression.get());
    auto* sum = dynamic_cast<BinaryExpr*>(body_var(routine, "sum")->expression.get());
    auto* folded = dynamic_cast<RealLiteral*>(body_var(routine, "folded_sum")->expression.get());
    assert(initial && initial->value == 0.0 && std::signbit(initial->value));
    assert(sum && sum->op == BinOp::Plus);
    assert(folded && folded->value == 0.0 && !std::signbit(folded->value));
}

void test_folding_preserves_potential_traps() {
    OptTest test(R"(type Fixed is array [2] integer
routine read(a: array [] integer, n: integer) is
    var fixed: Fixed is a
    var indexed is fixed[2] * 0
    var divided is (n / -1) * 0
    var constant is ((-2147483647 - 1) / -1) * 0
    var skipped is false and (fixed[2] = 0)
    print 1
end
routine main() is
    var a: array [1] integer
    read(a, -2147483647 - 1)
end)");

    test.optimizer.optimize(test.program);
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[1].get());
    for (const char* name : {"indexed", "divided", "constant"}) {
        auto* variable = body_var(routine, name);
        assert(variable && dynamic_cast<BinaryExpr*>(variable->expression.get()));
    }
    assert(!body_var(routine, "skipped"));
}

void test_unused_chain_and_shadowing() {
    std::string source = "routine main(flag: boolean) is\n"
                         "var v0 is 0\n"
                         "var x is 10\n"
                         "var y is 20\n";
    for (int i = 1; i <= 12; ++i) {
        source += "var v" + std::to_string(i) + " is v" + std::to_string(i - 1) + "\n";
    }
    source += R"(if flag then
        var x is 1
        var y is y + 1
        print x, y
    end
end)";
    OptTest test(source);
    assert(test.optimizer.remove_unused_variables(test.program));
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(routine && routine->body.size() == 2 && body_var(routine, "y"));
    auto* branch = dynamic_cast<IfStatement*>(routine->body.back().get());
    assert(branch && branch->then_body.size() == 3);
    assert(!test.optimizer.remove_unused_variables(test.program));
}

void test_unused_aggregates_keep_allocations() {
    OptTest test(R"(type Flag is boolean
type Entry is record var value: integer end
routine main() is
    var huge: array [2147483647] integer
    var anonymous: record var value: integer end
    var named: Entry
    var flag: Flag
    print 1
end)");

    assert(test.optimizer.optimize(test.program));
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[2].get());
    assert(routine && routine->body.size() == 4);
    assert(body_var(routine, "huge"));
    assert(body_var(routine, "anonymous"));
    assert(body_var(routine, "named"));
    assert(!body_var(routine, "flag"));
}

void test_constant_runtime_errors_survive_optimization() {
    OptTest test(R"(routine check(b: boolean) is end
routine main(n: integer) is
    var a: array [1] integer
    var b: boolean is n * 0 + 2
    var quotient is 1 / (n * 0)
    var remainder is 1 % (n * 0)
    var indexed is a[n * 0]
    check(n * 0 + 2)
    print 1.0 / 0.0, 0.0 / 0.0, false and (1 / 0 = 0)
end)");

    assert(test.optimizer.optimize(test.program));
    auto* routine = dynamic_cast<RoutineDeclaration*>(test.program->declarations[1].get());
    assert(routine);
    const auto& model = test.analyzer.get_model();
    auto* boolean = dynamic_cast<IntLiteral*>(body_var(routine, "b")->expression.get());
    assert(boolean && boolean->value == 2);
    assert(model.conversion_of(boolean) == ConversionKind::IntegerToBoolean);
    for (const char* name : {"quotient", "remainder"}) {
        auto* division = dynamic_cast<BinaryExpr*>(body_var(routine, name)->expression.get());
        assert(division);
        auto* divisor = dynamic_cast<IntLiteral*>(division->right.get());
        assert(divisor && divisor->value == 0);
    }
    auto* access = dynamic_cast<ElementAccessExpr*>(body_var(routine, "indexed")->expression.get());
    assert(access);
    auto* index = dynamic_cast<IntLiteral*>(access->index.get());
    assert(index && index->value == 0);

    auto* call = dynamic_cast<RoutineCallStmt*>(routine->body[5].get());
    assert(call && model.conversion_of(call->arguments.front().get()) == ConversionKind::IntegerToBoolean);
    auto* output = dynamic_cast<PrintStatement*>(routine->body.back().get());
    assert(output && output->to_print.size() == 3);
    auto* infinity = dynamic_cast<RealLiteral*>(output->to_print[0].get());
    auto* nan = dynamic_cast<RealLiteral*>(output->to_print[1].get());
    auto* skipped = dynamic_cast<BoolLiteral*>(output->to_print[2].get());
    assert(infinity && std::isinf(infinity->value) && infinity->value > 0);
    assert(nan && std::isnan(nan->value));
    assert(skipped && !skipped->value);

    SemanticAnalyzer reanalyzer;
    assert(reanalyzer.analyze(test.program));
    assert(!test.optimizer.optimize(test.program));
}

void test_record_field_initializers() {
    OptTest test(R"(routine side(): integer is
    print 42
    return 7
end
type Named is record
    var converted: real is 2 + 3
    var nested: array [2] record
        var value is 6 * 7
        var effect is side() * 0
        var skipped is false and (side() = 7)
    end
end
routine take(x: record var value is 8 + 1 end) is end
routine make(): record var value is 3 * 4 end => make()
routine main() is
    var unused: Named
    var local: record var value is 1 / 0 end
end
)");

    assert(test.optimizer.optimize(test.program));
    const auto& declarations = test.program->declarations;
    const auto& model = test.analyzer.get_model();
    const auto* named = dynamic_cast<const TypeDeclaration*>(declarations[1].get());
    const auto* record = dynamic_cast<const RecordTypeNode*>(named->type.get());
    const auto* converted = dynamic_cast<const IntLiteral*>(record->declarations[0]->expression.get());
    assert(converted && converted->value == 5);
    assert(model.type_of(converted)->is_integer());
    assert(model.conversion_of(converted) == ConversionKind::IntegerToReal);

    const auto* array = dynamic_cast<const ArrayTypeNode*>(record->declarations[1]->type.get());
    const auto* nested = dynamic_cast<const RecordTypeNode*>(array->element_type.get());
    const auto* value = dynamic_cast<const IntLiteral*>(nested->declarations[0]->expression.get());
    const auto* effect = dynamic_cast<const BinaryExpr*>(nested->declarations[1]->expression.get());
    const auto* skipped = dynamic_cast<const BoolLiteral*>(nested->declarations[2]->expression.get());
    assert(value && value->value == 42);
    assert(effect && effect->op == BinOp::Mul);
    const auto* call = dynamic_cast<const RoutineCallExpr*>(effect->left.get());
    assert(call && model.routine_of(call) && model.routine_of(call)->name == "side");
    assert(skipped && !skipped->value);

    const auto* take = dynamic_cast<const RoutineDeclaration*>(declarations[2].get());
    const auto* parameter = dynamic_cast<const RecordTypeNode*>(take->parameters[0]->type.get());
    const auto* parameter_value = dynamic_cast<const IntLiteral*>(parameter->declarations[0]->expression.get());
    assert(parameter_value && parameter_value->value == 9);

    const auto* make = dynamic_cast<const RoutineDeclaration*>(declarations[3].get());
    const auto* result = dynamic_cast<const RecordTypeNode*>(make->return_type.get());
    const auto* result_value = dynamic_cast<const IntLiteral*>(result->declarations[0]->expression.get());
    assert(result_value && result_value->value == 12);

    auto* main = dynamic_cast<RoutineDeclaration*>(declarations[4].get());
    assert(main && body_var(main, "unused"));
    const auto* local = dynamic_cast<const RecordTypeNode*>(body_var(main, "local")->type.get());
    const auto* trap = dynamic_cast<const BinaryExpr*>(local->declarations[0]->expression.get());
    assert(trap && trap->op == BinOp::Div);

    SemanticAnalyzer reanalyzer;
    assert(reanalyzer.analyze(test.program));
    assert(!test.optimizer.optimize(test.program));
}

void test_integer_conditions_and_scoped_returns() {
    OptTest test(R"(routine choose(): integer is
    if 0 then
        return 1
    else
        var x is 2
        return x
    end
    print 999
end
routine main() is
    while 0 loop
        print 999
    end
    if 0 then
        print 999
    else
        var x is choose()
        print x
    end
end)");

    assert(test.optimizer.optimize(test.program));
    auto* choose = dynamic_cast<RoutineDeclaration*>(test.program->declarations[0].get());
    assert(choose && choose->body.size() == 1);
    auto* main = dynamic_cast<RoutineDeclaration*>(test.program->declarations[1].get());
    assert(main && main->body.size() == 1);
    auto* branch = dynamic_cast<IfStatement*>(main->body.front().get());
    assert(branch && branch->else_body.empty() && branch->then_body.size() == 2);
    const auto* condition = dynamic_cast<BoolLiteral*>(branch->condition.get());
    assert(condition && condition->value);
    const auto& model = test.analyzer.get_model();
    assert(model.type_of(condition)->is_boolean());
    assert(model.conversion_of(condition) == ConversionKind::None);

    SemanticAnalyzer reanalyzer;
    assert(reanalyzer.analyze(test.program));
    assert(!test.optimizer.optimize(test.program));
}

int main() {
    std::cout << "=== Running AST Optimizer Unit Tests ===\n";
    test_constant_folding_and_algebraic();
    test_dead_code_elimination();
    test_dead_code_after_both_if_branches_return();
    test_branch_pruning();
    test_unused_variable_elimination();
    test_iterative_fixed_point_pipeline();
    test_folding_preserves_types();
    test_model_follows_replacements();
    test_branch_pruning_keeps_scopes();
    test_unused_variables_keep_type_dependencies();
    test_unused_variables_keep_failing_initializers();
    test_i32_wraps_between_operations();
    test_real_folding_preserves_signed_zero();
    test_folding_preserves_potential_traps();
    test_unused_chain_and_shadowing();
    test_unused_aggregates_keep_allocations();
    test_constant_runtime_errors_survive_optimization();
    test_record_field_initializers();
    test_integer_conditions_and_scoped_returns();
    std::cout << "All AST optimizer tests passed successfully!\n";
    return 0;
}
