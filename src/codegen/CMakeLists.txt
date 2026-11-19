add_library(wasm_codegen STATIC
        WasmStorage.cpp
        WasmAggregates.cpp
        WasmStorage.hpp
        WasmCodegen.hpp
        WatWriter.cpp
        WatWriter.hpp
)
target_include_directories(wasm_codegen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wasm_codegen PUBLIC common PRIVATE semantics)

if(MSVC)
    target_compile_options(wasm_codegen PRIVATE /EHsc)
endif()

add_subdirectory(tests)
