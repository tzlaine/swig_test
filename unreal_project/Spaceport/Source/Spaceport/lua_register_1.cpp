#include "lua.hpp"

#include "game_data_metadata.hpp"


void detail::lua_register_1(sol::state & l)
{
    REGISTER_GAME_DATA_TYPE(l, unit_design_t);
    REGISTER_GAME_DATA_TYPE(l, unit_t);
    REGISTER_GAME_DATA_TYPE(l, fleet_position_t);
    REGISTER_GAME_DATA_TYPE(l, fleet_t);
    REGISTER_GAME_DATA_TYPE(l, fleets_t);
#if 0 // TODO: adode::name_t
    REGISTER_GAME_DATA_TYPE(l, planet_effect_t);
#endif
    REGISTER_GAME_DATA_TYPE(l, planet_t);
    REGISTER_GAME_DATA_TYPE(l, location_object_t);
}
