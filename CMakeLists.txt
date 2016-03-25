cmake_minimum_required(VERSION 3.0)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES i386 x86_64)
endif ()

add_subdirectory(googletest)

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif ()
add_subdirectory(protobuf-v3.0.0-beta-2/cmake)

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
                --csharp_out=${CMAKE_BINARY_DIR}
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
    add_custom_command(
        OUTPUT
            ${CMAKE_SOURCE_DIR}/src/model_data.hpp
            ${CMAKE_SOURCE_DIR}/src/model_data.cpp
        COMMAND
            ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/proto_to_cpp.py
                --in-file ${CMAKE_SOURCE_DIR}/proto/model.proto
                --hpp-file ${CMAKE_SOURCE_DIR}/src/model_data.hpp
                --cpp-file ${CMAKE_SOURCE_DIR}/src/model_data.cpp
        DEPENDS
            ${CMAKE_SOURCE_DIR}/src/proto_to_cpp.py
            ${CMAKE_SOURCE_DIR}/proto/model.proto
        COMMENT "Create model data header."
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
    ${CMAKE_HOME_DIRECTORY}/protobuf-v3.0.0-beta-2/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(tests)
add_subdirectory(src)
