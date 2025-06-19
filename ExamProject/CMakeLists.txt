cmake_minimum_required(VERSION 3.16)
project(StochasticSimulation)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
endif()

# Doctest integration
find_package(doctest QUIET)

if(NOT doctest_FOUND)
    include(FetchContent)
    
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.11
    )
    
    FetchContent_MakeAvailable(doctest)
endif()

include_directories(include)

set(LIBRARY_SOURCES
        src/species.cpp
        src/reaction.cpp
        src/vessel.cpp
        src/simulator.cpp
        src/observer.cpp
        src/visualization.cpp
)


add_library(stochastic_lib STATIC ${LIBRARY_SOURCES})

set(EXAMPLE_IMPLEMENTATION_SOURCES
        examples/circadian_rythm.cpp
        examples/covid_seihr.cpp
        examples/catalytic_reaction.cpp
)


add_executable(stochastic_main
        examples/main.cpp
        ${EXAMPLE_IMPLEMENTATION_SOURCES}
)


target_link_libraries(stochastic_main stochastic_lib)


add_executable(circadian_example
        examples/circadian_main.cpp
        examples/circadian_rythm.cpp
)
target_link_libraries(circadian_example stochastic_lib)

add_executable(seihr_example
        examples/seihr_main.cpp
        examples/covid_seihr.cpp
)
target_link_libraries(seihr_example stochastic_lib)


add_executable(simple_catalytic_example
        examples/catalytic_reaction_main.cpp
        examples/catalytic_reaction.cpp
)
target_link_libraries(simple_catalytic_example stochastic_lib)


add_executable(benchmark
        benchmarks/benchmark.cpp
        ${EXAMPLE_IMPLEMENTATION_SOURCES}
)
target_link_libraries(benchmark stochastic_lib)


add_executable(unit_tests
        tests/test.cpp
        ${EXAMPLE_IMPLEMENTATION_SOURCES}
)
target_link_libraries(unit_tests stochastic_lib)


enable_testing()
add_test(NAME StochasticLibraryTests COMMAND unit_tests)


add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS unit_tests
        COMMENT "Running unit tests"
)


find_package(Threads REQUIRED)
target_link_libraries(stochastic_main Threads::Threads)
target_link_libraries(circadian_example Threads::Threads)
target_link_libraries(seihr_example Threads::Threads)
target_link_libraries(simple_catalytic_example Threads::Threads)
target_link_libraries(benchmark Threads::Threads)
target_link_libraries(unit_tests Threads::Threads)


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/output)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")


set_target_properties(stochastic_main circadian_example seihr_example simple_catalytic_example PROPERTIES FOLDER "Examples")
set_target_properties(benchmark PROPERTIES FOLDER "Benchmarks")
set_target_properties(unit_tests PROPERTIES FOLDER "Tests")
set_target_properties(stochastic_lib PROPERTIES FOLDER "Libraries")


add_custom_target(run_all_examples
        COMMAND circadian_example
        COMMAND seihr_example
        COMMAND simple_catalytic_example
        DEPENDS circadian_example seihr_example simple_catalytic_example
        COMMENT "Running all three example systems (R5)"
)


add_custom_target(run_circadian
        COMMAND circadian_example
        DEPENDS circadian_example
        COMMENT "Running circadian rhythm example"
)

add_custom_target(run_covid
        COMMAND seihr_example
        DEPENDS seihr_example
        COMMENT "Running COVID-19 SEIHR example"
)

add_custom_target(run_catalytic
        COMMAND simple_catalytic_example
        DEPENDS simple_catalytic_example
        COMMENT "Running simple catalytic reaction example"
)
