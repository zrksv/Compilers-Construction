#include <iostream>
#include <cassert>
#include <sstream>
#include "../ParserDriver.hpp"
#include "../../ast/AST.hpp"
#include "../../ast/ASTPrinter.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

void test_operator_precedence() {
    std::string code = "routine main() is\n"
                       "    var x is 2 + 3 * 4;\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok && "Parsing failed for operator precedence");
    assert(driver.get_ast() != nullptr);

    auto* routine = dynamic_cast<RoutineDeclaration*>(driver.get_ast());
    assert(routine != nullptr);
    assert(routine->name == "main");
    assert(routine->body.size() == 1);

    auto* varDecl = dynamic_cast<VariableDeclaration*>(routine->body[0].get());
    assert(varDecl != nullptr);
    assert(varDecl->name == "x");

    auto* addExpr = dynamic_cast<BinaryExpr*>(varDecl->expression.get());
    assert(addExpr != nullptr);
    assert(addExpr->op == BinOp::Plus);

    auto* leftInt = dynamic_cast<IntLiteral*>(addExpr->left.get());
    assert(leftInt != nullptr);
    assert(leftInt->value == 2);

    auto* mulExpr = dynamic_cast<BinaryExpr*>(addExpr->right.get());
    assert(mulExpr != nullptr);
    assert(mulExpr->op == BinOp::Mul);

    auto* mulLeft = dynamic_cast<IntLiteral*>(mulExpr->left.get());
    assert(mulLeft != nullptr);
    assert(mulLeft->value == 3);

    auto* mulRight = dynamic_cast<IntLiteral*>(mulExpr->right.get());
    assert(mulRight != nullptr);
    assert(mulRight->value == 4);

    std::cout << "[PASS] test_operator_precedence (2 + (3 * 4))\n";
}

void test_if_else() {
    std::string code = "routine check() is\n"
                       "    if x > 0 then\n"
                       "        print 1;\n"
                       "    else\n"
                       "        print 0;\n"
                       "    end\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok);

    auto* routine = dynamic_cast<RoutineDeclaration*>(driver.get_ast());
    assert(routine != nullptr);
    assert(routine->body.size() == 1);

    auto* ifStmt = dynamic_cast<IfStatement*>(routine->body[0].get());
    assert(ifStmt != nullptr);
    assert(ifStmt->then_body.size() == 1);
    assert(ifStmt->else_body.size() == 1);

    std::cout << "[PASS] test_if_else\n";
}

void test_loops() {
    std::string code = "routine loops() is\n"
                       "    while x > 0 loop\n"
                       "        x := x - 1;\n"
                       "    end\n"
                       "    for i in 1 .. 5 reverse loop\n"
                       "        print i;\n"
                       "    end\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok);

    auto* routine = dynamic_cast<RoutineDeclaration*>(driver.get_ast());
    assert(routine != nullptr);
    assert(routine->body.size() == 2);

    auto* whileStmt = dynamic_cast<WhileStatement*>(routine->body[0].get());
    assert(whileStmt != nullptr);
    assert(whileStmt->body.size() == 1);

    auto* forStmt = dynamic_cast<ForStatement*>(routine->body[1].get());
    assert(forStmt != nullptr);
    assert(forStmt->iterator_name == "i");
    assert(forStmt->is_reverse == true);
    assert(forStmt->body.size() == 1);

    std::cout << "[PASS] test_loops\n";
}

void test_syntax_error() {
    std::string code = "routine broken() is\n"
                       "    if true then\n"
                       "        print 1;\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(!ok);
    assert(driver.has_errors());
    assert(!driver.get_errors().empty());

    std::cout << "[PASS] test_syntax_error (detected missing end)\n";
}

void test_sample_01() {
    ParserDriver driver;
    bool ok = driver.parse_file("../../../tests/samples/01_print.imp");
    if (!ok) {
        ok = driver.parse_file("tests/samples/01_print.imp");
    }
    assert(ok);

    auto* routine = dynamic_cast<RoutineDeclaration*>(driver.get_ast());
    assert(routine != nullptr);
    assert(routine->name == "main");
    assert(routine->body.size() == 1);

    auto* printStmt = dynamic_cast<PrintStatement*>(routine->body[0].get());
    assert(printStmt != nullptr);
    assert(printStmt->to_print.size() == 1);

    std::cout << "[PASS] test_sample_01 (01_print.imp)\n";
    std::cout << "--- AST for 01_print.imp ---\n";
    AstPrinter printer(std::cout);
    routine->accept(printer);
    std::cout << "\n";
}

void test_sample_08() {
    ParserDriver driver;
    bool ok = driver.parse_file("../../../tests/samples/08_for_loop.imp");
    if (!ok) {
        ok = driver.parse_file("tests/samples/08_for_loop.imp");
    }
    assert(ok && "Failed to parse 08_for_loop.imp");

    auto* routine = dynamic_cast<RoutineDeclaration*>(driver.get_ast());
    assert(routine != nullptr);
    assert(routine->name == "main");
    assert(routine->body.size() == 4);

    std::cout << "[PASS] test_sample_08 (08_for_loop.imp)\n";
    std::cout << "--- AST for 08_for_loop.imp ---\n";
    AstPrinter printer(std::cout);
    routine->accept(printer);
    std::cout << "\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "Running Parser Tests...\n";
    test_operator_precedence();
    test_if_else();
    test_loops();
    test_syntax_error();
    test_sample_01();
    test_sample_08();
    std::cout << "All parser tests passed successfully!\n";
    return 0;
}
