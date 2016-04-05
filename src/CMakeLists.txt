########################################
# Plugin
########################################
set(
    model_plugin_path
    ${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins
)

generate_protobuf(model model_message_target)

if (APPLE)
    add_definitions(-Wall)

    add_library(
        model_plugin
        MODULE
        model.cpp
        ${CMAKE_HOME_DIRECTORY}/json2pb/json2pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/model_data.cpp
        ${CMAKE_BINARY_DIR}/model.pb.cc
    )

    set_target_properties(
        model_plugin
        PROPERTIES
            BUNDLE true
    )

    install(
        DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/model_plugin.bundle
        DESTINATION
            ${model_plugin_path}
        USE_SOURCE_PERMISSIONS
    )
else ()
    add_definitions(-DBUILD_LIBRARY)

    if (NOT MSVC)
        add_definitions(-Wall)
    endif ()

    add_library(
        model_plugin
        SHARED
        model.cpp
        ${CMAKE_HOME_DIRECTORY}/json2pb/json2pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/model_data.cpp
        ${CMAKE_BINARY_DIR}/model.pb.cc
    )

    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(arch_dir x86)
    else ()
        set(arch_dir x86_64)
    endif ()

    set(
        model_plugin_path
        ${model_plugin_path}/${arch_dir}
    )

    install(TARGETS model_plugin DESTINATION ${model_plugin_path}) 
endif ()

target_link_libraries(
    model_plugin
    libprotobuf
    jansson
)

add_dependencies(model_plugin model_message_target)
if (PYTHON_EXECUTABLE)
    add_dependencies(model_plugin model_data_target)
endif ()
