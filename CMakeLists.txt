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
# Tests
########################################
add_executable(
    model-tests
    model.cpp
)

target_link_libraries(
    model-tests
    gtest
    gtest_main
)


########################################
# Plugin
########################################
if (APPLE)
    # TODO: Make a universal (32- & 64-bit) bundle.

    set(
        model_plugin_path
        ${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins
    )

    # TODO: Add install step.
else ()
    add_definitions(-DBUILD_LIBRARY)

    add_library(
        model-plugin
        SHARED
        model.cpp
    )

    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(arch_dir x86)
    else ()
        set(arch_dir x86_64)
    endif ()

    set(
        model_plugin_path
        ${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins/${arch_dir}
    )

    install(TARGETS model-plugin DESTINATION ${model_plugin_path})
endif ()
