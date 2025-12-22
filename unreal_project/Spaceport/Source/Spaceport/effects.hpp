#pragma once

#include "check.hpp"
#include "game_data.hpp"
#include "lua.hpp"

#include <mutex>


// Precondition: the Lua state for this thread has already executed the
// effects.lua and generation.lua scripts.
inline void apply_planet_effect(planet_t & planet, planet_effect_t const & pe)
{
    sol::function apply_planet_effects = lua()["apply_planet_effect"];
    apply_planet_effects(planet, pe);
}
