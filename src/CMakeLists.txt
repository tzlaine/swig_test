########################################
# start_data library
########################################
generate_protobuf(base_types base_types_message_target)
generate_protobuf(start_data start_data_message_target)
generate_protobuf(game_data game_data_message_target)
generate_protobuf(visual_config visual_config_message_target)

add_definitions(-DBUILD_FOR_TEST)

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
# visual_config
########################################
add_library(
    adobe_name
    STATIC
    ${unreal_src_dir}/adobe/src/name.cpp
    ${unreal_src_dir}/adobe/src/string_pool.cpp
)
