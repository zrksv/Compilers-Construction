#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include "../ParserDriver.hpp"
#include "../../ast/AST.hpp"
#include "../../ast/ASTPrinter.hpp"
#include "TestUtils.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

void write_errors_to_file(const std::string& test_name, const std::string& code, const ParserDriver& driver) {
    std::ofstream out(test_output_dir() / (test_name + ".txt"), std::ios::out | std::ios::trunc);
    assert(out.is_open() && "Cannot open output file!");

    out << "=== " << test_name << " ===\n--- Source ---\n";
    std::istringstream lines(code);
    std::string line;
    for (size_t row = 1; std::getline(lines, line); ++row) {
        out << std::setw(3) << row << " | " << line << "\n";
    }

    out << "--- Errors (" << driver.get_errors().size() << ") ---\n";
    for (const auto& err : driver.get_errors()) {
        out << "Error at " << err.location.row << ":" << err.location.column
            << " - " << err.message << "\n";
    }
}

void test_operator_precedence() {
    std::string code = "routine main() is\n"
                       "    var x is 2 + 3 * 4;\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok && "Parsing failed for operator precedence");
    assert(driver.get_ast() != nullptr);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
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

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
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

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
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
    bool ok = driver.parse_file(resolve_path("01_print.imp"));
    assert(ok);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr);
    assert(routine->name == "main");
    assert(routine->body.size() == 1);

    auto* printStmt = dynamic_cast<PrintStatement*>(routine->body[0].get());
    assert(printStmt != nullptr);
    assert(printStmt->to_print.size() == 1);

    std::cout << "[PASS] test_sample_01 (01_print.imp)\n";
}

void test_sample_08() {
    ParserDriver driver;
    bool ok = driver.parse_file(resolve_path("08_for_loop.imp"));
    assert(ok && "Failed to parse 08_for_loop.imp");

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr);
    assert(routine->name == "main");
    assert(routine->body.size() == 4);

    std::cout << "[PASS] test_sample_08 (08_for_loop.imp)\n";
}

void test_type_declarations() {
    // 1. Primitive type alias
    {
        std::string code = "type IntAlias is integer\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* decl = dynamic_cast<TypeDeclaration*>(prog->declarations[0].get());
        assert(decl != nullptr);
        assert(decl->name == "IntAlias");
        auto* prim = dynamic_cast<PrimitiveTypeNode*>(decl->type.get());
        assert(prim != nullptr);
        assert(prim->type_name == "integer");
    }

    // 2. Custom identifier type alias
    {
        std::string code = "type MyType is IntAlias\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* decl = dynamic_cast<TypeDeclaration*>(prog->declarations[0].get());
        assert(decl != nullptr);
        assert(decl->name == "MyType");
        auto* idNode = dynamic_cast<IdentifierTypeNode*>(decl->type.get());
        assert(idNode != nullptr);
        assert(idNode->name == "IntAlias");
    }

    // 3. Sized array type
    {
        std::string code = "type SizedArr is array [10] integer\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* decl = dynamic_cast<TypeDeclaration*>(prog->declarations[0].get());
        assert(decl != nullptr);
        assert(decl->name == "SizedArr");
        auto* arrNode = dynamic_cast<ArrayTypeNode*>(decl->type.get());
        assert(arrNode != nullptr);
        assert(arrNode->size != nullptr);
        auto* szLit = dynamic_cast<IntLiteral*>(arrNode->size.get());
        assert(szLit != nullptr && szLit->value == 10);
        auto* elemPrim = dynamic_cast<PrimitiveTypeNode*>(arrNode->element_type.get());
        assert(elemPrim != nullptr && elemPrim->type_name == "integer");
    }

    // 4. Unsized array type
    {
        std::string code = "type UnsizedArr is array [] real\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* decl = dynamic_cast<TypeDeclaration*>(prog->declarations[0].get());
        assert(decl != nullptr);
        assert(decl->name == "UnsizedArr");
        auto* arrNode = dynamic_cast<ArrayTypeNode*>(decl->type.get());
        assert(arrNode != nullptr);
        assert(arrNode->size == nullptr);
        auto* elemPrim = dynamic_cast<PrimitiveTypeNode*>(arrNode->element_type.get());
        assert(elemPrim != nullptr && elemPrim->type_name == "real");
    }

    // 5. Record type
    {
        std::string code = "type Point is record\n"
                           "    var x : integer;\n"
                           "    var y : integer;\n"
                           "end\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* decl = dynamic_cast<TypeDeclaration*>(prog->declarations[0].get());
        assert(decl != nullptr);
        assert(decl->name == "Point");
        auto* recNode = dynamic_cast<RecordTypeNode*>(decl->type.get());
        assert(recNode != nullptr);
        assert(recNode->declarations.size() == 2);
        assert(recNode->declarations[0]->name == "x");
        assert(recNode->declarations[1]->name == "y");
    }

    std::cout << "[PASS] test_type_declarations (primitive, alias, array, record)\n";
}

