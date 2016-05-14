#pragma once

#include "start_data_t.hpp"
#include "game_data_t.hpp"


enum class grid_type_t {
    capital,
    offmap,
    partial
};

enum class grid_check_time_t {
    during_combat,
    not_during_combat
};

struct supply_grid_t
{
    grid_type_t type;

    std::vector<hex_coord_t> supply_points;
    std::vector<hex_coord_t> hexes_in_supply;
};

bool supply_source (int nation_id, hex_t const & hex, map_t const & map);

bool supply_point (int nation_id, hex_t const & hex, map_t const & map);

std::vector<supply_grid_t> find_supply_grids (
    int nation_id,
    start_data::start_data_t const & start_data,
    game_data_t const & game_data
);
