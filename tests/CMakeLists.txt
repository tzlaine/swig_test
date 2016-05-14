########################################
# Tests
########################################
include(CTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR})

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
    adobe_name
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

add_dependencies(start_data_tests data_header_target)

set(start_data_test_args)
if (MSVC)
    set(start_data_test_args
	${unreal_root}/nations.json
	${unreal_root}/default_map.json
	${unreal_root}/default_oob.json
	${unreal_root}/units.json
	${unreal_root}/scenarios/the_wind.json
	${unreal_root}/hex_map_config.json
    )
endif ()
add_test(start_data_tests ${CMAKE_BINARY_DIR}/start_data_tests ${start_data_test_args} --gtest_catch_exceptions=1)

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

add_dependencies(graph_tests data_header_target)

set(graph_test_args)
if (MSVC)
    set(graph_test_args
	${unreal_root}/nations.json
	${unreal_root}/default_map.json
	${unreal_root}/default_oob.json
	${unreal_root}/units.json
	${unreal_root}/scenarios/the_wind.json
	${unreal_root}/hex_map_config.json
    )
endif ()
add_test(graph_tests ${CMAKE_BINARY_DIR}/graph_tests ${graph_test_args} --gtest_catch_exceptions=1)
