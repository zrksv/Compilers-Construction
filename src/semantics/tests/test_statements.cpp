#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>

#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"

std::vector<SemanticError> analyze_source(const std::string& code) {
    ParserDriver driver;
    bool parsed = driver.parse(code);
    assert(parsed && "Test program must be syntactically valid");

    SemanticAnalyzer analyzer;
    bool ok = analyzer.analyze(dynamic_cast<Program*>(driver.get_ast()));
    auto errors = analyzer.get_diagnostics().get_errors();
    assert(ok == errors.empty());
    return errors;
}

void print_errors(const std::vector<SemanticError>& errors) {
    for (const auto& err : errors) {
        std::cerr << "    " << err.location.row << ":" << err.location.column << " " << err.message << "\n";
    }
}

void expect_valid(const std::string& code) {
    auto errors = analyze_source(code);
    if (!errors.empty()) {
        std::cerr << "Unexpected semantic errors in:\n" << code << "\n";
        print_errors(errors);
    }
    assert(errors.empty());
}

void expect_error_at(const std::string& code, size_t row) {
    auto errors = analyze_source(code);
    bool found = false;
    for (const auto& err : errors) {
        if (err.location.row == row) {
            found = true;
        }
    }
    if (!found) {
        std::cerr << "Expected a semantic error on line " << row << " in:\n" << code << "\n";
        print_errors(errors);
    }
    assert(found);
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

void test_separate_namespaces() {
    expect_valid(R"(var x : integer
type x is real
var f : integer
routine f() is
    f := 1
end
routine main() is
    var v : x is 1.5
    f()
end)");

    expect_error_at(R"(var x : integer
var x : real)", 2);

    std::cout << "[PASS] test_separate_namespaces\n";
}

void test_diagnostics_order_and_format() {
    auto errors = analyze_source(R"(routine main() is
    print undefined_a
end
var bad is undefined_b)");

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

int main() {
    std::cout << "Running Semantic Analyzer Tests...\n";

    test_assignment_conversions();
    test_aggregate_assignment();
    test_assignment_targets();
    test_conditions();
    test_for_loops();
    test_returns();
    test_routine_call_statements();
    test_print();
    test_scopes();
    test_separate_namespaces();
    test_diagnostics_order_and_format();

    std::cout << "\nAll Semantic Analyzer tests passed successfully!\n";
    return 0;
}
