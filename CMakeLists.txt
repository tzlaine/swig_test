cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8.9)

set(boost_dir "/Users/tzlaine/boost_1_53_0" CACHE PATH "Specifies the path to the headers for Boost).")

include_directories(${boost_dir})
add_definitions(-DBOOST_NO_EXCEPTIONS -O0 -g)

if (NOT WIN32)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++11)
endif ()

add_executable(
    model
    model.cpp
)

set_target_properties(
    model
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
)
