cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8.9)

add_subdirectory(googletest-1.7.0)

set(boost_dir ${CMAKE_HOME_DIRECTORY}/"boost_1_60")
include_directories(${boost_dir})
add_definitions(-DBOOST_NO_EXCEPTIONS)
if (NOT WIN32)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
endif ()

########################################
# Plugin
########################################
add_library(
    model-plugin
    SHARED
    model.cpp
)

#set_target_properties(
#    model-plugin
#    PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
#    LIBARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
#    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
#)

########################################
# Tests
########################################
add_executable(
    model-tests
    model.cpp
)

set_target_properties(
    model-tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}
)

target_link_libraries(
    model-tests
    gtest
    gtest_main
)
