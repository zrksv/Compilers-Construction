#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include "../ASTPrinter.hpp"
#include "../AST.hpp"

std::string ast_to_string(AstNode* root) {
    std::ostringstream oss;
    AstPrinter printer(oss);
    root->accept(printer);
    return oss.str();
}

void write_ast_to_file(const std::string& output_filename, AstNode* root) {
    std::filesystem::path sourcePath(__FILE__);
    std::filesystem::path outDir = sourcePath.parent_path() / "output";
    std::error_code ec;
    std::filesystem::create_directories(outDir, ec);
    std::filesystem::path outPath = outDir / output_filename;

    std::ofstream outFile(outPath);
    if (outFile.is_open()) {
        AstPrinter filePrinter(outFile);
        root->accept(filePrinter);
        std::cout << "Successfully written AST to: output/" << outPath.filename().string() << "\n";
    } else {
        std::cerr << "Failed to open file for writing: " << outPath.string() << "\n";
    }
}

void verify_and_write(const std::string& output_filename, AstNode* root, const std::string& expected) {
    std::string actual = ast_to_string(root);
    assert(actual == expected);
    write_ast_to_file(output_filename, root);
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
    
    assert(add != nullptr);
    assert(add->op == BinOp::Plus);
    assert(dynamic_cast<IntLiteral*>(add->left.get()) != nullptr);
    assert(dynamic_cast<IntLiteral*>(add->left.get())->value == 2);
    assert(dynamic_cast<BinaryExpr*>(add->right.get()) != nullptr);
    assert(dynamic_cast<BinaryExpr*>(add->right.get())->op == BinOp::Mul);

    std::string expected = 
        "BinaryExpr(+)\n"
        "├── Left: IntLiteral(2)\n"
        "└── Right: BinaryExpr(*)\n"
        "    ├── Left: Identifier(x)\n"
        "    └── Right: IntLiteral(3)";

    verify_and_write("ast_test_math.txt", add.get(), expected);
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

    assert(ifStmt != nullptr);
    assert(ifStmt->condition != nullptr);
    assert(ifStmt->then_body.size() == 1);
    assert(ifStmt->else_body.size() == 1);

    std::string expected = 
        "IfStatement\n"
        "├── Condition: BinaryExpr(<)\n"
        "│   ├── Left: Identifier(x)\n"
        "│   └── Right: IntLiteral(10)\n"
        "├── Then:\n"
        "│   └── AssignStatement(x)\n"
        "│       └── Value: BinaryExpr(+)\n"
        "│           ├── Left: Identifier(x)\n"
        "│           └── Right: IntLiteral(1)\n"
        "└── Else:\n"
        "    └── AssignStatement(x)\n"
        "        └── Value: IntLiteral(0)";

    verify_and_write("ast_test_if.txt", ifStmt.get(), expected);
}

// --- UNIT TEST 3: Routine Declaration (Block body) ---
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
    
    assert(routine != nullptr);
    assert(routine->name == "calculate_sum");
    assert(routine->parameters.size() == 2);
    assert(routine->return_type == "integer");
    assert(routine->body_type == RoutineBodyType::Block);
    assert(routine->body.size() == 1);

    std::string expected = 
        "RoutineDeclaration(calculate_sum) params: [a, b] returns: integer\n"
        "└── Body:\n"
        "    └── ReturnStatement\n"
        "        └── Value: BinaryExpr(+)\n"
        "            ├── Left: Identifier(a)\n"
        "            └── Right: Identifier(b)";

    verify_and_write("ast_test_routine.txt", routine.get(), expected);
}

