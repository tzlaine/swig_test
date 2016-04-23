########################################
# Tests
########################################
include(CTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -VV -C ${CMAKE_CFG_INTDIR})

generate_protobuf(model model_test_message_target)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_HOME_DIRECTORY}/googletest/googletest/include
)

add_executable(model_tests model_tests.cpp)

target_link_libraries(
    model_tests
    model
    libprotobuf
    jansson
    gtest
    gtest_main
)

set_target_properties(
    model_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(model_tests data_header_target model_test_message_target)

set(model_test_args)
if (MSVC)
    set(model_test_args
	${CMAKE_HOME_DIRECTORY}/nations.json
	${CMAKE_HOME_DIRECTORY}/default_map.json
	${CMAKE_HOME_DIRECTORY}/default_oob.json
	${CMAKE_HOME_DIRECTORY}/units.json
	${CMAKE_HOME_DIRECTORY}/scenarios/the_wind.json
    )
endif ()
add_test(model_tests ${CMAKE_BINARY_DIR}/model_tests ${model_test_args} --gtest_catch_exceptions=0)
