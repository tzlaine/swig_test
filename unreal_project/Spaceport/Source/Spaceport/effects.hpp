#pragma once

#include "game_data.hpp"
#include "lua.hpp"

#include <mutex>


inline void apply_planet_effect(planet_t & planet, planet_effect_t const & pe)
{
    sol::table planet_effects = lua()["planet_effects"];
    assert(planet_effects);
    sol::table effect = planet_effects[pe.name.c_str()];
    assert(effect);
    sol::function apply = effect["apply"];
    sol::object value = effect["value"];
    if (value)
        apply(effect, planet);
    else
        apply(effect, planet, pe.value);
}

inline bool transitory(planet_effect_t const & effect)
{
    return false; // TODO
}
