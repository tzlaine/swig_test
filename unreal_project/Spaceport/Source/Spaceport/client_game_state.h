#pragma once

#include "game_data.hpp"
#include "sparse_vector.hpp"


struct client_game_state_t
{
    int map_width;
    int map_height;
    sparse_vector<hex_t> hexes;
    sparse_vector<system_t> systems;
    sparse_vector<planet_t> planets;
    sparse_vector<nation_t> nations;
    bool operator==(client_game_state_t const &) const = default;
};