void test_member_access() {
    std::string code = "routine test_access() is\n"
                       "    var a is pt.x;\n"
                       "    var b is arr[i];\n"
                       "    var c is arr.size;\n"
                       "    var d is arr.size();\n"
                       "    var e is mat[i][j];\n"
                       "    var f is circle.center.x;\n"
                       "    var g is heap[1].weight;\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok && "Parsing member access failed");

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr);
    assert(routine->body.size() == 7);

    // a is pt.x
    auto* v1 = dynamic_cast<VariableDeclaration*>(routine->body[0].get());
    auto* fa1 = dynamic_cast<FieldAccessExpr*>(v1->expression.get());
    assert(fa1 != nullptr && fa1->field_name == "x");

    // b is arr[i]
    auto* v2 = dynamic_cast<VariableDeclaration*>(routine->body[1].get());
    auto* ea1 = dynamic_cast<ElementAccessExpr*>(v2->expression.get());
    assert(ea1 != nullptr);

    // c is arr.size
    auto* v3 = dynamic_cast<VariableDeclaration*>(routine->body[2].get());
    auto* fa2 = dynamic_cast<FieldAccessExpr*>(v3->expression.get());
    assert(fa2 != nullptr && fa2->field_name == "size");

    // d is arr.size()
    auto* v4 = dynamic_cast<VariableDeclaration*>(routine->body[3].get());
    auto* fa3 = dynamic_cast<FieldAccessExpr*>(v4->expression.get());
    assert(fa3 != nullptr && fa3->field_name == "size");

    // e is mat[i][j]
    auto* v5 = dynamic_cast<VariableDeclaration*>(routine->body[4].get());
    auto* ea_outer = dynamic_cast<ElementAccessExpr*>(v5->expression.get());
    assert(ea_outer != nullptr);
    auto* ea_inner = dynamic_cast<ElementAccessExpr*>(ea_outer->array.get());
    assert(ea_inner != nullptr);

    // f is circle.center.x
    auto* v6 = dynamic_cast<VariableDeclaration*>(routine->body[5].get());
    auto* fa_outer = dynamic_cast<FieldAccessExpr*>(v6->expression.get());
    assert(fa_outer != nullptr && fa_outer->field_name == "x");
    auto* fa_inner = dynamic_cast<FieldAccessExpr*>(fa_outer->object.get());
    assert(fa_inner != nullptr && fa_inner->field_name == "center");

    // g is heap[1].weight
    auto* v7 = dynamic_cast<VariableDeclaration*>(routine->body[6].get());
    auto* fa_heap = dynamic_cast<FieldAccessExpr*>(v7->expression.get());
    assert(fa_heap != nullptr && fa_heap->field_name == "weight");
    auto* ea_heap = dynamic_cast<ElementAccessExpr*>(fa_heap->object.get());
    assert(ea_heap != nullptr);

    std::cout << "[PASS] test_member_access (field, element, size, chained, combined)\n";
}

void test_assignments_to_modifiable_primary() {
    std::string code = "routine test_assign() is\n"
                       "    x := 10;\n"
                       "    pt.x := 20;\n"
                       "    arr[i] := 42;\n"
                       "    mat[i][j] := 0;\n"
                       "    circle.center.x := 5;\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok && "Parsing assignments failed");

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr);
    assert(routine->body.size() == 5);

    // x := 10
    auto* a1 = dynamic_cast<AssignStatement*>(routine->body[0].get());
    assert(a1 != nullptr && dynamic_cast<Identifier*>(a1->target.get()) != nullptr);

    // pt.x := 20
    auto* a2 = dynamic_cast<AssignStatement*>(routine->body[1].get());
    assert(a2 != nullptr && dynamic_cast<FieldAccessExpr*>(a2->target.get()) != nullptr);

    // arr[i] := 42
    auto* a3 = dynamic_cast<AssignStatement*>(routine->body[2].get());
    assert(a3 != nullptr && dynamic_cast<ElementAccessExpr*>(a3->target.get()) != nullptr);

    // mat[i][j] := 0
    auto* a4 = dynamic_cast<AssignStatement*>(routine->body[3].get());
    assert(a4 != nullptr && dynamic_cast<ElementAccessExpr*>(a4->target.get()) != nullptr);

    // circle.center.x := 5
    auto* a5 = dynamic_cast<AssignStatement*>(routine->body[4].get());
    assert(a5 != nullptr && dynamic_cast<FieldAccessExpr*>(a5->target.get()) != nullptr);

    std::cout << "[PASS] test_assignments_to_modifiable_primary\n";
}