// --- UNIT TEST 4: Variable Declaration (Primitive string) ---
// Checks: var flag : boolean is true
void test_variable_declaration() {
    Location loc{4, 1};
    auto valTrue = std::make_unique<BoolLiteral>(true, loc);
    auto varDecl = std::make_unique<VariableDeclaration>("flag", "boolean", std::move(valTrue), loc);
    
    assert(varDecl != nullptr);
    assert(varDecl->name == "flag");
    assert(varDecl->type == "boolean");
    assert(varDecl->expression != nullptr);

    std::string expected = 
        "VariableDeclaration(flag : boolean)\n"
        "└── Init: BoolLiteral(true)";

    verify_and_write("ast_test_vardecl.txt", varDecl.get(), expected);
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
    
    assert(whileStmt != nullptr);
    assert(whileStmt->condition != nullptr);
    assert(whileStmt->body.size() == 1);

    std::string expected = 
        "WhileLoop\n"
        "├── Condition: BinaryExpr(<)\n"
        "│   ├── Left: Identifier(x)\n"
        "│   └── Right: IntLiteral(10)\n"
        "└── Body:\n"
        "    └── PrintStatement\n"
        "        └── Arg: Identifier(x)";

    verify_and_write("ast_test_while.txt", whileStmt.get(), expected);
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
    
    assert(forStmt != nullptr);
    assert(forStmt->iterator_name == "i");
    assert(forStmt->is_reverse == true);
    assert(forStmt->range_start != nullptr);
    assert(forStmt->range_end != nullptr);

    std::string expected = 
        "ForLoop(i reverse)\n"
        "├── Start: IntLiteral(1)\n"
        "├── End: IntLiteral(10)\n"
        "└── Body:\n";

    verify_and_write("ast_test_for.txt", forStmt.get(), expected);
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
    
    assert(callStmt != nullptr);
    assert(callStmt->routine_name == "my_func");
    assert(callStmt->arguments.size() == 2);

    std::string expected = 
        "RoutineCallStmt(my_func)\n"
        "├── Arg: UnaryExpr(-)\n"
        "│   └── Child: IntLiteral(5)\n"
        "└── Arg: RealLiteral(3.14)";

    verify_and_write("ast_test_call.txt", callStmt.get(), expected);
}

// --- UNIT TEST 8: Primitive and Alias Type Declarations ---
// Checks:
// type IntAlias is integer
// type MyType is IntAlias
void test_type_aliases() {
    Location loc{8, 1};
    
    auto prim = std::make_unique<PrimitiveTypeNode>("integer", loc);
    auto alias1 = std::make_unique<TypeDeclaration>("IntAlias", std::move(prim), loc);
    
    assert(alias1 != nullptr);
    assert(alias1->name == "IntAlias");
    assert(dynamic_cast<PrimitiveTypeNode*>(alias1->type.get()) != nullptr);
    assert(dynamic_cast<PrimitiveTypeNode*>(alias1->type.get())->type_name == "integer");

    verify_and_write("ast_test_type_alias_primitive.txt", alias1.get(),
        "TypeDeclaration(IntAlias)\n"
        "└── Type: PrimitiveType(integer)");

    auto custom = std::make_unique<IdentifierTypeNode>("IntAlias", loc);
    auto alias2 = std::make_unique<TypeDeclaration>("MyType", std::move(custom), loc);
    
    assert(alias2 != nullptr);
    assert(alias2->name == "MyType");
    assert(dynamic_cast<IdentifierTypeNode*>(alias2->type.get()) != nullptr);
    assert(dynamic_cast<IdentifierTypeNode*>(alias2->type.get())->name == "IntAlias");

    verify_and_write("ast_test_type_alias_custom.txt", alias2.get(),
        "TypeDeclaration(MyType)\n"
        "└── Type: IdentifierType(IntAlias)");
}

