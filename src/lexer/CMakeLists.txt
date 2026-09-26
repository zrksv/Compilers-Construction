add_library(lexer STATIC Lexer.cpp Token.cpp)

target_link_libraries(lexer PRIVATE common)

add_subdirectory(tests)