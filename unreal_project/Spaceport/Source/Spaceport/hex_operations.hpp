#pragma once
#include "game_data.hpp"

#include <boost/array.hpp>
#include <boost/container/static_vector.hpp>

#if LOG
#include <fstream>
#endif

#include <cassert>
#include <string>


enum class hex_direction_t {
    above_right,
    above,
    above_left,
    below_left,
    below,
    below_right,
    hex_directions
};

inline hex_direction_t& operator++(hex_direction_t& d)
{
    assert(d != hex_direction_t::hex_directions);
    const int n = static_cast<int>(hex_direction_t::hex_directions);
    int d_int = static_cast<int>(d);
    d_int = (d_int + 1) % n;
    d = hex_direction_t(d_int);
    return d;
}

extern const hex_coord_t invalid_hex_coord;
extern const hex_direction_t all_hex_directions[6];


inline bool operator== (hex_coord_t lhs, hex_coord_t rhs)
{ return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!= (hex_coord_t lhs, hex_coord_t rhs)
{ return !(lhs == rhs); }
inline bool operator< (hex_coord_t lhs, hex_coord_t rhs)
{ return lhs.x < rhs.x || lhs.x == rhs.x && lhs.y < rhs.y; }

inline std::string hex_string(hex_coord_t hc)
{
    std::string retval;
    if (hc.x + 1 < 10)
        retval += "0";
    retval += std::to_string(hc.x + 1);
    if (hc.y + 1 < 10)
        retval += "0";
    retval += std::to_string(hc.y + 1);
    return retval;
}

#if LOG
std::ostream& operator<< (std::ostream& os, hex_coord_t hc)
{
    return os << (hc.x + 1 < 10 ? "0" : "") << (hc.x + 1) << (hc.y + 1 < 10 ? "0" : "") << (hc.y + 1);
}
#endif

inline hex_coord_t hex_above (hex_coord_t hc)
{ return hex_coord_t{hc.x, hc.y - 1}; }

inline hex_coord_t hex_below (hex_coord_t hc)
{ return hex_coord_t{hc.x, hc.y + 1}; }

inline hex_coord_t hex_above_left (hex_coord_t hc)
{ return hex_coord_t{hc.x - 1, hc.y + (hc.x % 2 ? 0 : -1)}; }

inline hex_coord_t hex_below_left (hex_coord_t hc)
{ return hex_coord_t{hc.x - 1, hc.y + (hc.x % 2 ? 1 : 0)}; }

inline hex_coord_t hex_above_right (hex_coord_t hc)
{ return hex_coord_t{hc.x + 1, hc.y + (hc.x % 2 ? 0 : -1)}; }

inline hex_coord_t hex_below_right (hex_coord_t hc)
{ return hex_coord_t{hc.x + 1, hc.y + (hc.x % 2 ? 1 : 0)}; }


inline hex_coord_t adjacent_hex_coord (hex_coord_t hc, hex_direction_t hd)
{
    switch (hd) {
#define CASE(x) case hex_direction_t::x: return hex_##x(hc)
    CASE(above_right);
    CASE(above);
    CASE(above_left);
    CASE(below_left);
    CASE(below);
    CASE(below_right);
#undef CASE
    default:
    case hex_direction_t::hex_directions:
        return invalid_hex_coord;
    };
}


inline bool on_map (hex_coord_t hc, int width, int height)
{ return 0 <= hc.x && hc.x < width && 0 <= hc.y && hc.y < height; }

inline bool on_map (hex_coord_t hc, const map_t& m)
{ return on_map(hc, m.width, m.height); }


inline int hex_index (hex_coord_t hc, int width)
{ return hc.x + hc.y * width; }


inline int hex_id (hex_coord_t hc)
{ return (hc.x + 1) * 100 + hc.y + 1; }


// Static container for hex ids within R=2 of a central hex.
typedef boost::container::static_vector<hex_coord_t, 19> neighbors_t;

inline neighbors_t adjacent_hex_coords (hex_coord_t hc, const map_t& m, int r = 1)
{
    assert(r == 1 || r == 2);
    neighbors_t retval;
    if (on_map(hc, m)) {
        retval.push_back(hc);
        for (hex_direction_t d : all_hex_directions) {
            hex_coord_t r1 = adjacent_hex_coord(hc, d);
            if (on_map(r1, m)) {
                retval.push_back(r1);

                {
                    hex_coord_t r2 = adjacent_hex_coord(r1, d);
                    if (on_map(r2, m))
                        retval.push_back(r2);
                }

                {
                    auto d2 = d;
                    ++d2;
                    hex_coord_t r2 = adjacent_hex_coord(r1, d2);
                    if (on_map(r2, m))
                        retval.push_back(r2);
                }
            }
        }
    }
    return retval;
}

inline hex_coord_t to_hex_coord (int hex_id)
{
    int hex_x = hex_id / 100 - 1;
    int hex_y = hex_id % 100 - 1;
    return hex_coord_t{hex_x, hex_y};
}
