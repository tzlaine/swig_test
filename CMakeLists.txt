add_subdirectory(googletest)

set(boost_dir ${CMAKE_HOME_DIRECTORY}/boost_1_60_0)
include_directories(${boost_dir})
add_definitions(-DBOOST_NO_EXCEPTIONS)
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
endif ()

include_directories(include)

add_subdirectory(tests)
add_subdirectory(src)