// --- UNIT TEST 9: Array Type Declarations ---
// Checks:
// type SizedArr is array [10] integer
// type UnsizedArr is array [] real
void test_array_types() {
    Location loc{9, 1};
    
    // array [10] integer
    auto sz10 = std::make_unique<IntLiteral>(10, loc);
    auto elemInt = std::make_unique<PrimitiveTypeNode>("integer", loc);
    auto arrSized = std::make_unique<ArrayTypeNode>(std::move(sz10), std::move(elemInt), loc);
    auto typeDeclSized = std::make_unique<TypeDeclaration>("SizedArr", std::move(arrSized), loc);
    
    assert(typeDeclSized != nullptr);
    assert(typeDeclSized->name == "SizedArr");
    auto* arrNode1 = dynamic_cast<ArrayTypeNode*>(typeDeclSized->type.get());
    assert(arrNode1 != nullptr);
    assert(arrNode1->size != nullptr);
    assert(dynamic_cast<IntLiteral*>(arrNode1->size.get())->value == 10);
    assert(arrNode1->element_type != nullptr);

    verify_and_write("ast_test_type_array_sized.txt", typeDeclSized.get(),
        "TypeDeclaration(SizedArr)\n"
        "└── Type: ArrayType\n"
        "    ├── Size: IntLiteral(10)\n"
        "    └── Element: PrimitiveType(integer)");

    // array [] real (unsized parameter type)
    auto elemReal = std::make_unique<PrimitiveTypeNode>("real", loc);
    auto arrUnsized = std::make_unique<ArrayTypeNode>(nullptr, std::move(elemReal), loc);
    auto typeDeclUnsized = std::make_unique<TypeDeclaration>("UnsizedArr", std::move(arrUnsized), loc);
    
    assert(typeDeclUnsized != nullptr);
    auto* arrNode2 = dynamic_cast<ArrayTypeNode*>(typeDeclUnsized->type.get());
    assert(arrNode2 != nullptr);
    assert(arrNode2->size == nullptr);

    verify_and_write("ast_test_type_array_unsized.txt", typeDeclUnsized.get(),
        "TypeDeclaration(UnsizedArr)\n"
        "└── Type: ArrayType\n"
        "    ├── Size: empty\n"
        "    └── Element: PrimitiveType(real)");
}

// --- UNIT TEST 10: Record Type Declarations ---
// Checks:
// type Point is record var x : integer; var y : integer; end
// type Circle is record var center : Point; var radius : real; end
void test_record_types() {
    Location loc{10, 1};
    
    // type Point is record var x: integer; var y: integer; end
    std::vector<std::unique_ptr<VariableDeclaration>> pointFields;
    pointFields.push_back(std::make_unique<VariableDeclaration>("x", "integer", nullptr, loc));
    pointFields.push_back(std::make_unique<VariableDeclaration>("y", "integer", nullptr, loc));
    auto pointRecord = std::make_unique<RecordTypeNode>(std::move(pointFields), loc);
    auto pointDecl = std::make_unique<TypeDeclaration>("Point", std::move(pointRecord), loc);
    
    assert(pointDecl != nullptr);
    auto* recNode1 = dynamic_cast<RecordTypeNode*>(pointDecl->type.get());
    assert(recNode1 != nullptr);
    assert(recNode1->declarations.size() == 2);
    assert(recNode1->declarations[0]->name == "x");
    assert(recNode1->declarations[1]->name == "y");

    verify_and_write("ast_test_type_record_point.txt", pointDecl.get(),
        "TypeDeclaration(Point)\n"
        "└── Type: RecordType\n"
        "    ├── VariableDeclaration(x : integer)\n"
        "    └── VariableDeclaration(y : integer)");

    // type Circle is record var center: Point; var radius: real; end
    std::vector<std::unique_ptr<VariableDeclaration>> circleFields;
    auto pointTypeNode = std::make_unique<IdentifierTypeNode>("Point", loc);
    circleFields.push_back(std::make_unique<VariableDeclaration>("center", std::move(pointTypeNode), nullptr, loc));
    circleFields.push_back(std::make_unique<VariableDeclaration>("radius", "real", nullptr, loc));
    auto circleRecord = std::make_unique<RecordTypeNode>(std::move(circleFields), loc);
    auto circleDecl = std::make_unique<TypeDeclaration>("Circle", std::move(circleRecord), loc);
    
    assert(circleDecl != nullptr);
    auto* recNode2 = dynamic_cast<RecordTypeNode*>(circleDecl->type.get());
    assert(recNode2 != nullptr);
    assert(recNode2->declarations.size() == 2);
    assert(recNode2->declarations[0]->type_node != nullptr);

    verify_and_write("ast_test_type_record_circle.txt", circleDecl.get(),
        "TypeDeclaration(Circle)\n"
        "└── Type: RecordType\n"
        "    ├── VariableDeclaration(center)\n"
        "    │   └── Type: IdentifierType(Point)\n"
        "    └── VariableDeclaration(radius : real)");
}

