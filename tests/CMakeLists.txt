########################################
# Tests
########################################
include(CTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR})

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_HOME_DIRECTORY}/googletest/googletest/include
)

macro (add_executable_and_test target)
    add_executable(${target} ${target}.cpp ${ARGN})
    target_link_libraries(
        ${target}
        game_data
        adobe_name
        jansson
        gtest
        gtest_main
    )
    set_target_properties(
        ${target}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_test(${target} ${CMAKE_BINARY_DIR}/${target} --gtest_catch_exceptions=1)
endmacro()

add_executable_and_test(flags_tests)
add_executable_and_test(formatter_tests)
add_executable_and_test(generation_tests)
add_executable_and_test(model_tests)
add_executable_and_test(serialize_impl_count_tests)
add_executable_and_test(serialize_impl_round_trip_tests)
add_executable_and_test(
    dir_watcher_tests
    ${CMAKE_SOURCE_DIR}/unreal_project/Spaceport/Source/Spaceport/dir_watcher.cpp)

if (false)
add_executable_and_test(graph_tests)
endif()
