add_executable(test_ast_printer test_ast_printer.cpp)
target_link_libraries(test_ast_printer PRIVATE ast)

add_test(NAME AstPrinterTest COMMAND test_ast_printer)
