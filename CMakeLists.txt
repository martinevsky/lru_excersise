cmake_minimum_required (VERSION 3.6)
project (lru)

find_package  (Boost REQUIRED COMPONENTS system)

link_libraries (${Boost_LIBRARIES})
include_directories (${Boost_INCLUDE_DIRS}) 

add_subdirectory (./external/benchmark)

include_directories (./external/benchmark/include) 
link_libraries (benchmark)

add_executable (lrut main.cpp)
