add_subdirectory(googletest)

set(boost_dir ${CMAKE_HOME_DIRECTORY}/boost_1_60_0)
include_directories(${boost_dir})
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

include_directories(include)

add_custom_command(
    OUTPUT
        data.hpp
    COMMAND
        ${CMAKE_COMMAND}
            -Dnations_file=${CMAKE_HOME_DIRECTORY}/nations.json
            -Dmap_file=${CMAKE_HOME_DIRECTORY}/default_map.json
            -Doob_file=${CMAKE_HOME_DIRECTORY}/default_oob.json
            -Din_file=${CMAKE_CURRENT_SOURCE_DIR}/cmake/data.hpp.in
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/make_data.cmake
    DEPENDS
        ${CMAKE_HOME_DIRECTORY}/nations.json
        ${CMAKE_HOME_DIRECTORY}/default_map.json
        ${CMAKE_HOME_DIRECTORY}/default_oob.json
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/data.hpp.in
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/make_data.cmake
    COMMENT "Create data header."
    VERBATIM
)
add_custom_target(data_header_target DEPENDS data.hpp)

add_subdirectory(tests)
add_subdirectory(src)
