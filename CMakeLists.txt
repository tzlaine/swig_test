cmake_minimum_required(VERSION 3.0)

set(use_asan false)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES x86_64)

    if (${use_asan})
        add_definitions(-fsanitize=address)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif ()
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif ()

add_subdirectory(googletest)
add_subdirectory(benchmark-1.0.0)

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif ()

if (NOT MSVC)
    add_definitions(-Wall -Wno-parentheses)
endif ()

set(unreal_root ${CMAKE_HOME_DIRECTORY}/unreal_project/Spaceport)
set(unreal_src_dir ${unreal_root}/Source/Spaceport)
set(unreal_3rd_party_root ${unreal_root}/ThirdParty)

add_definitions(-DGOOGLE_PROTOBUF_NO_RTTI)
set(protobuf_dir protobuf-v3.0.0-beta-2)
add_subdirectory(${protobuf_dir}/cmake)

set(boost_dir ${CMAKE_HOME_DIRECTORY}/boost_1_60_0)
include_directories(${boost_dir})
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

add_subdirectory(jansson)
include_directories(${CMAKE_BINARY_DIR}/jansson/include)

macro(generate_protobuf file_root target)
    add_custom_command(
        OUTPUT
            ${unreal_src_dir}/${file_root}.pb.cc
            ${unreal_src_dir}/${file_root}.pb.h
        COMMAND
            protoc
                --proto_path=${CMAKE_SOURCE_DIR}/proto
                --cpp_out=${unreal_src_dir}
                ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
        DEPENDS
            ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
        COMMENT "Create Protobuf messages."
        VERBATIM
    )
    add_custom_target(
        ${target}
        DEPENDS
        ${unreal_src_dir}/${file_root}.pb.h
        ${unreal_src_dir}/${file_root}.pb.cc
    )
    set_source_files_properties(
        ${unreal_src_dir}/${file_root}.pb.h
        ${unreal_src_dir}/${file_root}.pb.cc
        PROPERTIES
            GENERATED true
    )
endmacro()

find_package(PythonInterp)
if (PYTHON_EXECUTABLE)
    add_custom_command(
        OUTPUT
            ${CMAKE_SOURCE_DIR}/tests/test_data.hpp
        COMMAND
            ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/cmake/make_data.py
                --nations-file=${unreal_root}/nations.json
                --map-file=${unreal_root}/default_map.json
                --oob-file=${unreal_root}/default_oob.json
                --units-file=${unreal_root}/units.json
                --scenario-file=${unreal_root}/scenarios/the_wind.json
                --hex-map-config-file=${unreal_root}/hex_map_config.json
                --out-file=${CMAKE_SOURCE_DIR}/tests/test_data.hpp
        DEPENDS
            ${unreal_root}/nations.json
            ${unreal_root}/default_map.json
            ${unreal_root}/default_oob.json
            ${unreal_root}/units.json
            ${unreal_root}/scenarios/the_wind.json
            ${unreal_root}/hex_map_config.json
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/make_data.py
        COMMENT "Create JSON data header."
        VERBATIM
    )
    add_custom_target(data_header_target DEPENDS ${CMAKE_SOURCE_DIR}/tests/test_data.hpp)

    set(protobuf_python_dir ${CMAKE_BINARY_DIR}/protobuf_python)

    if (CYGWIN OR MINGW)
        message(FATAL_ERROR "Cygwin and MSYS builds are not supported!  Use the Windows version of CMake.")
    elseif (APPLE)
        set(protobuf_python_pth_dir ${protobuf_python_dir}/lib/python/site-packages)
    elseif (WIN32)
        set(protobuf_python_pth_dir ${protobuf_python_dir}/Python27/site-packages)
    elseif (UNIX)
        set(protobuf_python_pth_dir ${protobuf_python_dir}/lib/python2.7/site-packages)
    endif()

    add_custom_command(
        OUTPUT
            ${protobuf_python_pth_dir}/setuptools.pth
        COMMAND
            ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/build_protobuf_python.py
                --working-dir ${CMAKE_HOME_DIRECTORY}/${protobuf_dir}/python
                --protoc-path $<TARGET_FILE_DIR:protoc>
                --protobuf-python-dir ${protobuf_python_dir}
        DEPENDS
            protoc
            ${CMAKE_SOURCE_DIR}/src/build_protobuf_python.py
        COMMENT "Build the Python Protobuf module."
        VERBATIM
    )

    add_custom_target(
        install_python_protobuf
        DEPENDS
            ${protobuf_python_pth_dir}/setuptools.pth
    )

    macro(python_generate_sources file_root namespace)
        add_custom_target(
            make_desc_file_${file_root}
            COMMAND
                protoc
                    --proto_path=${CMAKE_HOME_DIRECTORY}/proto
                    --descriptor_set_out=${CMAKE_CURRENT_BINARY_DIR}/${file_root}.desc
                    ${CMAKE_HOME_DIRECTORY}/proto/${file_root}.proto
            DEPENDS
                install_python_protobuf
                ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
            COMMENT "Make ${file_root}.desc from ${file_root}.proto."
            VERBATIM
        )
    
        set(generated_sources
            ${unreal_src_dir}/${file_root}.hpp
            ${unreal_src_dir}/${file_root}.cpp
        )
        set(python_generated_sources ${python_generated_sources} ${generated_sources})

        set(ns_arg --namespace ${namespace})
        if (${namespace} STREQUAL GLOBAL)
            set(ns_arg)
        endif ()
    
        add_custom_command(
            OUTPUT
                ${generated_sources}
            COMMAND
                ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/protobuf_codegen.py
                    --site-packages ${protobuf_python_pth_dir}
                    --proto-file ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
                    --desc-file ${CMAKE_CURRENT_BINARY_DIR}/${file_root}.desc
                    --hpp-file ${unreal_src_dir}/${file_root}.hpp
                    --cpp-file ${unreal_src_dir}/${file_root}.cpp
                    ${ns_arg}
            DEPENDS
                ${CMAKE_SOURCE_DIR}/src/protobuf_codegen.py
                ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
                make_desc_file_${file_root}
            COMMENT "Create ${file_root}.{h,c}pp."
            VERBATIM
        )
        add_custom_target(
            ${file_root}_target
            DEPENDS
                ${generated_sources}
        )
    endmacro()

    set(python_generated_sources)
    python_generate_sources(base_types GLOBAL)
    python_generate_sources(start_data start_data)
    python_generate_sources(game_data GLOBAL)
    python_generate_sources(visual_config visual_config)

    set_source_files_properties(
        ${python_generated_sources}
        PROPERTIES
            GENERATED true
    )
endif ()

include_directories(
    ${unreal_src_dir}
    ${unreal_3rd_party_root}/Protobuf/Includes
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DBUILD_FOR_TEST)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(perf)
