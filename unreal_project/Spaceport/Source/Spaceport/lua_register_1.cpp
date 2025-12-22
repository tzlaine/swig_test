#include "lua.hpp"

#include "game_data_formatters.hpp"
#include "game_data_metadata.hpp"


void detail::lua_register_1(sol::state & l)
{
    REGISTER_GAME_DATA_TYPE(l, unit_design_t);
    REGISTER_GAME_DATA_TYPE(l, unit_t);
    REGISTER_GAME_DATA_TYPE(l, fleet_position_t);
    REGISTER_GAME_DATA_TYPE(l, fleet_t);
    REGISTER_GAME_DATA_TYPE(l, fleets_t);

    REGISTER_GAME_DATA_TYPE(l, planet_effect_t);
    planet_effect_t_type["new"] = sol::overload(
        [](char const * name) { return planet_effect_t{adobe::name_t(name)}; },
        [](char const * name, char const * reason) {
            return planet_effect_t{adobe::name_t(name), adobe::name_t(reason)};
        },
        [](char const * name, char const * reason, float value) {
            return planet_effect_t{
                adobe::name_t(name), adobe::name_t(reason), value};
        },
        &detail::make_default<planet_effect_t>);

    REGISTER_GAME_DATA_TYPE(l, planet_t);
}
