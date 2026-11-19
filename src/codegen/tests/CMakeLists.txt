add_imp_test(TARGET test_codegen NAME CodegenTest SOURCES test_codegen.cpp LIBS wasm_codegen parser semantics)

if(MSVC)
    target_compile_options(test_codegen PRIVATE /EHsc)
endif()

find_program(WAT2WASM_EXECUTABLE NAMES wat2wasm)
find_program(NODE_EXECUTABLE NAMES node)
if(WAT2WASM_EXECUTABLE AND NODE_EXECUTABLE)
    add_test(NAME CodegenMemoryRuntimeTest COMMAND "${NODE_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/test_memory_runtime.cjs"
        "$<TARGET_FILE:test_codegen>" "${WAT2WASM_EXECUTABLE}")
    set_tests_properties(CodegenMemoryRuntimeTest PROPERTIES TIMEOUT 30)
else()
    message(STATUS "CodegenMemoryRuntimeTest unavailable: requires wat2wasm (WABT) and Node.js")
endif()
