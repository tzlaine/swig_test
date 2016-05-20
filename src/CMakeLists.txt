########################################
# start_data library
########################################
generate_protobuf(base_types base_types_message_target)
generate_protobuf(start_data start_data_message_target)
generate_protobuf(game_data game_data_message_target)
generate_protobuf(visual_config visual_config_message_target)

########################################
# start_data
########################################
add_library(
    start_data
    STATIC
    ${unreal_src_dir}/start_data_t.cpp
    ${unreal_src_dir}/json2pb.cc
    ${unreal_src_dir}/base_types.cpp
    ${unreal_src_dir}/start_data.cpp
    ${unreal_src_dir}/visual_config.cpp
    ${unreal_src_dir}/base_types.pb.cc
    ${unreal_src_dir}/start_data.pb.cc
    ${unreal_src_dir}/visual_config.pb.cc
)

target_link_libraries(
    start_data
    libprotobuf
    jansson
)

add_dependencies(start_data base_types_message_target start_data_message_target visual_config_message_target)
if (PYTHON_EXECUTABLE)
    add_dependencies(start_data base_types_target start_data_target visual_config_target)
endif ()


########################################
# game_data
########################################
add_library(
    game_data
    STATIC
    ${unreal_src_dir}/game_data_t.cpp
    ${unreal_src_dir}/json2pb.cc
    ${unreal_src_dir}/base_types.cpp
    ${unreal_src_dir}/game_data.cpp
    ${unreal_src_dir}/base_types.pb.cc
    ${unreal_src_dir}/game_data.pb.cc
)

target_link_libraries(
    game_data
    libprotobuf
    jansson
)

add_dependencies(game_data base_types_message_target game_data_message_target)
if (PYTHON_EXECUTABLE)
    add_dependencies(game_data base_types_target game_data_target)
endif ()


########################################
# graph
########################################
add_library(
    graph
    STATIC
    ${unreal_src_dir}/graph.cpp
    ${unreal_src_dir}/supply.cpp
    ${unreal_src_dir}/base_types.cpp
    ${unreal_src_dir}/base_types.pb.cc
)

target_link_libraries(
    graph
)

if (false)
add_dependencies(graph base_types_message_target graph_message_target)
if (PYTHON_EXECUTABLE)
    add_dependencies(graph base_types_target graph_target)
endif ()
endif ()


########################################
# visual_config
########################################
add_library(
    visual_config
    STATIC
    ${unreal_src_dir}/visual_config_t.cpp
    ${unreal_src_dir}/json2pb.cc
    ${unreal_src_dir}/visual_config.cpp
    ${unreal_src_dir}/visual_config.pb.cc
)

target_link_libraries(
    visual_config
    libprotobuf
    jansson
)

add_dependencies(visual_config base_types_message_target visual_config_message_target)
if (PYTHON_EXECUTABLE)
    add_dependencies(visual_config base_types_target visual_config_target)
endif ()


########################################
# adobe_name
########################################
add_library(
    adobe_name
    STATIC
    ${unreal_src_dir}/adobe/src/name.cpp
    ${unreal_src_dir}/adobe/src/string_pool.cpp
)


########################################
# Boost.System
########################################
add_library(
    boost_system
    STATIC
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/system/src/error_code.cpp
)


########################################
# Boost.Filesystem
########################################
add_library(
    boost_filesystem
    STATIC
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/filesystem/src/codecvt_error_category.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/filesystem/src/operations.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/filesystem/src/path.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/filesystem/src/path_traits.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/filesystem/src/portability.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/filesystem/src/unique_path.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/filesystem/src/utf8_codecvt_facet.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/filesystem/src/windows_file_codecvt.cpp
)


########################################
# Boost.Thread
########################################
if (WIN32)
    set(boost_thread_sources
        ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/thread/src/win32/thread.cpp
        ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/thread/src/win32/tss_dll.cpp
        ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/thread/src/win32/tss_pe.cpp
        ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/thread/src/future.cpp
    )
else ()
    set(boost_thread_sources
        ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/thread/src/pthread/thread.cpp
        ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/thread/src/pthread/once.cpp
        ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/thread/src/future.cpp
    )
endif ()

add_library(
    boost_thread
    STATIC
    ${boost_thread_sources}
)


install(
    TARGETS boost_system boost_filesystem boost_thread
    DESTINATION ${unreal_3rd_party_root}/Boost/Libraries
)


# These last two are only used for the adobe_source_libraries' tests.

########################################
# Boost.ProgramOptions
########################################
add_library(
    boost_program_options
    STATIC
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/cmdline.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/config_file.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/options_description.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/parsers.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/variables_map.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/value_semantic.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/positional_options.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/utf8_codecvt_facet.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/convert.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/winmain.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/program_options/src/split.cpp
)


########################################
# Boost.Test
########################################
add_library(
    boost_unit_test_framework
    STATIC
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/compiler_log_formatter.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/debug.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/decorator.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/execution_monitor.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/framework.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/plain_report_formatter.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/progress_monitor.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/results_collector.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/results_reporter.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/test_tools.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/test_tree.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/unit_test_log.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/unit_test_main.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/unit_test_monitor.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/unit_test_parameters.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/xml_log_formatter.cpp
    ${CMAKE_SOURCE_DIR}/boost_1_60_0/libs/test/src/xml_report_formatter.cpp
)
