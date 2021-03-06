cmake_minimum_required (VERSION 3.5)
project (lru)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED on)

set (Boost_DEBUG ON)
set (Boost_DETAILED_FAILURE_MSG ON)
find_package  (Boost REQUIRED COMPONENTS system)

link_libraries (${Boost_LIBRARIES})
include_directories (${Boost_INCLUDE_DIRS}) 

#Benchmaking
set (BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory (./external/benchmark)

include_directories (./external/benchmark/include) 
link_libraries (benchmark)

add_executable (lrub main_benchmark.cpp)

#Testing
set (gtest_force_shared_crt ON)
set (BUILD_GMOCK OFF)
set (BUILD_GTEST ON)
add_subdirectory (./external/googletest)

include_directories (./external/googletest/googletest/include) 
link_libraries (gtest)
link_libraries (gtest_main)
add_executable (lrut main_test.cpp)