void test_routine_variations() {
    // Forward declaration
    {
        std::string code = "routine gcd(a : integer, b : integer) : integer;\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
        assert(routine != nullptr);
        assert(routine->name == "gcd");
        assert(routine->body_type == RoutineBodyType::Forward);
        assert(routine->parameters.size() == 2);
        assert(routine->parameters[0]->name == "a");
        assert(dynamic_cast<PrimitiveTypeNode*>(routine->parameters[0]->type.get()) != nullptr);
        assert(dynamic_cast<PrimitiveTypeNode*>(routine->parameters[0]->type.get())->type_name == "integer");
        assert(routine->parameters[1]->name == "b");
        assert(dynamic_cast<PrimitiveTypeNode*>(routine->parameters[1]->type.get()) != nullptr);
        assert(dynamic_cast<PrimitiveTypeNode*>(routine->parameters[1]->type.get())->type_name == "integer");
        assert(routine->return_type != nullptr);
        auto* retPrim = dynamic_cast<PrimitiveTypeNode*>(routine->return_type.get());
        assert(retPrim != nullptr && retPrim->type_name == "integer");
    }

    // Arrow body
    {
        std::string code = "routine square(x : integer) : integer => x * x\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
        assert(routine != nullptr);
        assert(routine->name == "square");
        assert(routine->body_type == RoutineBodyType::Expression);
        assert(routine->expression_body != nullptr);
        assert(routine->parameters.size() == 1);
        assert(routine->parameters[0]->name == "x");
        assert(routine->return_type != nullptr);
        auto* retPrim = dynamic_cast<PrimitiveTypeNode*>(routine->return_type.get());
        assert(retPrim != nullptr && retPrim->type_name == "integer");
    }

    // Routine with array parameter
    {
        std::string code = "routine sum(arr : array [] integer) : integer is\n"
                           "    return 0;\n"
                           "end\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
        assert(routine != nullptr);
        assert(routine->parameters.size() == 1);
        assert(routine->parameters[0]->name == "arr");
        auto* arrType = dynamic_cast<ArrayTypeNode*>(routine->parameters[0]->type.get());
        assert(arrType != nullptr);
        assert(arrType->size == nullptr);
        auto* elemPrim = dynamic_cast<PrimitiveTypeNode*>(arrType->element_type.get());
        assert(elemPrim != nullptr && elemPrim->type_name == "integer");
        assert(routine->return_type != nullptr);
    }

    // Empty routine
    {
        std::string code = "routine noop() is\nend\n";
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr && prog->declarations.size() == 1);
        auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
        assert(routine != nullptr);
        assert(routine->name == "noop");
        assert(routine->body_type == RoutineBodyType::Block);
        assert(routine->body.empty());
        assert(routine->parameters.empty());
        assert(routine->return_type == nullptr);
    }

    std::cout << "[PASS] test_routine_variations (forward, arrow, array param, empty)\n";
}

void test_program_multiple_declarations() {
    std::string code = "type IntArray is array [3] integer;\n"
                       "routine get_first(arr : IntArray) : integer is\n"
                       "    return arr[1];\n"
                       "end\n"
                       "routine main() is\n"
                       "    var a : IntArray;\n"
                       "    print get_first(a);\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok);
    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 3);
    assert(dynamic_cast<TypeDeclaration*>(prog->declarations[0].get()) != nullptr);
    assert(dynamic_cast<RoutineDeclaration*>(prog->declarations[1].get()) != nullptr);
    assert(dynamic_cast<RoutineDeclaration*>(prog->declarations[2].get()) != nullptr);
    std::cout << "[PASS] test_program_multiple_declarations\n";
}

void test_for_loop_array() {
    std::string code = "routine test_arr_for() is\n"
                       "    for item in items reverse loop\n"
                       "        print item;\n"
                       "    end\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok);
    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr && routine->body.size() == 1);
    auto* forStmt = dynamic_cast<ForStatement*>(routine->body[0].get());
    assert(forStmt != nullptr);
    assert(forStmt->iterator_name == "item");
    assert(forStmt->is_reverse == true);
    assert(forStmt->range_start != nullptr);
    assert(forStmt->range_end == nullptr);
    std::cout << "[PASS] test_for_loop_array\n";
}

void test_sample_10_forward() {
    ParserDriver driver;
    bool ok = driver.parse_file(resolve_path("10_forward_declaration.imp"));
    assert(ok && "Failed to parse 10_forward_declaration.imp");
    std::cout << "[PASS] test_sample_10_forward (10_forward_declaration.imp)\n";
}

void test_sample_16_records() {
    ParserDriver driver;
    bool ok = driver.parse_file(resolve_path("16_records.imp"));
    assert(ok && "Failed to parse 16_records.imp");
    std::cout << "[PASS] test_sample_16_records (16_records.imp)\n";
}

void test_sample_18_arrow() {
    ParserDriver driver;
    bool ok = driver.parse_file(resolve_path("18_arrow_routines.imp"));
    assert(ok && "Failed to parse 18_arrow_routines.imp");
    std::cout << "[PASS] test_sample_18_arrow (18_arrow_routines.imp)\n";
}

void test_scientific_notation() {
    std::string code = 
        "routine main() is\n"
        "    var a : real is 1e5;\n"
        "    var b : real is 1.5e-3;\n"
        "    var c is 2.0E+10;\n"
        "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok && "Failed to parse scientific notation in parser!");
    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr && routine->body.size() == 3);
    auto* v1 = dynamic_cast<VariableDeclaration*>(routine->body[0].get());
    auto* r1 = dynamic_cast<RealLiteral*>(v1->expression.get());
    assert(r1 != nullptr && r1->value == 1e5);
    auto* v2 = dynamic_cast<VariableDeclaration*>(routine->body[1].get());
    auto* r2 = dynamic_cast<RealLiteral*>(v2->expression.get());
    assert(r2 != nullptr && r2->value == 1.5e-3);
    auto* v3 = dynamic_cast<VariableDeclaration*>(routine->body[2].get());
    auto* r3 = dynamic_cast<RealLiteral*>(v3->expression.get());
    assert(r3 != nullptr && r3->value == 2.0e10);
    std::cout << "[PASS] test_scientific_notation\n";
}

