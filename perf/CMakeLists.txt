include_directories(${CMAKE_SOURCE_DIR}/benchmark-1.0.0/include)

add_executable(graph_perf graph_perf.cpp)

target_link_libraries(
    graph_perf
    start_data
    game_data
    graph
    visual_config
    asl
    jansson
    benchmark
)

set_target_properties(
    graph_perf
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if (PYTHON_EXECUTABLE)
    add_dependencies(graph_perf data_header_target)
endif ()
