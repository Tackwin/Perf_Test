# CMakeList.txt : CMake project for Perf_Test, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_executable (Perf_Test "Perf_Test.cpp")

find_package(benchmark CONFIG REQUIRED)
target_link_libraries(Perf_Test PRIVATE benchmark::benchmark benchmark::benchmark_main)
# Add source to this project's executable.

# TODO: Add tests and install targets if needed.
