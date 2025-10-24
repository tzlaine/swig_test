########################################
# Tests
########################################
include(CTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR})

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_HOME_DIRECTORY}/googletest/googletest/include
)

add_executable(generation_tests generation_tests.cpp)

target_link_libraries(
    generation_tests
    game_data
    adobe_name
    jansson
    gtest
    gtest_main
)

set_target_properties(
    generation_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_test(generation_tests ${CMAKE_BINARY_DIR}/generation_tests --gtest_catch_exceptions=1)

add_executable(formatter_tests formatter_tests.cpp)

target_link_libraries(
    formatter_tests
    game_data
    adobe_name
    jansson
    gtest
    gtest_main
)

set_target_properties(
    formatter_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_test(formatter_tests ${CMAKE_BINARY_DIR}/formatter_tests --gtest_catch_exceptions=1)

if (false)
add_executable(graph_tests graph_tests.cpp)

target_link_libraries(
    graph_tests
    start_data
    game_data
    graph
    visual_config
    adobe_name
    jansson
    gtest
    gtest_main
)

set_target_properties(
    graph_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if (Python3_EXECUTABLE)
    add_dependencies(graph_tests data_header_target)
endif ()

add_test(graph_tests ${CMAKE_BINARY_DIR}/graph_tests --gtest_catch_exceptions=1)
endif()
