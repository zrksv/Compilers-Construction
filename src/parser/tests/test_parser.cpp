#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>
#include "../ParserDriver.hpp"
#include "../../ast/AST.hpp"
#include "../../ast/ASTPrinter.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static bool parse_sample_helper(ParserDriver& driver, const std::string& sample_rel_path) {
    std::vector<std::string> prefixes = {
        "",
        "../",
        "../../",
        "../../../",
        "../../../../"
    };
    for (const auto& pre : prefixes) {
        if (driver.parse_file(pre + sample_rel_path)) {
            return true;
        }
        driver.clear_errors();
    }
    return false;
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
    bool ok = parse_sample_helper(driver, "tests/samples/01_print.imp");
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
    bool ok = parse_sample_helper(driver, "tests/samples/08_for_loop.imp");
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
    bool ok = parse_sample_helper(driver, "tests/samples/10_forward_declaration.imp");
    assert(ok && "Failed to parse 10_forward_declaration.imp");
    std::cout << "[PASS] test_sample_10_forward (10_forward_declaration.imp)\n";
}

void test_sample_16_records() {
    ParserDriver driver;
    bool ok = parse_sample_helper(driver, "tests/samples/16_records.imp");
    assert(ok && "Failed to parse 16_records.imp");
    std::cout << "[PASS] test_sample_16_records (16_records.imp)\n";
}

void test_sample_18_arrow() {
    ParserDriver driver;
    bool ok = parse_sample_helper(driver, "tests/samples/18_arrow_routines.imp");
    assert(ok && "Failed to parse 18_arrow_routines.imp");
    std::cout << "[PASS] test_sample_18_arrow (18_arrow_routines.imp)\n";
}

void test_sample_20_matrix() {
    ParserDriver driver;
    bool ok = parse_sample_helper(driver, "tests/samples/20_array_of_array.imp");
    assert(ok && "Failed to parse 20_array_of_array.imp");
    std::cout << "[PASS] test_sample_20_matrix (20_array_of_array.imp)\n";
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

    std::cout << "All parser tests passed successfully!\n";
    return 0;
}
