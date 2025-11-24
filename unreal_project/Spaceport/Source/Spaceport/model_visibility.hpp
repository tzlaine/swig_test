#pragma once

#include "game_data.hpp"


template<typename T>
bool visible_to(int nation_id, T const & x)
{
    if (nation_id < 0)
        return true;
    return false; // TODO
}

unit_design_t view_of(int nation_id, unit_design_t const & x);

unit_t view_of(int nation_id, unit_t const & x);

fleet_t view_of(int nation_id, fleet_t const & x);

fleets_t view_of(int nation_id, fleets_t const & x);

planet_effect_t view_of(int nation_id, planet_effect_t const & x);

planet_t view_of(int nation_id, planet_t const & x);

location_object_t view_of(int nation_id, location_object_t const & x);

system_location_t view_of(int nation_id, system_location_t const & x);

star_t view_of(int nation_id, star_t const & x);

system_t view_of(int nation_id, system_t const & x);

hex_t view_of(int nation_id, hex_t const & x);

province_t view_of(int nation_id, province_t const & x);

nation_t view_of(int nation_id, nation_t const & x);
