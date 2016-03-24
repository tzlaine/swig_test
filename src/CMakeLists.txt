########################################
# Plugin
########################################
set(
    model_plugin_path
    ${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins
)

add_definitions(-DBOOST_NO_EXCEPTIONS)

generate_protobuf(model model_message_target)

if (APPLE)
    # TODO: Make a universal (32- & 64-bit) bundle.

    add_definitions(-Wall)

    add_library(
        model-plugin
        MODULE
        model.cpp
        ${CMAKE_BINARY_DIR}/model.pb.cc
    )
else ()
    add_definitions(-DBUILD_LIBRARY -Wall)

    add_library(
        model-plugin
        SHARED
        model.cpp
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
endif ()

target_link_libraries(
    model-plugin
    libprotobuf
)

add_dependencies(model-plugin model_message_target)

install(TARGETS model-plugin DESTINATION ${model_plugin_path})
