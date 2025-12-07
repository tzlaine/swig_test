#pragma once

#include "constants.hpp"
#include "base_types.hpp"

#if !defined(BUILD_FOR_TEST)
#include <Math/MathFwd.h>
#endif


struct point_2d
{
    double x;
    double y;

    friend point_2d operator-(point_2d pt1, point_2d pt2)
    {
        return {pt1.x - pt2.x, pt1.y - pt2.y};
    }

    friend point_2d operator*(point_2d pt, double c)
    {
        return {pt.x * c, pt.y * c};
    }

    friend point_2d operator/(point_2d pt, double c)
    {
        return {pt.x / c, pt.y / c};
    }

    bool operator==(point_2d const &) const = default;
};

inline double dist_sq(point_2d pt1, point_2d pt2)
{
    auto const delta = pt1 - pt2;
    return delta.x * delta.x + delta.y * delta.y;
}

inline double dist(point_2d pt1, point_2d pt2)
{
    return std::sqrt(dist_sq(pt1, pt2));
}

inline double norm(point_2d pt)
{
    return std::sqrt(dist_sq(pt, point_2d{0, 0}));
}

inline double dot(point_2d pt1, point_2d pt2)
{
    return pt1.x * pt2.x + pt1.y * pt2.y;
}

inline bool within(point_2d pt1, point_2d pt2, double dist)
{
    return dist_sq(pt1, pt2) + 0.001 < dist * dist;
}


#if !defined(BUILD_FOR_TEST)
inline FVector to_fvector(point_2d pt) { return FVector(pt.x, pt.y, 0); }
#endif

struct box_2d
{
    point_2d min_;
    point_2d max_;

    bool operator==(box_2d const &) const = default;
};

inline point_2d extent(box_2d box) { return box.max_ - box.min_; }

inline double closest_distance_sq(box_2d box, point_2d pt)
{
    double retval = 0.0;
    if (pt.x < box.min_.x) {
        double const delta = pt.x - box.min_.x;
        retval += delta * delta;
    } else if (pt.x > box.max_.x) {
        double const delta = pt.x - box.max_.x;
        retval += delta * delta;
    }
    if (pt.y < box.min_.y) {
        double const delta = pt.y - box.min_.y;
        retval += delta * delta;
    } else if (pt.y > box.max_.y) {
        double const delta = pt.y - box.max_.y;
        retval += delta * delta;
    }
    return retval;
}

struct int_point
{
    int x;
    int y;

    friend int_point operator+(int_point lhs, int_point rhs)
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y};
    }

    bool operator==(int_point const &) const = default;
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
