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

add_executable(
    model-tests
    ../src/model.cpp
    ${CMAKE_BINARY_DIR}/model.pb.cc
    model_tests.cpp
)

target_link_libraries(
    model-tests
    libprotobuf
    gtest
    gtest_main
)

set_target_properties(
    model-tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(model-tests data_header_target model_test_message_target)

add_test(model_tests ${CMAKE_BINARY_DIR}/model-tests)
