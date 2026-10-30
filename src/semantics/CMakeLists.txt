add_library(semantics STATIC
        types/Type.hpp
        types/Type.cpp
        types/TypeFactory.hpp
        types/TypeCompatibility.hpp
)

target_include_directories(semantics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(semantics PUBLIC common ast)

if(MSVC)
    target_compile_options(semantics PRIVATE /EHsc /Zc:__cplusplus)
endif()

add_subdirectory(tests)
