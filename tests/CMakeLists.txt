########################################
# Tests
########################################
include(CTest)

enable_testing()

add_custom_target(
    check
    COMMAND ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR}
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/adobe_source_libraries/test ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR}
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/adobe_source_libraries/test/spirit2_parser ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR}
)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_HOME_DIRECTORY}/googletest/googletest/include
)

add_executable(start_data_tests start_data_tests.cpp)

target_link_libraries(
    start_data_tests
    start_data
    game_data
    visual_config
    asl
    jansson
    gtest
    gtest_main
)

set_target_properties(
    start_data_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if (PYTHON_EXECUTABLE)
    add_dependencies(start_data_tests data_header_target)
endif ()

add_test(start_data_tests ${CMAKE_BINARY_DIR}/start_data_tests --gtest_catch_exceptions=1)

add_executable(graph_tests graph_tests.cpp)

target_link_libraries(
    graph_tests
    start_data
    game_data
    graph
    visual_config
    asl
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

if (PYTHON_EXECUTABLE)
    add_dependencies(graph_tests data_header_target)
endif ()

add_test(graph_tests ${CMAKE_BINARY_DIR}/graph_tests --gtest_catch_exceptions=1)
