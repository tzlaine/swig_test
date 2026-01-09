#include "lua.hpp"

#include "game_data_formatters.hpp"
#include "game_data_metadata.hpp"


void detail::lua_register_4(sol::state & l)
{
    REGISTER_GAME_DATA_TYPE(l, resource_t);
    REGISTER_GAME_DATA_TYPE(l, day_update_t);
    REGISTER_GAME_DATA_TYPE(l, month_update_t);
    REGISTER_GAME_DATA_TYPE(l, year_update_t);
}
