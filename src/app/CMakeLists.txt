add_executable(fair_compiler main.cpp
        WasmExecution.cpp
        WasmExecution.hpp)
target_link_libraries(fair_compiler PRIVATE parser semantics)

if(MSVC)
    target_compile_options(fair_compiler PRIVATE /EHsc)
endif()

add_subdirectory(tests)
