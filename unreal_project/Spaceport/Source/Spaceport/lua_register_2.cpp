#include "lua.hpp"

#include "game_data_formatters.hpp"
#include "game_data_metadata.hpp"


void detail::lua_register_2(sol::state & l)
{
    REGISTER_GAME_DATA_TYPE(l, planet_t);
    REGISTER_GAME_DATA_TYPE(l, location_object_t);
    REGISTER_GAME_DATA_TYPE(l, system_location_t);
    REGISTER_GAME_DATA_TYPE(l, star_t);
    REGISTER_GAME_DATA_TYPE(l, system_t);
}
