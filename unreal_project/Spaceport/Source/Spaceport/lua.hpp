#pragma once

#include "config.hpp"

#define SOL_LUA_VERSION 504
#define SOL_ALL_SAFETIES_ON 1
PUSH_WARNING
DISABLE_WARNING(4602)
#include <sol/sol.hpp>
POP_WARNING

#include <adobe/name.hpp>


namespace detail {
    template<typename MemPtr>
    struct member_type;
    template<typename C, typename T>
    struct member_type<T C::*>
    {
        using type = T;
    };
    template<typename T>
    using member_type_t = typename member_type<T>::type;

    template<typename T>
    T make_default()
    {
        return T{};
    }

    void lua_register_1(sol::state & l);
    void lua_register_2(sol::state & l);
    void lua_register_3(sol::state & l);
    void lua_register_enums(sol::state & l);
    void lua_register_functions(sol::state & l);
}


#define REGISTER_GAME_DATA_TYPE(lua, T)                                        \
    sol::usertype<T> T##_type = lua.new_usertype<T>(                           \
        #T,                                                                    \
        sol::no_constructor,                                                   \
        sol::meta_function::to_string,                                         \
        [](T const & x) { return std::format("{}", x); });                     \
    T##_type["new"] = &detail::make_default<T>;                                \
    detail::metadata<T>::foreach_member(                                       \
        [&](auto meta) { T##_type[meta.name_] = meta.ptr_; })


#if WITH_EDITOR
// Does just what it sounds like.  This should only be necessary to do at the
// start of an Unreal editor run, since in an editor build, the initial
// startup code only gets run once, no matter how many times you run the code
// in the editor.  CALLING THIS AT ANY OTHER TIME IS A BUG.
void reset_all_lua_states();
#endif

// Returns a sol3 lua state, initialized with all our project-specific Lua
// code.
sol::state make_lua_state();

// Returns a reference to a thread-local singleton sol3 lua state created by
// make_lua_state().
sol::state & lua();

// Maps a Lua file like 'foo.lua' to the full path to that script, regardless
// of which build we're in (CMake or Unreal), and regardless of whether we're
// running in the Unreal editor, or a packaged build.
std::string script_path(std::string const & script);

// Executes the script file `script` as if by
// `lua().script_file(script_path(script))`.
void script_file(std::string const & script);
