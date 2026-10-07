add_executable(test_pipeline test_pipeline.cpp TestUtils.cpp)
target_link_libraries(test_pipeline PRIVATE parser lexer ast common)

add_executable(test_app test_app.cpp)