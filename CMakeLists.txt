cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8.10)

set(boost_dir "/usr/include" CACHE PATH "Specifies the path to the headers for Boost).")

include_directories(${boost_dir})
add_definitions(-DBUILD_LIBRARY -DBOOST_NO_EXCEPTIONS)

add_library(
    graph_algorithms
    MODULE
    model.cpp
)
