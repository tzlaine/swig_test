#pragma once

#include "constants.hpp"
#include "game_data.hpp"

#include <numbers>


struct game_start_params;
struct game_state_t;

// TODO: Move these.
struct point_2d
{
    double x;
    double y;
};

inline point_2d operator-(point_2d pt1, point_2d pt2)
{
    return {pt1.x - pt2.x, pt1.y - pt2.y};
}

inline bool within(point_2d pt1, point_2d pt2, double dist)
{
    auto const delta = pt1 - pt2;
    return (delta.x * delta.x + delta.y * delta.y + 0.001) < (dist * dist);
}

inline point_2d hex_position(hex_coord_t hc, int map_height)
{
    point_2d retval;
    retval.x = hc.x * 1.5;
    retval.y = (map_height - 1 - hc.y) * 2 * sin_60;
    if ((hc.x + 1000) % 2 == 1)
        retval.y -= sin_60;
    return retval;
}

inline double degrees_to_radians(double d)
{
    return d / 180.0 * std::numbers::pi;
}

namespace generation {
    namespace detail {
        float determine_growth_factor_and_effects(planet_t & planet);

        bool generate_planet(planet_t & planet, system_t const & system);

        star_t generate_star();

        bool generate_system(
            system_t & system, std::vector<planet_t> & planets,
            hex_coord_t hc, point_2d hex_world_pos, int system_id);

#if defined(BUILD_FOR_TEST)
        inline thread_local bool g_skip_system_generation_for_testing = false;
#endif
    }

    void generate_galaxy(game_start_params const & params,
                         game_state_t & game_state);
}
