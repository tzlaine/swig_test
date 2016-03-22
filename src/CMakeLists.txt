########################################
# Plugin
########################################
if (APPLE)
    # TODO: Make a universal (32- & 64-bit) bundle.

    set(
        model_plugin_path
        ${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins
    )

    # TODO: Add install step.
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
        ${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins/${arch_dir}
    )

    install(TARGETS model-plugin DESTINATION ${model_plugin_path})
endif ()
