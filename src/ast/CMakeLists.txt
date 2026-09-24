add_library(ast
    AST.cpp
    ASTPrinter.cpp
)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common)

add_subdirectory(tests)