// --- UNIT TEST 11: Variable Declaration with TypeNode & Initializer ---
// Checks:
// var buffer : array [256] integer
// var pt : Point
void test_variables_with_type_node() {
    Location loc{11, 1};
    
    // var buffer : array [256] integer
    auto sz256 = std::make_unique<IntLiteral>(256, loc);
    auto elemInt = std::make_unique<PrimitiveTypeNode>("integer", loc);
    auto arrType = std::make_unique<ArrayTypeNode>(std::move(sz256), std::move(elemInt), loc);
    auto varBuf = std::make_unique<VariableDeclaration>("buffer", std::move(arrType), nullptr, loc);
    
    assert(varBuf != nullptr);
    assert(varBuf->name == "buffer");
    assert(varBuf->type_node != nullptr);
    assert(varBuf->expression == nullptr);

    verify_and_write("ast_test_var_array.txt", varBuf.get(),
        "VariableDeclaration(buffer)\n"
        "└── Type: ArrayType\n"
        "    ├── Size: IntLiteral(256)\n"
        "    └── Element: PrimitiveType(integer)");

    // var pt : Point
    auto ptType = std::make_unique<IdentifierTypeNode>("Point", loc);
    auto varPt = std::make_unique<VariableDeclaration>("pt", std::move(ptType), nullptr, loc);
    
    assert(varPt != nullptr);
    assert(varPt->name == "pt");
    assert(varPt->type_node != nullptr);

    verify_and_write("ast_test_var_record.txt", varPt.get(),
        "VariableDeclaration(pt)\n"
        "└── Type: IdentifierType(Point)");
}

// --- UNIT TEST 12: Member Access (Field & Element & Size) ---
// Checks:
// pt.x
// arr[i]
// arr.size
void test_member_access() {
    Location loc{12, 1};
    
    // pt.x
    auto ptId = std::make_unique<Identifier>("pt", loc);
    auto fieldAccess = std::make_unique<FieldAccessExpr>(std::move(ptId), "x", loc);
    
    assert(fieldAccess != nullptr);
    assert(fieldAccess->field_name == "x");
    assert(fieldAccess->object != nullptr);

    verify_and_write("ast_test_field_access.txt", fieldAccess.get(),
        "FieldAccess(x)\n"
        "└── Object: Identifier(pt)");

    // arr[i]
    auto arrId = std::make_unique<Identifier>("arr", loc);
    auto idxId = std::make_unique<Identifier>("i", loc);
    auto elemAccess = std::make_unique<ElementAccessExpr>(std::move(arrId), std::move(idxId), loc);
    
    assert(elemAccess != nullptr);
    assert(elemAccess->array != nullptr);
    assert(elemAccess->index != nullptr);

    verify_and_write("ast_test_element_access.txt", elemAccess.get(),
        "ElementAccess\n"
        "├── Array: Identifier(arr)\n"
        "└── Index: Identifier(i)");

    // arr.size (P04)
    auto arrForSize = std::make_unique<Identifier>("arr", loc);
    auto sizeAccess = std::make_unique<FieldAccessExpr>(std::move(arrForSize), "size", loc);
    
    assert(sizeAccess != nullptr);
    assert(sizeAccess->field_name == "size");

    verify_and_write("ast_test_size_access.txt", sizeAccess.get(),
        "FieldAccess(size)\n"
        "└── Object: Identifier(arr)");
}

