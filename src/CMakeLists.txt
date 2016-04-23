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

add_library(
    model
    STATIC
    model.cpp
    ${CMAKE_HOME_DIRECTORY}/json2pb/json2pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/model_data.cpp
    ${CMAKE_BINARY_DIR}/model.pb.cc
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
