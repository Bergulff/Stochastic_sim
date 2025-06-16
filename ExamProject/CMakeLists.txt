#[[
cmake_minimum_required(VERSION 3.31)
project(ExamProject)

set(CMAKE_CXX_STANDARD 23)

# Include all headers
include_directories(include)

# Define the core simulation library (no main() files here!)
add_library(ExamProject STATIC
        src/species.cpp
        src/reaction.cpp
        src/vessel.cpp
        src/simulator.cpp
        src/visualization.cpp
        src/observer.cpp

        examples/covid_seihr.cpp         # ✅ library only
        examples/circadian_rythm.cpp     # ✅ library only
)

# Link public headers for CLion indexing
target_include_directories(ExamProject PUBLIC include)

# === Examples with main() ===
#[=[
add_executable(seir examples/seihr_main.cpp)
target_link_libraries(seir ExamProject)

add_executable(circadian examples/circadian_main.cpp)
target_link_libraries(circadian ExamProject)
]=]

add_executable(main examples/main.cpp)
target_link_libraries(main ExamProject)


# === Benchmark ===
add_executable(benchmark benchmarks/benchmark.cpp)
target_link_libraries(benchmark ExamProject)
]]
cmake_minimum_required(VERSION 3.16)
project(StochasticSimulation)

# R1, R4 - Set C++ standard and compilation options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
endif()

# Include directories
include_directories(include)

# R3, R4, R6, R7, R8 - Library source files
set(LIBRARY_SOURCES
        src/species.cpp
        src/reaction.cpp
        src/vessel.cpp
        src/simulator.cpp
        src/observer.cpp
        src/visualization.cpp
)

# Create static library
add_library(stochastic_lib STATIC ${LIBRARY_SOURCES})

# R5 - Example source files
set(EXAMPLE_SOURCES
        examples/circadian_rythm.cpp
        examples/covid_seihr.cpp
)

# Main executable
add_executable(stochastic_main
        examples/main.cpp
        ${EXAMPLE_SOURCES}
)

# Link library to main executable
target_link_libraries(stochastic_main stochastic_lib)

# Individual example executables
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

# R10 - Benchmark executable (using your updated benchmark.cpp)
add_executable(benchmark
        benchmarks/benchmark.cpp
        ${EXAMPLE_SOURCES}
)
target_link_libraries(benchmark stochastic_lib)

# R9 - Unit tests (if you create test files)
# Uncomment when you add unit tests
# add_executable(unit_tests tests/test_main.cpp tests/test_symbol_table.cpp)
# target_link_libraries(unit_tests stochastic_lib)

# Threading support for R8
find_package(Threads REQUIRED)
target_link_libraries(stochastic_main Threads::Threads)
target_link_libraries(circadian_example Threads::Threads)
target_link_libraries(seihr_example Threads::Threads)
target_link_libraries(benchmark Threads::Threads)

# Create output directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/output)