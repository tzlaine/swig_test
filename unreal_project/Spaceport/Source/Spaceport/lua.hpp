#pragma once

#define SOL_LUA_VERSION 504
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <exception>


namespace detail {
    template<typename T>
    T make_default()
    {
        return T{};
    }

    void lua_register_1(sol::state & l);
    void lua_register_2(sol::state & l);
}

#define REGISTER_GAME_DATA_TYPE(lua, T)                                        \
    sol::usertype<T> T##_type = lua.new_usertype<T>(#T, sol::no_constructor);  \
    T##_type["new"] = &detail::make_default<T>;                                \
    detail::metadata<T>::foreach_member(                                       \
        [&](auto meta) { T##_type[meta.name_] = meta.ptr_; })


sol::state & lua();

std::string script_path(std::string const & script);

void script_file(std::string const & script);
