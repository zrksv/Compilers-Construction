add_library(test_utils STATIC TestUtils.cpp)
target_include_directories(test_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(test_utils PRIVATE IMP_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

add_imp_test(TARGET test_pipeline NAME PipelineTest SOURCES test_pipeline.cpp LIBS parser test_utils)
