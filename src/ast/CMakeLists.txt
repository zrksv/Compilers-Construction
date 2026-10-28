add_library(ast STATIC
    AST.cpp
    ASTPrinter.cpp
)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ast PUBLIC common)

add_subdirectory(tests)
