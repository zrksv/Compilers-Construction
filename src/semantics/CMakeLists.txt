add_library(semantics STATIC
        Type.cpp
        Scope.cpp
)

target_include_directories(semantics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(semantics PUBLIC common)