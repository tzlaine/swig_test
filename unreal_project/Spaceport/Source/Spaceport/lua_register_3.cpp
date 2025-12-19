#include "lua.hpp"

#include "game_data_metadata.hpp"


void detail::lua_register_3(sol::state & l)
{
    REGISTER_GAME_DATA_TYPE(l, hex_t);
    REGISTER_GAME_DATA_TYPE(l, province_t);
    REGISTER_GAME_DATA_TYPE(l, nation_t);
    REGISTER_GAME_DATA_TYPE(l, game_state_t);
}
