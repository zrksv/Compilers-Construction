#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include "../ASTPrinter.hpp"
#include "../AST.hpp"

void write_ast_to_file(const std::string& output_filename, AstNode* root) {
    std::filesystem::path sourcePath(__FILE__);
    std::filesystem::path outDir = sourcePath.parent_path();
    std::filesystem::path outPath = outDir / output_filename;

    std::ofstream outFile(outPath);
    if (outFile.is_open()) {
        AstPrinter filePrinter(outFile);
        root->accept(filePrinter);
        std::cout << "Successfully written AST to: " << outPath.filename().string() << "\n";
    } else {
        std::cerr << "Failed to open file for writing: " << outPath.string() << "\n";
    }
}

// --- UNIT TEST 1: Math Expression ---
// Checks: 2 + (x * 3)
void test_math_expression() {
    Location loc{1, 1};
    auto lit2 = std::make_unique<IntLiteral>(2, loc);
    auto varX = std::make_unique<Identifier>("x", loc);
    auto lit3 = std::make_unique<IntLiteral>(3, loc);
    
    auto mul = std::make_unique<BinaryExpr>(BinOp::Mul, std::move(varX), std::move(lit3), loc);
    auto add = std::make_unique<BinaryExpr>(BinOp::Plus, std::move(lit2), std::move(mul), loc);
    
    write_ast_to_file("ast_test_math.txt", add.get());
}

// --- UNIT TEST 2: If-Else Statement ---
// Checks: if (x < 10) then x := x + 1 else x := 0 end
void test_if_statement() {
    Location loc{2, 1};
    
    // Condition: x < 10
    auto condX = std::make_unique<Identifier>("x", loc);
    auto cond10 = std::make_unique<IntLiteral>(10, loc);
    auto cond = std::make_unique<BinaryExpr>(BinOp::Less, std::move(condX), std::move(cond10), loc);

    // Then block: x := x + 1
    auto thenX = std::make_unique<Identifier>("x", loc);
    auto then1 = std::make_unique<IntLiteral>(1, loc);
    auto thenPlus = std::make_unique<BinaryExpr>(BinOp::Plus, std::move(thenX), std::move(then1), loc);
    auto assignThen = std::make_unique<AssignStatement>("x", std::move(thenPlus), loc);
    std::vector<std::unique_ptr<AstNode>> then_body;
    then_body.push_back(std::move(assignThen));

    // Else block: x := 0
    auto else0 = std::make_unique<IntLiteral>(0, loc);
    auto assignElse = std::make_unique<AssignStatement>("x", std::move(else0), loc);
    std::vector<std::unique_ptr<AstNode>> else_body;
    else_body.push_back(std::move(assignElse));

    // If statement itself
    auto ifStmt = std::make_unique<IfStatement>(std::move(cond), std::move(then_body), std::move(else_body), loc);

    write_ast_to_file("ast_test_if.txt", ifStmt.get());
}

// --- UNIT TEST 3: Routine Declaration ---
// Checks: routine calculate_sum(a, b) : integer is return a + b; end
void test_routine_declaration() {
    Location loc{3, 1};
    std::vector<std::string> params = {"a", "b"};
    
    // Body: return a + b
    auto varA = std::make_unique<Identifier>("a", loc);
    auto varB = std::make_unique<Identifier>("b", loc);
    auto plus = std::make_unique<BinaryExpr>(BinOp::Plus, std::move(varA), std::move(varB), loc);
    auto ret = std::make_unique<ReturnStatement>(std::move(plus), loc);

    std::vector<std::unique_ptr<AstNode>> body;
    body.push_back(std::move(ret));

    auto routine = std::make_unique<RoutineDeclaration>("calculate_sum", std::move(params), "integer", std::move(body), loc);
    
    write_ast_to_file("ast_test_routine.txt", routine.get());
}

// --- UNIT TEST 4: Variable Declaration ---
// Checks: var flag : boolean is true
void test_variable_declaration() {
    Location loc{4, 1};
    auto valTrue = std::make_unique<BoolLiteral>(true, loc);
    auto varDecl = std::make_unique<VariableDeclaration>("flag", "boolean", std::move(valTrue), loc);
    
    write_ast_to_file("ast_test_vardecl.txt", varDecl.get());
}

// --- UNIT TEST 5: While Loop ---
// Checks: while (x < 10) loop print(x); end
void test_while_loop() {
    Location loc{5, 1};
    auto varX = std::make_unique<Identifier>("x", loc);
    auto lit10 = std::make_unique<IntLiteral>(10, loc);
    auto cond = std::make_unique<BinaryExpr>(BinOp::Less, std::move(varX), std::move(lit10), loc);

    auto varXPrint = std::make_unique<Identifier>("x", loc);
    std::vector<std::unique_ptr<Expression>> printArgs;
    printArgs.push_back(std::move(varXPrint));
    auto printStmt = std::make_unique<PrintStatement>(std::move(printArgs), loc);

    std::vector<std::unique_ptr<AstNode>> body;
    body.push_back(std::move(printStmt));

    auto whileStmt = std::make_unique<WhileStatement>(std::move(cond), std::move(body), loc);
    
    write_ast_to_file("ast_test_while.txt", whileStmt.get());
}

// --- UNIT TEST 6: For Loop ---
// Checks: for i in 1 .. 10 reverse loop end
void test_for_loop() {
    Location loc{6, 1};
    auto start = std::make_unique<IntLiteral>(1, loc);
    auto end = std::make_unique<IntLiteral>(10, loc);
    
    std::vector<std::unique_ptr<AstNode>> body; // empty body
    
    // is_reverse = true
    auto forStmt = std::make_unique<ForStatement>("i", std::move(start), std::move(end), true, std::move(body), loc);
    
    write_ast_to_file("ast_test_for.txt", forStmt.get());
}

// --- UNIT TEST 7: Routine Call & Unary/Real ---
// Checks: my_func(-5, 3.14)
void test_routine_call() {
    Location loc{7, 1};
    
    auto lit5 = std::make_unique<IntLiteral>(5, loc);
    auto neg5 = std::make_unique<UnaryExpr>(UnaryOp::Minus, std::move(lit5), loc);
    auto litPi = std::make_unique<RealLiteral>(3.14, loc);
    
    std::vector<std::unique_ptr<Expression>> args;
    args.push_back(std::move(neg5));
    args.push_back(std::move(litPi));
    
    auto callStmt = std::make_unique<RoutineCallStmt>("my_func", std::move(args), loc);
    
    write_ast_to_file("ast_test_call.txt", callStmt.get());
}

int main() {
    std::cout << "Running AST Printer Tests...\n";
    
    test_math_expression();
    test_if_statement();
    test_routine_declaration();
    test_variable_declaration();
    test_while_loop();
    test_for_loop();
    test_routine_call();
    
    std::cout << "All 7 tests completed successfully!\n";
    return 0;
}