// --- UNIT TEST 13: Chained & Nested Member Access ---
// Checks:
// matrix[i][j]
// circle.center.x
void test_chained_access() {
    Location loc{13, 1};
    
    // matrix[i][j]
    auto matrixId = std::make_unique<Identifier>("matrix", loc);
    auto rowId = std::make_unique<Identifier>("i", loc);
    auto rowAccess = std::make_unique<ElementAccessExpr>(std::move(matrixId), std::move(rowId), loc);
    auto colId = std::make_unique<Identifier>("j", loc);
    auto cellAccess = std::make_unique<ElementAccessExpr>(std::move(rowAccess), std::move(colId), loc);
    
    assert(cellAccess != nullptr);
    assert(dynamic_cast<ElementAccessExpr*>(cellAccess->array.get()) != nullptr);

    verify_and_write("ast_test_chained_index.txt", cellAccess.get(),
        "ElementAccess\n"
        "├── Array: ElementAccess\n"
        "│   ├── Array: Identifier(matrix)\n"
        "│   └── Index: Identifier(i)\n"
        "└── Index: Identifier(j)");

    // circle.center.x
    auto circleId = std::make_unique<Identifier>("circle", loc);
    auto centerAccess = std::make_unique<FieldAccessExpr>(std::move(circleId), "center", loc);
    auto xAccess = std::make_unique<FieldAccessExpr>(std::move(centerAccess), "x", loc);
    
    assert(xAccess != nullptr);
    assert(xAccess->field_name == "x");
    assert(dynamic_cast<FieldAccessExpr*>(xAccess->object.get()) != nullptr);

    verify_and_write("ast_test_chained_field.txt", xAccess.get(),
        "FieldAccess(x)\n"
        "└── Object: FieldAccess(center)\n"
        "    └── Object: Identifier(circle)");
}

// --- UNIT TEST 14: Assignments to ModifiablePrimary ---
// Checks:
// arr[i] := 42
// pt.x := 10
// matrix[i][j] := 0
void test_assignments_to_modifiable_primary() {
    Location loc{14, 1};
    
    // arr[i] := 42
    auto arrId = std::make_unique<Identifier>("arr", loc);
    auto idxId = std::make_unique<Identifier>("i", loc);
    auto elemAccess = std::make_unique<ElementAccessExpr>(std::move(arrId), std::move(idxId), loc);
    auto val42 = std::make_unique<IntLiteral>(42, loc);
    auto assignArr = std::make_unique<AssignStatement>(std::move(elemAccess), std::move(val42), loc);
    
    assert(assignArr != nullptr);
    assert(dynamic_cast<ElementAccessExpr*>(assignArr->target.get()) != nullptr);
    assert(dynamic_cast<IntLiteral*>(assignArr->value.get()) != nullptr);

    verify_and_write("ast_test_assign_element.txt", assignArr.get(),
        "AssignStatement\n"
        "├── Target: ElementAccess\n"
        "│   ├── Array: Identifier(arr)\n"
        "│   └── Index: Identifier(i)\n"
        "└── Value: IntLiteral(42)");

    // pt.x := 10
    auto ptId = std::make_unique<Identifier>("pt", loc);
    auto fieldAccess = std::make_unique<FieldAccessExpr>(std::move(ptId), "x", loc);
    auto val10 = std::make_unique<IntLiteral>(10, loc);
    auto assignField = std::make_unique<AssignStatement>(std::move(fieldAccess), std::move(val10), loc);
    
    assert(assignField != nullptr);
    assert(dynamic_cast<FieldAccessExpr*>(assignField->target.get()) != nullptr);

    verify_and_write("ast_test_assign_field.txt", assignField.get(),
        "AssignStatement\n"
        "├── Target: FieldAccess(x)\n"
        "│   └── Object: Identifier(pt)\n"
        "└── Value: IntLiteral(10)");

    // matrix[i][j] := 0
    auto matId = std::make_unique<Identifier>("matrix", loc);
    auto iId = std::make_unique<Identifier>("i", loc);
    auto row = std::make_unique<ElementAccessExpr>(std::move(matId), std::move(iId), loc);
    auto jId = std::make_unique<Identifier>("j", loc);
    auto cell = std::make_unique<ElementAccessExpr>(std::move(row), std::move(jId), loc);
    auto val0 = std::make_unique<IntLiteral>(0, loc);
    auto assignMat = std::make_unique<AssignStatement>(std::move(cell), std::move(val0), loc);
    
    assert(assignMat != nullptr);
    assert(dynamic_cast<ElementAccessExpr*>(assignMat->target.get()) != nullptr);

    verify_and_write("ast_test_assign_matrix.txt", assignMat.get(),
        "AssignStatement\n"
        "├── Target: ElementAccess\n"
        "│   ├── Array: ElementAccess\n"
        "│   │   ├── Array: Identifier(matrix)\n"
        "│   │   └── Index: Identifier(i)\n"
        "│   └── Index: Identifier(j)\n"
        "└── Value: IntLiteral(0)");
}