void test_comments() {
    std::string code = 
        "// Program with comments\n"
        "/* Multi-line header\n"
        "   describing the routine */\n"
        "routine main() is\n"
        "    // Declare variable x\n"
        "    var x : integer is /* initial value */ 10;\n"
        "    /* Check condition */\n"
        "    if x > 5 then // inline condition comment\n"
        "        print x; // print x\n"
        "    end // end of if\n"
        "end // end of main\n";

    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok && "Parsing failed for program with comments!");
    assert(driver.get_ast() != nullptr);
    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);
    std::cout << "[PASS] test_comments (single-line and multi-line in parser)\n";
}

void test_sample_20_matrix() {
    ParserDriver driver;
    bool ok = driver.parse_file(resolve_path("20_array_of_array.imp"));
    assert(ok && "Failed to parse 20_array_of_array.imp");
    std::cout << "[PASS] test_sample_20_matrix (20_array_of_array.imp)\n";
}

void test_error_recovery_multiple_statements() {
    std::string code = "routine foo() is\n"
                       "    var a is 10;\n"
                       "    bad syntax here 123 + ;\n"
                       "    var b is 20;\n"
                       "    another broken @@ statement ;\n"
                       "    print a + b;\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(!ok && "Expected parsing to fail due to syntax errors");
    assert(driver.has_errors() && "Expected parser to report errors");
    assert(driver.get_errors().size() >= 2 && "Expected multiple errors to be recovered and reported");
    assert(prog != nullptr && "Expected non-null AST root despite syntax errors");
    assert(prog->declarations.size() == 1);

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr);
    assert(routine->name == "foo");
    assert(routine->body.size() == 3);

    auto* varA = dynamic_cast<VariableDeclaration*>(routine->body[0].get());
    assert(varA != nullptr && varA->name == "a");

    auto* varB = dynamic_cast<VariableDeclaration*>(routine->body[1].get());
    assert(varB != nullptr && varB->name == "b");

    auto* printStmt = dynamic_cast<PrintStatement*>(routine->body[2].get());
    assert(printStmt != nullptr);

    std::ostringstream oss;
    AstPrinter printer(oss);
    prog->accept(printer);
    assert(!oss.str().empty());

    std::cout << "[PASS] test_error_recovery_multiple_statements\n";
}

void test_error_recovery_top_level() {
    std::string code = "routine first() is\n"
                       "    print 1;\n"
                       "end;\n"
                       "bad top level declaration 999;\n"
                       "routine second() is\n"
                       "    print 2;\n"
                       "end;\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok && "Expected parsing to fail due to top-level syntax error");
    assert(driver.has_errors());
    assert(!driver.get_errors().empty());

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 2 && "Expected both valid routines to be recovered");

    auto* r1 = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(r1 != nullptr && r1->name == "first");

    auto* r2 = dynamic_cast<RoutineDeclaration*>(prog->declarations[1].get());
    assert(r2 != nullptr && r2->name == "second");

    std::ostringstream oss;
    AstPrinter printer(oss);
    prog->accept(printer);
    assert(!oss.str().empty());

    std::cout << "[PASS] test_error_recovery_top_level\n";
}

void test_error_recovery_record_fields() {
    std::string code = "type Point is record\n"
                       "    var x : integer;\n"
                       "    var broken_field : ;\n"
                       "    var y : integer;\n"
                       "end;\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok && "Expected parsing to fail due to record field error");
    assert(driver.has_errors());
    assert(!driver.get_errors().empty());

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);

    auto* typeDecl = dynamic_cast<TypeDeclaration*>(prog->declarations[0].get());
    assert(typeDecl != nullptr && typeDecl->name == "Point");

    auto* recType = dynamic_cast<RecordTypeNode*>(typeDecl->type.get());
    assert(recType != nullptr);
    assert(recType->declarations.size() == 2 && "Expected valid fields x and y to be recovered");
    assert(recType->declarations[0]->name == "x");
    assert(recType->declarations[1]->name == "y");

    std::ostringstream oss;
    AstPrinter printer(oss);
    prog->accept(printer);
    assert(!oss.str().empty());

    std::cout << "[PASS] test_error_recovery_record_fields\n";
}

