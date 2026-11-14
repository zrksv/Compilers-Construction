add_library(wasm_codegen STATIC
        WasmCodegen.cpp
        WasmCodegen.hpp
)
target_include_directories(wasm_codegen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wasm_codegen PUBLIC common PRIVATE semantics)

add_subdirectory(tests)