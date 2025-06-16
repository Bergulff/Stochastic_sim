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
#[[
add_executable(seir examples/seihr_main.cpp)
target_link_libraries(seir ExamProject)

add_executable(circadian examples/circadian_main.cpp)
target_link_libraries(circadian ExamProject)
]]

add_executable(main examples/main.cpp)
target_link_libraries(main ExamProject)


# === Benchmark ===
add_executable(benchmark benchmarks/benchmark.cpp)
target_link_libraries(benchmark ExamProject)