void test_error_recovery_nested_block() {
    std::string code = "routine nested() is\n"
                       "    if true then\n"
                       "        var ok1 is 1;\n"
                       "        broken +++ syntax;\n"
                       "        var ok2 is 2;\n"
                       "    end\n"
                       "    print 42;\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok && "Expected parsing to fail due to nested syntax error");
    assert(driver.has_errors());

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr && routine->body.size() == 2);

    auto* ifStmt = dynamic_cast<IfStatement*>(routine->body[0].get());
    assert(ifStmt != nullptr);
    assert(ifStmt->then_body.size() == 2);

    auto* v1 = dynamic_cast<VariableDeclaration*>(ifStmt->then_body[0].get());
    assert(v1 != nullptr && v1->name == "ok1");

    auto* v2 = dynamic_cast<VariableDeclaration*>(ifStmt->then_body[1].get());
    assert(v2 != nullptr && v2->name == "ok2");

    auto* printStmt = dynamic_cast<PrintStatement*>(routine->body[1].get());
    assert(printStmt != nullptr);

    std::cout << "[PASS] test_error_recovery_nested_block\n";
}

void test_error_recovery_single_separator() {
    std::string code = "routine f() is\n"
                       "    var a is 1\n"
                       "    x := := 2\n"
                       "    var b is 2\n"
                       "end\n"
                       "var c is 3\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 1 && "Only the broken line must be reported");

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 2);

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr && routine->body.size() == 2);
    auto* varA = dynamic_cast<VariableDeclaration*>(routine->body[0].get());
    assert(varA != nullptr && varA->name == "a");
    auto* varB = dynamic_cast<VariableDeclaration*>(routine->body[1].get());
    assert(varB != nullptr && varB->name == "b");

    auto* varC = dynamic_cast<VariableDeclaration*>(prog->declarations[1].get());
    assert(varC != nullptr && varC->name == "c");

    std::cout << "[PASS] test_error_recovery_single_separator\n";
}

void test_error_recovery_keeps_previous_items() {
    std::string code = "var a is 1\n"
                       ")\n"
                       "type R is record\n"
                       "    var x : integer\n"
                       "    )\n"
                       "    var y : integer\n"
                       "end\n"
                       "routine f() is\n"
                       "    print 1\n"
                       "    )\n"
                       "    print 2\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 3);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 3);

    auto* varA = dynamic_cast<VariableDeclaration*>(prog->declarations[0].get());
    assert(varA != nullptr && varA->name == "a");

    auto* typeDecl = dynamic_cast<TypeDeclaration*>(prog->declarations[1].get());
    assert(typeDecl != nullptr);
    auto* recType = dynamic_cast<RecordTypeNode*>(typeDecl->type.get());
    assert(recType != nullptr && recType->declarations.size() == 2);
    assert(recType->declarations[0]->name == "x");
    assert(recType->declarations[1]->name == "y");

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[2].get());
    assert(routine != nullptr && routine->body.size() == 2);

    std::cout << "[PASS] test_error_recovery_keeps_previous_items\n";
}

void test_error_recovery_first_item() {
    std::string code = "bad top level\n"
                       "type R is record\n"
                       "    var : integer\n"
                       "    var y : integer\n"
                       "end\n"
                       "routine f() is\n"
                       "    x := ;\n"
                       "    print 1\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 3);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 2);

    auto* typeDecl = dynamic_cast<TypeDeclaration*>(prog->declarations[0].get());
    assert(typeDecl != nullptr);
    auto* recType = dynamic_cast<RecordTypeNode*>(typeDecl->type.get());
    assert(recType != nullptr && recType->declarations.size() == 1);
    assert(recType->declarations[0]->name == "y");

    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[1].get());
    assert(routine != nullptr && routine->body.size() == 1);
    assert(dynamic_cast<PrintStatement*>(routine->body[0].get()) != nullptr);

    std::cout << "[PASS] test_error_recovery_first_item\n";
}

void test_error_recovery_last_item() {
    std::string code = "routine f() is\n"
                       "    print 1\n"
                       "    x := end\n"
                       "routine g() is print 2 end\n"
                       "var z is";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 2);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 2);

    auto* f = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(f != nullptr && f->name == "f" && f->body.size() == 1);
    auto* g = dynamic_cast<RoutineDeclaration*>(prog->declarations[1].get());
    assert(g != nullptr && g->name == "g");

    std::cout << "[PASS] test_error_recovery_last_item\n";
}

void test_error_recovery_compound_headers() {
    std::string code = "routine f() is\n"
                       "    if x + then\n"
                       "        print 1\n"
                       "    else\n"
                       "        print 2\n"
                       "    end\n"
                       "    while 1 +\n"
                       "        print 3\n"
                       "    end\n"
                       "    for i in 1 .. loop\n"
                       "        print 4\n"
                       "    end\n"
                       "    print 5\n"
                       "end\n"
                       "routine g() is print 6 end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 3);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 2);

    auto* f = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(f != nullptr && f->name == "f");
    assert(f->body.size() == 1 && "Broken statements are dropped, the rest of the body is kept");
    assert(dynamic_cast<PrintStatement*>(f->body[0].get()) != nullptr);

    auto* g = dynamic_cast<RoutineDeclaration*>(prog->declarations[1].get());
    assert(g != nullptr && g->name == "g");

    std::cout << "[PASS] test_error_recovery_compound_headers\n";
}

