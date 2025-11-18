cmake_minimum_required(VERSION 4.0)

project(spaceport)

set(use_asan false)

set(CMAKE_CXX_STANDARD 20)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES i386 x86_64)

    if (${use_asan})
        add_definitions(-fsanitize=address)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif ()
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif ()

add_subdirectory(googletest-1.10.0)
#TODO add_subdirectory(benchmark-1.0.0)

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
set(protobuf_dir protobuf-3.13.0)
add_subdirectory(${protobuf_dir}/cmake)

set(boost_dir ${CMAKE_HOME_DIRECTORY}/boost_1_89_0)
include_directories(${boost_dir})

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
        ${CMAKE_BINARY_DIR}}/${file_root}_pb2.py
        PROPERTIES
            GENERATED true
    )
endmacro()

find_package(Python3 COMPONENTS Interpreter Development)
if (Python3_FOUND)
    set(installed_token ${CMAKE_BINARY_DIR}/python_google_protobuf_installed_token)

    add_custom_command(
        OUTPUT
            ${installed_token}
        COMMAND
            ${Python3_EXECUTABLE} -m pip install six # We also need this.
        COMMAND
            ${Python3_EXECUTABLE} -m pip install protobuf==3.13.0
        COMMAND
            ${CMAKE_COMMAND} -E copy ${CMAKE_HOME_DIRECTORY}/CMakeLists.txt ${installed_token}
        DEPENDS
            protoc
        COMMENT "Install the Google Protobuf Python module."
        VERBATIM
    )

    add_custom_target(
        install_python_protobuf
        DEPENDS
            ${installed_token}
    )

    macro(python_generate_sources file_root namespace)
        add_custom_target(
            make_desc_file_${file_root}
            COMMAND
                protoc
                    --proto_path=${CMAKE_HOME_DIRECTORY}/proto
                    --descriptor_set_out=${CMAKE_CURRENT_BINARY_DIR}/${file_root}.desc
                    --include_source_info
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
            ${unreal_src_dir}/${file_root}_formatters.hpp
            ${unreal_src_dir}/${file_root}_serialization.hpp
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
                ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/protobuf_codegen.py
                    --proto-file ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
                    --desc-file ${CMAKE_CURRENT_BINARY_DIR}/${file_root}.desc
                    --file-root ${unreal_src_dir}/${file_root}
                    ${ns_arg}
            DEPENDS
                ${CMAKE_SOURCE_DIR}/src/protobuf_codegen.py
                ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
                make_desc_file_${file_root}
            COMMENT "Create ${file_root}.{h,c}pp, formatters, and serialization headers."
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
    python_generate_sources(game_data GLOBAL)

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
# TODO add_subdirectory(perf)
