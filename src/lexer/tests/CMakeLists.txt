add_executable(test_lexer test_lexer.cpp ../../../tests/TestUtils.cpp)
target_link_libraries(test_lexer PRIVATE lexer common)