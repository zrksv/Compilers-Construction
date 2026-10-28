add_imp_test(TARGET test_app NAME AppTest SOURCES test_app.cpp)
target_compile_definitions(test_app PRIVATE IMP_COMPILER_PATH="$<TARGET_FILE:fair_compiler>")
add_dependencies(test_app fair_compiler)
