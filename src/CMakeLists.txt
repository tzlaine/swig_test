########################################
# start_data library
########################################
generate_protobuf(base_types base_types_message_target)
generate_protobuf(game_data game_data_message_target)

########################################
# game_data
########################################
add_library(
    game_data
    STATIC
    ${unreal_src_dir}/model.cpp
    ${unreal_src_dir}/json2pb.cc
    ${unreal_src_dir}/base_types.cpp
    ${unreal_src_dir}/game_data.cpp
    ${unreal_src_dir}/generate_galaxy.cpp
    ${unreal_src_dir}/base_types.pb.cc
    ${unreal_src_dir}/game_data.pb.cc
)

target_link_libraries(
    game_data
    libprotobuf
    jansson
)

add_dependencies(game_data base_types_message_target game_data_message_target)
if (Python3_EXECUTABLE)
    add_dependencies(game_data base_types_target game_data_target)
endif ()


if (false)
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

add_dependencies(graph base_types_message_target graph_message_target)
if (Python3_EXECUTABLE)
    add_dependencies(graph base_types_target graph_target)
endif ()
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
