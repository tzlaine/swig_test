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

inline hex_direction_t & operator++ (hex_direction_t & d)
{
    assert(d != hex_direction_t::hex_directions);
    const int n = static_cast<int>(hex_direction_t::hex_directions);
    int d_int = static_cast<int>(d);
    d_int = (d_int + 1) % n;
    d = hex_direction_t(d_int);
    return d;
}

inline hex_direction_t & operator-- (hex_direction_t & d)
{
    assert(d != hex_direction_t::hex_directions);
    const int n = static_cast<int>(hex_direction_t::hex_directions);
    int d_int = static_cast<int>(d);
    d_int = (d_int + n - 1) % n;
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

inline std::string hex_string (hex_coord_t hc)
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

constexpr hex_coord_t const odd_direction_offsets[6] = {
    hex_coord_t{1, 0},
    hex_coord_t{0, -1},
    hex_coord_t{-1, 0},
    hex_coord_t{-1, 1},
    hex_coord_t{0, 1},
    hex_coord_t{1, 1}
};

constexpr hex_coord_t const even_direction_offsets[6] = {
    hex_coord_t{1, -1},
    hex_coord_t{0, -1},
    hex_coord_t{-1, -1},
    hex_coord_t{-1, 0},
    hex_coord_t{0, 1},
    hex_coord_t{1, 0}
};

inline hex_coord_t hex_above_right (hex_coord_t hc)
{ return hex_coord_t{hc.x + 1, hc.y + (hc.x % 2 ? 0 : -1)}; }

inline hex_coord_t hex_above (hex_coord_t hc)
{ return hex_coord_t{hc.x, hc.y - 1}; }

inline hex_coord_t hex_above_left (hex_coord_t hc)
{ return hex_coord_t{hc.x - 1, hc.y + (hc.x % 2 ? 0 : -1)}; }

inline hex_coord_t hex_below_left (hex_coord_t hc)
{ return hex_coord_t{hc.x - 1, hc.y + (hc.x % 2 ? 1 : 0)}; }

inline hex_coord_t hex_below (hex_coord_t hc)
{ return hex_coord_t{hc.x, hc.y + 1}; }

inline hex_coord_t hex_below_right (hex_coord_t hc)
{ return hex_coord_t{hc.x + 1, hc.y + (hc.x % 2 ? 1 : 0)}; }

inline hex_coord_t adjacent_hex_coord (hex_coord_t hc, hex_direction_t hd)
{
    auto const offset =
        (hc.x % 2 ? odd_direction_offsets : even_direction_offsets)[(int)hd];
    return hex_coord_t{hc.x + offset.x, hc.y + offset.y};
}


inline bool on_map (hex_coord_t hc, int width, int height)
{ return 0 <= hc.x && hc.x < width && 0 <= hc.y && hc.y < height; }

inline bool on_map (hex_coord_t hc, const map_t& m)
{ return on_map(hc, m.width, m.height); }

struct hex_index_t;

struct hex_id_t
{
    hex_id_t () :
        value_ (-1)
    {}

    explicit hex_id_t (int hex_id) :
        value_ (hex_id)
    {}

    hex_id_t (hex_index_t hex_index, int map_width);

    explicit hex_id_t (hex_coord_t hc) :
        value_ ((hc.x + 1) * 100 + hc.y + 1)
    {}

    int to_int () const
    { return value_; }

    hex_index_t to_hex_index (int map_width) const;

    hex_coord_t to_hex_coord () const
    {
        int const hex_x = value_ / 100 - 1;
        int const hex_y = value_ % 100 - 1;
        return hex_coord_t{hex_x, hex_y};
    }

private:
    int value_;
};

struct hex_index_t
{
    hex_index_t () :
        value_ (-1)
    {}

    explicit hex_index_t (int hex_index) :
        value_ (hex_index)
    {}

    hex_index_t (hex_id_t hex_id, int map_width)
    {
        int const hex_x = hex_id.to_int() / 100 - 1;
        int const hex_y = hex_id.to_int() % 100 - 1;
        value_ = hex_x + hex_y * map_width;
    }

    explicit hex_index_t (hex_coord_t hc, int map_width) :
        value_ (hc.x + hc.y * map_width)
    {}

    operator int () const
    { return value_; }

    hex_index_t to_hex_id (int map_width) const
    {
        int const hex_x = value_ % map_width;
        int const hex_y = value_ / map_width;
        return hex_index_t((hex_x + 1) * 100 + hex_y + 1);
    }

    hex_coord_t to_hex_coord (int map_width) const
    {
        int const hex_x = value_ % map_width;
        int const hex_y = value_ / map_width;
        return hex_coord_t{hex_x, hex_y};
    }

private:
    int value_;
};

inline hex_id_t::hex_id_t (hex_index_t hex_index, int map_width)
{
    int const hex_x = hex_index % map_width;
    int const hex_y = hex_index / map_width;
    value_ = (hex_x + 1) * 100 + hex_y + 1;
}

inline hex_index_t hex_id_t::to_hex_index (int map_width) const
{
    int const hex_x = value_ / 100 - 1;
    int const hex_y = value_ % 100 - 1;
    return hex_index_t(hex_x + hex_y * map_width);
}


constexpr int const max_useful_hex_distance = 6;

constexpr int const max_hexes_at_radius[max_useful_hex_distance + 1] = {
    1 + 0 * 6,
    1 + 1 * 6,
    1 + 3 * 6,
    1 + 6 * 6,
    1 + 10 * 6,
    1 + 15 * 6,
    1 + 21 * 6
};

constexpr int const max_neighbors = max_hexes_at_radius[max_useful_hex_distance];

// Static container for hex ids within R=6 of a central hex.  Note that max_neighbors ==
// 1 + 1*6 + 2*6 + ... + 6*MAX_USEFUL_HEX_DISTANCE.
typedef boost::container::static_vector<hex_coord_t, max_neighbors> neighbors_t;

hex_coord_t const even_neighbor_offsets[max_neighbors] = {
    { 0, 0 },
    { 1, 0 },
    { 1, -1 },
    { 0, -1 },
    { -1, -1 },
    { -1, 0 },
    { 0, 1 },
    { 1, 1 },
    { 2, 1 },
    { 2, 0 },
    { 2, -1 },
    { 1, -2 },
    { 0, -2 },
    { -1, -2 },
    { -2, -1 },
    { -2, 0 },
    { -2, 1 },
    { -1, 1 },
    { 0, 2 },
    { 1, 2 },
    { 2, 2 },
    { 3, 1 },
    { 3, 0 },
    { 3, -1 },
    { 3, -2 },
    { 2, -2 },
    { 1, -3 },
    { 0, -3 },
    { -1, -3 },
    { -2, -2 },
    { -3, -2 },
    { -3, -1 },
    { -3, 0 },
    { -3, 1 },
    { -2, 2 },
    { -1, 2 },
    { 0, 3 },
    { 1, 3 },
    { 2, 3 },
    { 3, 2 },
    { 4, 2 },
    { 4, 1 },
    { 4, 0 },
    { 4, -1 },
    { 4, -2 },
    { 3, -3 },
    { 2, -3 },
    { 1, -4 },
    { 0, -4 },
    { -1, -4 },
    { -2, -3 },
    { -3, -3 },
    { -4, -2 },
    { -4, -1 },
    { -4, 0 },
    { -4, 1 },
    { -4, 2 },
    { -3, 2 },
    { -2, 3 },
    { -1, 3 },
    { 0, 4 },
    { 1, 4 },
    { 2, 4 },
    { 3, 3 },
    { 4, 3 },
    { 5, 2 },
    { 5, 1 },
    { 5, 0 },
    { 5, -1 },
    { 5, -2 },
    { 5, -3 },
    { 4, -3 },
    { 3, -4 },
    { 2, -4 },
    { 1, -5 },
    { 0, -5 },
    { -1, -5 },
    { -2, -4 },
    { -3, -4 },
    { -4, -3 },
    { -5, -3 },
    { -5, -2 },
    { -5, -1 },
    { -5, 0 },
    { -5, 1 },
    { -5, 2 },
    { -4, 3 },
    { -3, 3 },
    { -2, 4 },
    { -1, 4 },
    { 0, 5 },
    { 1, 5 },
    { 2, 5 },
    { 3, 4 },
    { 4, 4 },
    { 5, 3 },
    { 6, 3 },
    { 6, 2 },
    { 6, 1 },
    { 6, 0 },
    { 6, -1 },
    { 6, -2 },
    { 6, -3 },
    { 5, -4 },
    { 4, -4 },
    { 3, -5 },
    { 2, -5 },
    { 1, -6 },
    { 0, -6 },
    { -1, -6 },
    { -2, -5 },
    { -3, -5 },
    { -4, -4 },
    { -5, -4 },
    { -6, -3 },
    { -6, -2 },
    { -6, -1 },
    { -6, 0 },
    { -6, 1 },
    { -6, 2 },
    { -6, 3 },
    { -5, 3 },
    { -4, 4 },
    { -3, 4 },
    { -2, 5 },
    { -1, 5 },
    { 0, 6 }
};

hex_coord_t const odd_neighbor_offsets[max_neighbors] = {
    { 0, 0 },
    { 1, 1 },
    { 1, 0 },
    { 0, -1 },
    { -1, 0 },
    { -1, 1 },
    { 0, 1 },
    { 1, 2 },
    { 2, 1 },
    { 2, 0 },
    { 2, -1 },
    { 1, -1 },
    { 0, -2 },
    { -1, -1 },
    { -2, -1 },
    { -2, 0 },
    { -2, 1 },
    { -1, 2 },
    { 0, 2 },
    { 1, 3 },
    { 2, 2 },
    { 3, 2 },
    { 3, 1 },
    { 3, 0 },
    { 3, -1 },
    { 2, -2 },
    { 1, -2 },
    { 0, -3 },
    { -1, -2 },
    { -2, -2 },
    { -3, -1 },
    { -3, 0 },
    { -3, 1 },
    { -3, 2 },
    { -2, 2 },
    { -1, 3 },
    { 0, 3 },
    { 1, 4 },
    { 2, 3 },
    { 3, 3 },
    { 4, 2 },
    { 4, 1 },
    { 4, 0 },
    { 4, -1 },
    { 4, -2 },
    { 3, -2 },
    { 2, -3 },
    { 1, -3 },
    { 0, -4 },
    { -1, -3 },
    { -2, -3 },
    { -3, -2 },
    { -4, -2 },
    { -4, -1 },
    { -4, 0 },
    { -4, 1 },
    { -4, 2 },
    { -3, 3 },
    { -2, 3 },
    { -1, 4 },
    { 0, 4 },
    { 1, 5 },
    { 2, 4 },
    { 3, 4 },
    { 4, 3 },
    { 5, 3 },
    { 5, 2 },
    { 5, 1 },
    { 5, 0 },
    { 5, -1 },
    { 5, -2 },
    { 4, -3 },
    { 3, -3 },
    { 2, -4 },
    { 1, -4 },
    { 0, -5 },
    { -1, -4 },
    { -2, -4 },
    { -3, -3 },
    { -4, -3 },
    { -5, -2 },
    { -5, -1 },
    { -5, 0 },
    { -5, 1 },
    { -5, 2 },
    { -5, 3 },
    { -4, 3 },
    { -3, 4 },
    { -2, 4 },
    { -1, 5 },
    { 0, 5 },
    { 1, 6 },
    { 2, 5 },
    { 3, 5 },
    { 4, 4 },
    { 5, 4 },
    { 6, 3 },
    { 6, 2 },
    { 6, 1 },
    { 6, 0 },
    { 6, -1 },
    { 6, -2 },
    { 6, -3 },
    { 5, -3 },
    { 4, -4 },
    { 3, -4 },
    { 2, -5 },
    { 1, -5 },
    { 0, -6 },
    { -1, -5 },
    { -2, -5 },
    { -3, -4 },
    { -4, -4 },
    { -5, -3 },
    { -6, -3 },
    { -6, -2 },
    { -6, -1 },
    { -6, 0 },
    { -6, 1 },
    { -6, 2 },
    { -6, 3 },
    { -5, 4 },
    { -4, 4 },
    { -3, 5 },
    { -2, 5 },
    { -1, 6 },
    { 0, 6 }
};

inline neighbors_t adjacent_hex_coords (hex_coord_t hc, int width, int height, int r = 1)
{
    neighbors_t retval;

    assert(0 < r && r <= 6);
    assert(on_map(hc, width, height));

    int const n = max_hexes_at_radius[r];
    hex_coord_t const * const offsets =
        hc.x % 2 == 0 ? even_neighbor_offsets : odd_neighbor_offsets;

    for (int i = 0; i < n; ++i) {
        hex_coord_t const offset = offsets[i];
        hex_coord_t const hc_i = { hc.x + offset.x, hc.y + offset.y };
        if (on_map(hc_i, width, height))
            retval.push_back(hc_i);
    }

    return retval;
}
