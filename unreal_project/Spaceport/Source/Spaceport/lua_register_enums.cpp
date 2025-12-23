#include "lua.hpp"

#include "base_types_enum_lua_bindings.hpp"
#include "game_data_enum_lua_bindings.hpp"


void detail::lua_register_enums(sol::state & l)
{
    base_types_register_enums(l);
    game_data_register_enums(l);
}
