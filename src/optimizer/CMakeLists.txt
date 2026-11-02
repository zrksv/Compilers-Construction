add_library(optimizer STATIC
    OptimizerUtils.hpp
    OptimizerUtils.cpp
    ConstantFolding.hpp
    ConstantFolding.cpp
    BranchPruning.hpp
    BranchPruning.cpp
    DeadCode.hpp
    DeadCode.cpp
    UnusedVariables.hpp
    UnusedVariables.cpp
    AstOptimizer.hpp
    AstOptimizer.cpp
)

target_include_directories(optimizer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(optimizer PUBLIC ast semantics common)

if(MSVC)
    target_compile_options(optimizer PRIVATE /EHsc /Zc:__cplusplus)
endif()

add_subdirectory(tests)
