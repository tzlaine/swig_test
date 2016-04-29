#pragma once

#include "game_data.hpp"
#include <boost/utility.hpp>

namespace start_data { struct start_data_t; }

struct game_data_t
{
    game_data_t (start_data::start_data_t const & start_data);

    map_t const & map () const
    { return map_; }

    offmap_areas_t const & offmap_areas () const
    { return offmap_areas_; }

private:
    map_t map_;
    offmap_areas_t offmap_areas_;
};
