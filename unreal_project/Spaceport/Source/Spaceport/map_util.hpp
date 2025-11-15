#pragma once

#include "constants.hpp"
#include "base_types.hpp"


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

struct box_2d
{
    point_2d min_;
    point_2d max_;
};

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
