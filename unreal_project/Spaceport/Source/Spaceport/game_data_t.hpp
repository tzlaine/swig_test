#pragma once

#include "game_data.hpp"
#include "hex_operations.hpp"

#include <boost/utility.hpp>
#include <boost/container/flat_set.hpp>


struct game_data_t
{
    game_data_t ();

    hex_t const & hex (hex_coord_t hc) const
    {
        auto const i = hex_index_t(hc, game_state_.map_width);
        assert(i < (int)game_state_.hexes.size());
        return game_state_.hexes[i];
    }

    std::vector<hex_t> const & hexes () const
    {
        return game_state_.hexes;
    }

    std::vector<system_t> const& systems () const
    {
        return game_state_.systems;
    }

    std::vector<planet_t> const& planets () const
    {
        return game_state_.planets;
    }

    std::vector<nation_t> const& nations () const
    {
        return game_state_.nations;
    }

private:
    game_state_t game_state_;
};
