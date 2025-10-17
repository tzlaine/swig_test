#pragma once

#include "game_data_t.hpp"


namespace detail {

    struct supply_relevant_contents_t
    {
        int friendly_ships;
        int friendly_units;
        int friendly_bases;
        int enemy_ships;
        int enemy_units;
        int enemy_bases;
    };

    supply_relevant_contents_t find_supply_relevant_contents (
        hex_t const & hex,
        int nation_id,
        game_data_t const & game_data
    );

}

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

#ifdef BUILD_FOR_TEST
    struct supply_source_t
    {
        hex_id_t origin_;
        std::vector<hex_id_t> path_;
    };
    boost::container::flat_map<hex_coord_t, supply_source_t> hex_supply_sources;
#endif
};

bool supply_source (int nation_id, hex_t const & hex, std::vector<hex_t> const & hexes, int map_width);

bool supply_point (int nation_id, hex_t const & hex, std::vector<hex_t> const & hexes, int map_width);

std::vector<supply_grid_t> find_supply_grids (
    int nation_id,
    game_data_t const & game_data
);
