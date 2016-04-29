#pragma once

#include "game_data.hpp"
#include "hex_operations.hpp"
#include <boost/utility.hpp>

namespace start_data { struct start_data_t; }

struct game_data_t
{
    game_data_t ();
    game_data_t (start_data::start_data_t const & start_data);

    hex_t const & hex (hex_coord_t hc) const
    {
        auto const i = hex_index(hc, map_.width);
        assert(i < map_.hexes.size());
        return map_.hexes[i];
    }

    province_t const & province (int province_id) const
    {
        assert(province_id < map_.provinces.size());
        return map_.provinces[province_id];
    }

    map_t const & map () const
    { return map_; }

    offmap_areas_t const & offmap_areas () const
    { return offmap_areas_; }

private:
    map_t map_;
    offmap_areas_t offmap_areas_;
};
