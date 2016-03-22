########################################
# Tests
########################################
add_executable(
    model-tests
    ../src/model.cpp
)

target_link_libraries(
    model-tests
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