// --- UNIT TEST 15: Routine Declaration - Forward Declaration ---
// Checks: routine gcd(a: integer, b: integer) : integer
void test_routine_forward_declaration() {
    Location loc{15, 1};
    std::vector<std::string> params = {"a : integer", "b : integer"};
    
    auto fwd = std::make_unique<RoutineDeclaration>("gcd", std::move(params), "integer", loc);
    assert(fwd != nullptr);
    assert(fwd->name == "gcd");
    assert(fwd->body_type == RoutineBodyType::Forward);
    assert(fwd->body.empty());
    assert(fwd->expression_body == nullptr);
    
    verify_and_write("ast_test_routine_forward.txt", fwd.get(),
        "RoutineDeclaration(gcd) params: [a : integer, b : integer] returns: integer (forward)");
}

// --- UNIT TEST 16: Routine Declaration - Arrow Body (=> Expression) ---
// Checks: routine square(x: integer) : integer => x * x
void test_routine_arrow_body() {
    Location loc{16, 1};
    std::vector<std::string> params = {"x : integer"};
    
    auto varX1 = std::make_unique<Identifier>("x", loc);
    auto varX2 = std::make_unique<Identifier>("x", loc);
    auto mul = std::make_unique<BinaryExpr>(BinOp::Mul, std::move(varX1), std::move(varX2), loc);
    
    auto arrowRoutine = std::make_unique<RoutineDeclaration>("square", std::move(params), "integer", std::move(mul), loc);
    assert(arrowRoutine != nullptr);
    assert(arrowRoutine->name == "square");
    assert(arrowRoutine->body_type == RoutineBodyType::Expression);
    assert(arrowRoutine->expression_body != nullptr);
    
    verify_and_write("ast_test_routine_arrow.txt", arrowRoutine.get(),
        "RoutineDeclaration(square) params: [x : integer] returns: integer\n"
        "└── ExprBody: BinaryExpr(*)\n"
        "    ├── Left: Identifier(x)\n"
        "    └── Right: Identifier(x)");
}

// --- UNIT TEST 17: Routine Declaration - Empty Body ---
// Checks: routine noop() is end
void test_routine_empty_body() {
    Location loc{17, 1};
    std::vector<std::string> params;
    std::vector<std::unique_ptr<AstNode>> emptyBody;
    
    auto emptyRoutine = std::make_unique<RoutineDeclaration>("noop", std::move(params), "", std::move(emptyBody), loc);
    assert(emptyRoutine != nullptr);
    assert(emptyRoutine->name == "noop");
    assert(emptyRoutine->body_type == RoutineBodyType::Block);
    assert(emptyRoutine->body.empty());
    
    verify_and_write("ast_test_routine_empty.txt", emptyRoutine.get(),
        "RoutineDeclaration(noop)");
}

int main() {
    std::cout << "Running Canonical AST Unit Tests with assert()...\n";
    
    test_math_expression();
    test_if_statement();
    test_routine_declaration();
    test_variable_declaration();
    test_while_loop();
    test_for_loop();
    test_routine_call();
    
    test_type_aliases();
    test_array_types();
    test_record_types();
    test_variables_with_type_node();
    test_member_access();
    test_chained_access();
    test_assignments_to_modifiable_primary();
    test_routine_forward_declaration();
    test_routine_arrow_body();
    test_routine_empty_body();
    
    std::cout << "All 17 canonical unit tests passed successfully!\n";
    return 0;
}
