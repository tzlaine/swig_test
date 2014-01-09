cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8.9)

set(boost_dir "/usr/include" CACHE PATH "Specifies the path to the headers for Boost).")

include_directories(${boost_dir})
add_definitions(-DBUILD_LIBRARY -DBOOST_NO_EXCEPTIONS)

if (NOT WIN32)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++11)
endif ()

add_library(
    graph_algorithms
    SHARED
    model.cpp
)

set_target_properties(
    graph_algorithms
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
)
