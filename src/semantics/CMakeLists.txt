add_library(semantics STATIC
    Type.cpp
    Scope.cpp
    DeclarationAnalyzer.cpp
)

target_include_directories(semantics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(semantics PUBLIC common ast)

if(MSVC)
    target_compile_options(semantics PRIVATE /EHsc /Zc:__cplusplus)
endif()

add_subdirectory(tests)
