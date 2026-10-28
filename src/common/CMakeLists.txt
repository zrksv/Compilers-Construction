add_library(common STATIC SourceManager.cpp)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)
