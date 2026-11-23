add_imp_test(TARGET test_codegen NAME CodegenTest SOURCES test_codegen.cpp LIBS wasm_codegen parser semantics)
add_imp_test(TARGET test_codegen_records NAME CodegenRecordTest SOURCES test_records.cpp LIBS wasm_codegen parser semantics optimizer)
add_imp_test(TARGET test_expressions NAME CodegenExpressionsTest SOURCES test_expressions.cpp LIBS wasm_codegen parser semantics)

if(MSVC)
    target_compile_options(test_codegen PRIVATE /EHsc)
    target_compile_options(test_codegen_records PRIVATE /EHsc)
    target_compile_options(test_expressions PRIVATE /EHsc)
endif()

if(WAT2WASM_EXECUTABLE AND NODE_EXECUTABLE)
    add_test(NAME CodegenMemoryRuntimeTest COMMAND "${NODE_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/test_memory_runtime.cjs"
        "$<TARGET_FILE:test_codegen>" "${WAT2WASM_EXECUTABLE}")
    set_tests_properties(CodegenMemoryRuntimeTest PROPERTIES TIMEOUT 30)
    add_test(NAME CodegenRecordRuntimeTest COMMAND "${NODE_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/test_records_runtime.cjs"
        "$<TARGET_FILE:test_codegen_records>" "${WAT2WASM_EXECUTABLE}")
    set_tests_properties(CodegenRecordRuntimeTest PROPERTIES TIMEOUT 30)
else()
    message(STATUS "CodegenMemoryRuntimeTest unavailable: requires wat2wasm (WABT) and Node.js")
endif()
