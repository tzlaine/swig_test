########################################
# start_data library
########################################
generate_protobuf(start_data start_data_message_target)

if (NOT MSVC)
    add_definitions(-Wall)
endif ()

add_definitions(-DBUILD_FOR_TEST)

add_library(
    start_data
    STATIC
    ${unreal_src_dir}/start_data_t.cpp
    ${unreal_src_dir}/json2pb.cc
    ${unreal_src_dir}/start_data.cpp
    ${unreal_src_dir}/start_data.pb.cc
)

target_link_libraries(
    start_data
    libprotobuf
    jansson
)

add_dependencies(start_data start_data_message_target)
if (PYTHON_EXECUTABLE)
    add_dependencies(start_data start_data_target)
endif ()