void test_error_recovery_routine_header() {
    std::string code = "routine a(x integer) is\n"
                       "    print 1\n"
                       "end\n"
                       "routine b(x: integer): integer => x +\n"
                       "routine c(x: ) \n"
                       "routine d() is print 2 end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 3);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1);

    auto* d = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(d != nullptr && d->name == "d");

    std::cout << "[PASS] test_error_recovery_routine_header\n";
}

void test_newline_inside_parentheses() {
    std::string code = "routine add(a : integer,\n"
                       "            b : integer) : integer => (a +\n"
                       "    b)\n"
                       "routine main() is\n"
                       "    var x is (1 +\n"
                       "        2) * 3\n"
                       "    print add(x,\n"
                       "\n"
                       "        4)\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok && "Newlines inside parentheses must be ignored");

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 2);

    auto* add = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(add != nullptr && add->parameters.size() == 2);
    assert(dynamic_cast<BinaryExpr*>(add->expression_body.get()) != nullptr);

    auto* main = dynamic_cast<RoutineDeclaration*>(prog->declarations[1].get());
    assert(main != nullptr && main->body.size() == 2);
    auto* x = dynamic_cast<VariableDeclaration*>(main->body[0].get());
    assert(x != nullptr);
    auto* mul = dynamic_cast<BinaryExpr*>(x->expression.get());
    assert(mul != nullptr && mul->op == BinOp::Mul);
    auto* call = dynamic_cast<PrintStatement*>(main->body[1].get());
    assert(call != nullptr);

    std::cout << "[PASS] test_newline_inside_parentheses\n";
}

void test_newline_outside_parentheses() {
    ParserDriver driver;
    assert(!driver.parse("routine main() is\n    var x is 1 +\n        2\nend\n"));
    assert(!driver.parse("routine main() is\n    var a : array [3] integer\n    a[1\n] := 2\nend\n"));
    std::cout << "[PASS] test_newline_outside_parentheses\n";
}

void test_error_recovery_unclosed_parenthesis() {
    std::string code = "routine main() is\n"
                       "    print (1 + 2\n"
                       "    print 3\n"
                       "    var y is 4\n"
                       "end\n"
                       "routine g() is print 5 end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 1);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 2);
    auto* main = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(main != nullptr && main->body.size() == 1);
    auto* y = dynamic_cast<VariableDeclaration*>(main->body[0].get());
    assert(y != nullptr && y->name == "y");

    std::cout << "[PASS] test_error_recovery_unclosed_parenthesis\n";
}

void test_unknown_character_top_level() {
    std::string code = "var a is @5\n"
                       "var b is 2\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 1 && "Only the lexical error must be reported");
    const auto& err = driver.get_errors()[0];
    assert(err.location.row == 1 && err.location.column == 10);
    assert(err.message.find("'@'") != std::string::npos);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr);
    assert(prog->declarations.size() == 1 && "The declaration with '@' must not reach the AST");
    auto* varB = dynamic_cast<VariableDeclaration*>(prog->declarations[0].get());
    assert(varB != nullptr && varB->name == "b");

    std::cout << "[PASS] test_unknown_character_top_level\n";
}

void test_unknown_character_in_body() {
    std::string code = "routine f() is\n"
                       "    var a is 1\n"
                       "    print $\n"
                       "    var b is 2\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 1);
    assert(driver.get_errors()[0].location.row == 3);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* routine = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(routine != nullptr && routine->body.size() == 2);
    auto* varA = dynamic_cast<VariableDeclaration*>(routine->body[0].get());
    assert(varA != nullptr && varA->name == "a");
    auto* varB = dynamic_cast<VariableDeclaration*>(routine->body[1].get());
    assert(varB != nullptr && varB->name == "b");

    std::cout << "[PASS] test_unknown_character_in_body\n";
}

void test_unknown_characters_each_reported() {
    std::string code = "var x is 1 @# 2\n"
                       "var y is 3\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 2);
    for (const auto& err : driver.get_errors()) {
        assert(err.message.find("syntax error") == std::string::npos);
    }

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* varY = dynamic_cast<VariableDeclaration*>(prog->declarations[0].get());
    assert(varY != nullptr && varY->name == "y");

    std::cout << "[PASS] test_unknown_characters_each_reported\n";
}

void test_program_location_is_file_start() {
    const std::vector<std::string> codes = {
        "var x is 1\n",
        "  \n\n  ; var x is 1\n",
        "\n\t\n",
        "",
    };
    for (const auto& code : codes) {
        ParserDriver driver;
        bool ok = driver.parse(code);
        assert(ok);
        auto* prog = dynamic_cast<Program*>(driver.get_ast());
        assert(prog != nullptr);
        assert(prog->location.row == 1 && prog->location.column == 1);
    }

    std::cout << "[PASS] test_program_location_is_file_start\n";
}

void test_ast_null_after_unclosed_block_at_eof() {
    std::string code = "routine first() is\n"
                       "    print 1\n"
                       "end\n"
                       "routine second() is\n"
                       "    print 2\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(!ok);
    assert(driver.get_errors().size() == 1);
    const auto& err = driver.get_errors()[0];
    assert(err.location.row == 6 && err.location.column == 1);
    assert(err.message.find("end of file") != std::string::npos);
    assert(driver.get_ast() == nullptr && "No substitute root after an aborted parse");
    assert(driver.take_ast() == nullptr);

    assert(driver.parse("var x is 1\n"));
    assert(driver.get_ast() != nullptr && "A later successful parse builds a fresh root");

    std::cout << "[PASS] test_ast_null_after_unclosed_block_at_eof\n";
}

