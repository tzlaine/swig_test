########################################
# Plugin
########################################
set(
    model_plugin_path
    ${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins
)

if (APPLE)
    # TODO: Make a universal (32- & 64-bit) bundle.

    set(
        CMAKE_OSX_ARCHITECTURES
        i386
        x86_64
    )

    add_library(
        model-plugin
        MODULE
        model.cpp
    )
else ()
    add_definitions(-DBUILD_LIBRARY)

    add_library(
        model-plugin
        SHARED
        model.cpp
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

install(TARGETS model-plugin DESTINATION ${model_plugin_path})
