#pragma once

#define SOL_LUA_VERSION 504
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

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

    // Get/set an adobe::name_t, which sol3 knows nothing about.
    template<typename T, auto MemPtr>
    char const * get_name(T const & x)
    {
        return (x.*MemPtr).c_str();
    }
    template<typename T, auto MemPtr>
    void set_name(T & x, char const * s)
    {
        x.*MemPtr = adobe::name_t(s);
    }

    void lua_register_1(sol::state & l);
    void lua_register_2(sol::state & l);
    void lua_register_3(sol::state & l);
    void lua_register_4(sol::state & l);
}


#define REGISTER_GAME_DATA_TYPE(lua, T)                                        \
    sol::usertype<T> T##_type = lua.new_usertype<T>(#T, sol::no_constructor);  \
    T##_type["new"] = &detail::make_default<T>;                                \
    detail::metadata<T>::foreach_member([&](auto meta) {                       \
        if constexpr (std::same_as<                                            \
                          detail::member_type_t<decltype(meta.ptr_)>,          \
                          adobe::name_t>) {                                    \
            T##_type[meta.name_] = sol::property(                              \
                &detail::get_name<T, decltype(meta.ptr_){}>,                   \
                &detail::set_name<T, decltype(meta.ptr_){}>);                  \
        } else {                                                               \
            T##_type[meta.name_] = meta.ptr_;                                  \
        }                                                                      \
    })


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
