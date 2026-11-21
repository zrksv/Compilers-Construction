add_library(wasm_execution STATIC WasmExecution.cpp WasmExecution.hpp)
target_include_directories(wasm_execution PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wasm_execution PUBLIC semantics)
target_compile_definitions(wasm_execution PRIVATE
        IMP_WASM_RUNNER_PATH="${PROJECT_SOURCE_DIR}/runtime/runner.mjs")
foreach(tool IN ITEMS WAT2WASM WASM_VALIDATE NODE)
    if(${tool}_EXECUTABLE)
        target_compile_definitions(wasm_execution PRIVATE
                IMP_${tool}_PATH="${${tool}_EXECUTABLE}")
    endif()
endforeach()

add_executable(fair_compiler main.cpp)
target_link_libraries(fair_compiler PRIVATE parser semantics optimizer wasm_codegen wasm_execution)

if(MSVC)
    target_compile_options(fair_compiler PRIVATE /EHsc)
    target_compile_options(wasm_execution PRIVATE /EHsc)
endif()

add_subdirectory(tests)
