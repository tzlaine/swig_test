set(source_files
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lapi.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lauxlib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lbaselib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lcode.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lcorolib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lctype.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/ldblib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/ldebug.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/ldo.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/ldump.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lfunc.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lgc.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/linit.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/liolib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/llex.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lmathlib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lmem.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/loadlib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lobject.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lopcodes.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/loslib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lparser.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lstate.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lstring.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lstrlib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/ltable.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/ltablib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/ltm.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lua.c
    # ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/luac.c # excluding the compiler
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lundump.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lutf8lib.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lvm.c
    ${CMAKE_SOURCE_DIR}/lua-5.4.8/src/lzio.c)
set_source_files_properties(${source_files} PROPERTIES LANGUAGE CXX)

add_library(lua STATIC ${source_files})
target_include_directories(lua PRIVATE ${CMAKE_SOURCE_DIR}/lua-5.4.8/src)
