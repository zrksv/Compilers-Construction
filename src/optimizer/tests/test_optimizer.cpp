#include <iostream>
#include <cassert>
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
    var safe is a[2]
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
    assert(!body_var(routine, "safe") && !body_var(routine, "g"));

    std::cout << "[PASS] test_unused_variables_keep_failing_initializers\n";
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
    std::cout << "All AST optimizer tests passed successfully!\n";
    return 0;
}