void test_parameterless_routine_call_requires_parentheses() {
    std::string code = "routine tick() is\n"
                       "    print 1\n"
                       "end\n"
                       "routine one() : integer => 1\n"
                       "routine main() is\n"
                       "    tick()\n"
                       "    var x is one()\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 3);
    auto* tick = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(tick != nullptr && tick->parameters.empty());

    auto* main = dynamic_cast<RoutineDeclaration*>(prog->declarations[2].get());
    assert(main != nullptr && main->body.size() == 2);
    auto* callStmt = dynamic_cast<RoutineCallStmt*>(main->body[0].get());
    assert(callStmt != nullptr && callStmt->routine_name == "tick" && callStmt->arguments.empty());
    auto* x = dynamic_cast<VariableDeclaration*>(main->body[1].get());
    assert(x != nullptr);
    auto* callExpr = dynamic_cast<RoutineCallExpr*>(x->expression.get());
    assert(callExpr != nullptr && callExpr->routine_name == "one" && callExpr->arguments.empty());

    ParserDriver bare;
    assert(!bare.parse("routine tick() is\n    print 1\nend\nroutine main() is\n    tick\nend\n"));
    assert(bare.get_errors().size() == 1);
    assert(bare.get_errors()[0].location.row == 5);

    std::cout << "[PASS] test_parameterless_routine_call_requires_parentheses\n";
}

void test_error_recovery_before_else() {
    std::string code = "routine f() is\n"
                       "    if true then\n"
                       "        print 1\n"
                       "        x :=\n"
                       "    else\n"
                       "        print 2\n"
                       "    end\n"
                       "    print 3\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 1);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* f = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(f != nullptr && f->body.size() == 2);
    auto* ifStmt = dynamic_cast<IfStatement*>(f->body[0].get());
    assert(ifStmt != nullptr);
    assert(ifStmt->then_body.size() == 1);
    assert(ifStmt->else_body.size() == 1);
    assert(dynamic_cast<PrintStatement*>(f->body[1].get()) != nullptr);

    std::cout << "[PASS] test_error_recovery_before_else\n";
}

void test_error_recovery_consecutive_broken_lines() {
    std::string code = "routine f() is\n"
                       "    x :=\n"
                       "    y :=\n"
                       "    print 1\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 2);
    assert(driver.get_errors()[0].location.row == 2);
    assert(driver.get_errors()[1].location.row == 3);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* f = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(f != nullptr && f->body.size() == 1);
    assert(dynamic_cast<PrintStatement*>(f->body[0].get()) != nullptr);

    std::cout << "[PASS] test_error_recovery_consecutive_broken_lines\n";
}

void test_error_recovery_header_without_keyword() {
    std::string code = "routine f() is\n"
                       "    if x\n"
                       "        print 1\n"
                       "    else\n"
                       "        print 2\n"
                       "    end\n"
                       "    while 1 + loop\n"
                       "        print 3\n"
                       "    end\n"
                       "    print 4\n"
                       "end\n"
                       "routine g() is print 5 end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 2);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 2);
    auto* f = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(f != nullptr && f->body.size() == 1);
    assert(dynamic_cast<PrintStatement*>(f->body[0].get()) != nullptr);
    auto* g = dynamic_cast<RoutineDeclaration*>(prog->declarations[1].get());
    assert(g != nullptr && g->name == "g");

    std::cout << "[PASS] test_error_recovery_header_without_keyword\n";
}

void test_error_recovery_tokens_after_end() {
    std::string code = "routine f() is\n"
                       "    while true loop print 1 end print 2\n"
                       "    print 3\n"
                       "end\n"
                       "type R is record var x : integer end var y is 1\n"
                       "routine g() is print 4 end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    write_errors_to_file(__func__, code, driver);
    assert(!ok);
    assert(driver.get_errors().size() == 2);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 3);

    auto* f = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(f != nullptr && f->body.size() == 2);
    auto* loop = dynamic_cast<WhileStatement*>(f->body[0].get());
    assert(loop != nullptr && loop->body.size() == 1);
    assert(dynamic_cast<PrintStatement*>(f->body[1].get()) != nullptr);

    auto* typeDecl = dynamic_cast<TypeDeclaration*>(prog->declarations[1].get());
    assert(typeDecl != nullptr);
    auto* rec = dynamic_cast<RecordTypeNode*>(typeDecl->type.get());
    assert(rec != nullptr && rec->declarations.size() == 1);

    auto* g = dynamic_cast<RoutineDeclaration*>(prog->declarations[2].get());
    assert(g != nullptr && g->name == "g");

    std::cout << "[PASS] test_error_recovery_tokens_after_end\n";
}

