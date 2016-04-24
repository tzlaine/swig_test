########################################
# Model library
########################################
set(
    model_plugin_path
    ${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins
)

generate_protobuf(model model_message_target)

if (NOT MSVC)
    add_definitions(-Wall)
endif ()

add_definitions(-DBUILD_FOR_TEST)

add_library(
    model
    STATIC
    ${unreal_src_dir}/model.cpp
    ${unreal_src_dir}/json2pb.cc
    ${unreal_src_dir}/model_data.cpp
    ${unreal_src_dir}/model.pb.cc
)

target_link_libraries(
    model
    libprotobuf
    jansson
)

add_dependencies(model model_message_target)
if (PYTHON_EXECUTABLE)
    add_dependencies(model model_data_target)
endif ()
