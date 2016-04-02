cmake_minimum_required(VERSION 3.0)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES i386 x86_64)
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif ()

add_subdirectory(googletest)

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif ()

set(protobuf_dir protobuf-v3.0.0-beta-2)
add_subdirectory(${protobuf_dir}/cmake)

set(boost_dir ${CMAKE_HOME_DIRECTORY}/boost_1_60_0)
include_directories(${boost_dir})
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

add_custom_command(
    OUTPUT
        test_data.hpp
    COMMAND
        ${CMAKE_COMMAND}
            -Dnations_file=${CMAKE_HOME_DIRECTORY}/nations.txt
            -Dmap_file=${CMAKE_HOME_DIRECTORY}/default_map.txt
            -Doob_file=${CMAKE_HOME_DIRECTORY}/default_oob.txt
            -Din_file=${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_data.hpp.in
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/make_data.cmake
    DEPENDS
        ${CMAKE_HOME_DIRECTORY}/nations.txt
        ${CMAKE_HOME_DIRECTORY}/default_map.txt
        ${CMAKE_HOME_DIRECTORY}/default_oob.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_data.hpp.in
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/make_data.cmake
    COMMENT "Create JSON data header."
    VERBATIM
)
add_custom_target(data_header_target DEPENDS test_data.hpp)

macro(generate_protobuf file_root target)
    add_custom_command(
        OUTPUT
            ${CMAKE_BINARY_DIR}/${file_root}.pb.cc
            ${CMAKE_BINARY_DIR}/${file_root}.pb.h
        COMMAND
            protoc
                --proto_path=${CMAKE_SOURCE_DIR}/proto
                --cpp_out=${CMAKE_BINARY_DIR}
                --csharp_out=${CMAKE_HOME_DIRECTORY}/unity_project/Assets/plugins
                ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
        DEPENDS
            ${CMAKE_SOURCE_DIR}/proto/${file_root}.proto
        COMMENT "Create Protobuf messages."
        VERBATIM
    )
    add_custom_target(
        ${target}
        DEPENDS
        ${CMAKE_BINARY_DIR}/model.pb.h
        ${CMAKE_BINARY_DIR}/model.pb.cc
    )
    set_source_files_properties(
        ${CMAKE_BINARY_DIR}/model.pb.h
        ${CMAKE_BINARY_DIR}/model.pb.cc
        PROPERTIES
            GENERATED true
    )
endmacro()

find_package(PythonInterp)
if (PYTHON_EXECUTABLE)
    get_target_property(
        protoc_path
        protoc
        RUNTIME_OUTPUT_DIRECTORY
    )
    set(protobuf_python_dir ${CMAKE_BINARY_DIR}/protobuf_python)

    add_custom_target(
        build_protobuf_python
        COMMAND
            ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/build_protobuf_python.py
                --working-dir ${CMAKE_HOME_DIRECTORY}/${protobuf_dir}/python
                --protoc-path ${protoc_path}
                --protobuf-python-dir ${protobuf_python_dir}
        DEPENDS
            protoc
            ${CMAKE_SOURCE_DIR}/src/build_protobuf_python.py
        COMMENT "Build the Python Protobuf module."
        VERBATIM
    )

    add_custom_target(
        make_desc_file
        COMMAND
            protoc
                --proto_path=${CMAKE_HOME_DIRECTORY}/proto
                --descriptor_set_out=${CMAKE_CURRENT_BINARY_DIR}/model.desc
                ${CMAKE_HOME_DIRECTORY}/proto/model.proto
        DEPENDS
            build_protobuf_python
            ${CMAKE_SOURCE_DIR}/proto/model.proto
        COMMENT "Make model.desc from model.proto."
        VERBATIM
    )

    add_custom_command(
        OUTPUT
            ${CMAKE_SOURCE_DIR}/src/model_data.hpp
            ${CMAKE_SOURCE_DIR}/src/model_data.cpp
        COMMAND
            ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/protobuf_codegen.py
                --site-packages ${protobuf_python_dir}/lib/python/site-packages
                --proto-file ${CMAKE_SOURCE_DIR}/proto/model.proto
                --desc-file=${CMAKE_CURRENT_BINARY_DIR}/model.desc
                --hpp-file ${CMAKE_SOURCE_DIR}/src/model_data.hpp
                --cpp-file ${CMAKE_SOURCE_DIR}/src/model_data.cpp
        DEPENDS
            ${CMAKE_SOURCE_DIR}/src/protobuf_codegen.py
            make_desc_file
        COMMENT "Create model_data.{h,c}pp."
        VERBATIM
    )
    add_custom_target(
        model_data_target
        DEPENDS
            ${CMAKE_SOURCE_DIR}/src/model_data.hpp
            ${CMAKE_SOURCE_DIR}/src/model_data.cpp
    )

    set_source_files_properties(
        ${CMAKE_SOURCE_DIR}/src/model_data.hpp
        ${CMAKE_SOURCE_DIR}/src/model_data.cpp
        PROPERTIES
            GENERATED true
    )
endif ()

include_directories(
    include
    ${CMAKE_HOME_DIRECTORY}/${protobuf_dir}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(tests)
add_subdirectory(src)
