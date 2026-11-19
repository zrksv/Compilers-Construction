add_library(test_utils STATIC TestUtils.cpp)
target_include_directories(test_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(test_utils PRIVATE IMP_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

add_imp_test(TARGET test_pipeline NAME PipelineTest SOURCES test_pipeline.cpp LIBS parser semantics optimizer test_utils)

find_program(WAT2WASM_EXECUTABLE NAMES wat2wasm)
find_program(WASM_VALIDATE_EXECUTABLE NAMES wasm-validate)
find_program(NODE_EXECUTABLE NAMES node)
if(WAT2WASM_EXECUTABLE AND WASM_VALIDATE_EXECUTABLE AND NODE_EXECUTABLE)
    add_imp_test(TARGET test_wasm_execution NAME WasmExecutionTest SOURCES test_wasm_execution.cpp
            LIBS parser semantics optimizer wasm_codegen wasm_execution)
    target_compile_definitions(test_wasm_execution PRIVATE
            IMP_WASM_SAMPLE="${CMAKE_CURRENT_SOURCE_DIR}/wasm_samples/end_to_end.imp")
    add_test(NAME WasmExecutionNoOptTest COMMAND test_wasm_execution --no-opt)
    set_tests_properties(WasmExecutionTest WasmExecutionNoOptTest PROPERTIES TIMEOUT 60)
else()
    message(STATUS "WasmExecution tests unavailable: require wat2wasm, wasm-validate (WABT), and Node.js")
endif()