void test_untyped_parameter_is_syntax_error() {
    std::string code = "routine f(x) is\n"
                       "    print x\n"
                       "end\n"
                       "routine g(a : integer, b) : integer => a\n"
                       "routine main() is\n"
                       "    print 1\n"
                       "end\n";
    ParserDriver driver;
    assert(!driver.parse(code));
    write_errors_to_file(__func__, code, driver);
    assert(driver.get_errors().size() == 2);
    assert(driver.get_errors()[0].location.row == 1);
    assert(driver.get_errors()[1].location.row == 4);

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 1);
    auto* main = dynamic_cast<RoutineDeclaration*>(prog->declarations[0].get());
    assert(main != nullptr && main->name == "main");

    std::cout << "[PASS] test_untyped_parameter_is_syntax_error\n";
}

void test_line_continuation() {
    std::string code = "type point is \\\n"
                       "    record var x : integer; var y : integer end\n"
                       "routine add(a : integer, b : integer) \\\n"
                       "    : integer => a + \\\n"
                       "        b\n"
                       "routine main() \\   \n"
                       "is\n"
                       "    var s is 1 + \\\n"
                       "        2 * 3\n"
                       "    if s > 0 \\\n"
                       "        and s < 10 then print s end\n"
                       "    for i in 1 .. \\\n"
                       "        s reverse loop print i end\n"
                       "    var p : point\n"
                       "    p.x \\\n"
                       "        := add(1, \\\n"
                       "            2)\n"
                       "    print s, \\\n"
                       "        p.x\n"
                       "end\n";
    ParserDriver driver;
    bool ok = driver.parse(code);
    assert(ok && "A backslash before a newline must join the lines");

    auto* prog = dynamic_cast<Program*>(driver.get_ast());
    assert(prog != nullptr && prog->declarations.size() == 3);

    auto* add = dynamic_cast<RoutineDeclaration*>(prog->declarations[1].get());
    assert(add != nullptr && add->parameters.size() == 2 && add->return_type != nullptr);
    auto* sum = dynamic_cast<BinaryExpr*>(add->expression_body.get());
    assert(sum != nullptr && sum->op == BinOp::Plus);

    auto* main = dynamic_cast<RoutineDeclaration*>(prog->declarations[2].get());
    assert(main != nullptr && main->body.size() == 6);
    auto* s = dynamic_cast<VariableDeclaration*>(main->body[0].get());
    assert(s != nullptr);
    auto* plus = dynamic_cast<BinaryExpr*>(s->expression.get());
    assert(plus != nullptr && plus->op == BinOp::Plus);
    assert(dynamic_cast<IfStatement*>(main->body[1].get()) != nullptr);
    auto* loop = dynamic_cast<ForStatement*>(main->body[2].get());
    assert(loop != nullptr && loop->range_end != nullptr && loop->is_reverse);
    assert(dynamic_cast<AssignStatement*>(main->body[4].get()) != nullptr);
    auto* print = dynamic_cast<PrintStatement*>(main->body[5].get());
    assert(print != nullptr && print->to_print.size() == 2);

    std::cout << "[PASS] test_line_continuation\n";
}

void test_line_continuation_edge_cases() {
    ParserDriver driver;
    assert(!driver.parse("routine main() is\n    var x is 1 \\ + 2\nend\n"));
    assert(driver.get_errors().size() == 1);
    assert(driver.get_errors()[0].location.row == 2);

    ParserDriver blank;
    assert(!blank.parse("routine main() is\n    var x is 1 + \\\n\n        2\nend\n"));

    ParserDriver at_eof;
    assert(at_eof.parse("routine main() is\n    print 1\nend \\"));
    assert(at_eof.parse("var x is 1 \\  "));

    std::cout << "[PASS] test_line_continuation_edge_cases\n";
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

    test_type_declarations();
    test_member_access();
    test_assignments_to_modifiable_primary();
    test_routine_variations();
    test_program_multiple_declarations();
    test_for_loop_array();
    test_sample_10_forward();
    test_sample_16_records();
    test_sample_18_arrow();
    test_sample_20_matrix();
    test_comments();
    test_scientific_notation();

    test_error_recovery_multiple_statements();
    test_error_recovery_top_level();
    test_error_recovery_record_fields();
    test_error_recovery_nested_block();
    test_error_recovery_single_separator();
    test_error_recovery_keeps_previous_items();
    test_error_recovery_first_item();
    test_error_recovery_last_item();
    test_error_recovery_compound_headers();
    test_error_recovery_routine_header();

    test_newline_inside_parentheses();
    test_newline_outside_parentheses();
    test_line_continuation();
    test_line_continuation_edge_cases();
    test_error_recovery_unclosed_parenthesis();

    test_unknown_character_top_level();
    test_unknown_character_in_body();
    test_unknown_characters_each_reported();

    test_program_location_is_file_start();
    test_ast_null_after_unclosed_block_at_eof();
    test_parameterless_routine_call_requires_parentheses();
    test_untyped_parameter_is_syntax_error();

    test_error_recovery_before_else();
    test_error_recovery_consecutive_broken_lines();
    test_error_recovery_header_without_keyword();
    test_error_recovery_tokens_after_end();

    std::cout << "All parser tests passed successfully!\n";
    return 0;
}
