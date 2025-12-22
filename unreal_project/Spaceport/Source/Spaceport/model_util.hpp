#pragma once

#include "check.hpp"
#include "game_data.hpp"
#include "lua.hpp"
#include "map_util.hpp"

#include <algorithm>
#include <span>
#include <vector>


inline auto
operator<=>(nation_and_object_id_t const & a, nation_and_object_id_t const & b)
{
    if (auto retval = a.nation_id <=> b.nation_id; retval != 0)
        return retval;
    return a.object_id <=> b.object_id;
}

enum struct fleet_visitation { garrisons, no_garrisons };

template<typename GameState, typename F>
void visit_fleets(
    GameState & gs,
    F && f,
    int nation_id = nation_none,
    fleet_visitation garrisons = fleet_visitation::no_garrisons)
{
    std::span<nation_t const> nations;
    if (nation_id == nation_none) {
        nations = std::span(gs.nations);
    } else {
        auto const first = gs.nations.data() + nation_id;
        nations = std::span(first, first + 1);
    }
    for (auto & nation : nations) {
        for (auto & fleet : nation.fleets) {
            if (!fleet.position.is_garrison ||
                garrisons == fleet_visitation::garrisons) {
                f(fleet);
            }
        }
    }
}

template<typename T>
struct indexed_object
{
    int index_;
    T object_;
};

inline auto ptr_to_id(fleet_t const * f) { return f->id; };

inline void sort_by_id(std::vector<fleet_t const *> & visible_fleets)
{
    std::ranges::sort(visible_fleets, std::ranges::less{}, ptr_to_id);
}

inline bool visible_fleet(
    std::vector<fleet_t const *> const & visible_fleets,
    nation_and_object_id_t fleet_id)
{
    return std::ranges::binary_search(
        visible_fleets, fleet_id, std::ranges::less{}, ptr_to_id);
}

inline constexpr nation_and_object_id_t invalid_nation_and_object{-1, -1};

inline bool invalid(fleet_t const & f)
{
    return f.id == invalid_nation_and_object;
}

inline game_start_params_t default_game_start_params()
{
    return lua()["default_game_start_params"];
}

inline box_2d world_map_extent(game_state_t const & game_state)
{
    point_2d hex_centers[4] = {
        hex_position(hex_coord_t{0, 0}, game_state.map_height),
        hex_position(
            hex_coord_t{game_state.map_width - 1, 0}, game_state.map_height),
        hex_position(
            hex_coord_t{0, game_state.map_height - 1}, game_state.map_height),
        hex_position(
            hex_coord_t{game_state.map_width - 1, game_state.map_height - 1},
            game_state.map_height)};

    if ((game_state.map_width - 1 + 1000) % 2 == 1)
        hex_centers[1].y += sin_60;

    double const min_x =
        std::ranges::min(hex_centers, std::less{}, &point_2d::x).x - 1.0;
    double const max_x =
        std::ranges::max(hex_centers, std::less{}, &point_2d::x).x + 1.0;
    double const min_y =
        std::ranges::min(hex_centers, std::less{}, &point_2d::y).y - sin_60;
    double const max_y =
        std::ranges::max(hex_centers, std::less{}, &point_2d::y).y + sin_60;

    return box_2d{point_2d{min_x, min_y}, point_2d{max_x, max_y}};
}

inline double max_detection_radius_before_stealth(game_state_t const & game_state)
{
    return 3.0; // TODO
}

namespace detail {
    using dynamic_bitset_block_type = uint32_t;
    inline int const dynamic_bitset_bits_per_block =
        std::numeric_limits<dynamic_bitset_block_type>::digits;

    template<bool Const>
    struct dynamic_bitset_t
    {
        using block_type = dynamic_bitset_block_type;
        static int const bits_per_block = dynamic_bitset_bits_per_block;

        dynamic_bitset_t() = default;
        dynamic_bitset_t(std::vector<block_type> const & blocks)
            requires(Const)
            : blocks_(&blocks)
        {}
        dynamic_bitset_t(std::vector<block_type> & blocks)
            requires(!Const)
            : blocks_(&blocks)
        {}

        bool get(std::ptrdiff_t i) const
        {
            block_type const mask = block_type(1) << (i % bits_per_block);
            return (*blocks_)[i / bits_per_block] & mask;
        }

        void set(std::ptrdiff_t i, bool b)
            requires(!Const)
        {
            block_type const mask = block_type(1) << (i % bits_per_block);
            if (b)
                (*blocks_)[i / bits_per_block] |= mask;
            else
                (*blocks_)[i / bits_per_block] &= ~mask;
        }

    private:
        using vec_type = std::vector<block_type>;
        std::conditional_t<Const, vec_type const *, vec_type *> blocks_;
    };

    template<bool Const>
    struct alliances_t
    {
        alliances_t() = default;
        alliances_t(std::vector<dynamic_bitset_block_type> const & bits)
            requires(Const)
            : allied_(bits)
        {}
        alliances_t(std::vector<dynamic_bitset_block_type> & bits)
            requires(!Const)
            : allied_(bits)
        {}

        bool allied(int nation_id_1, int nation_id_2) const
        {
            if (nation_id_2 < nation_id_1)
                std::swap(nation_id_1, nation_id_2);
            auto const i = nation_id_1 * (nation_id_1 + 1) / 2 + nation_id_2;
            return allied_.get(i);
        }

        void form_alliance(int nation_id_1, int nation_id_2)
            requires(!Const)
        {
            change_alliance(nation_id_1, nation_id_2, true);
        }

        void break_alliance(int nation_id_1, int nation_id_2)
            requires(!Const)
        {
            change_alliance(nation_id_1, nation_id_2, false);
        }

    private:
        void change_alliance(int nation_id_1, int nation_id_2, bool b)
        {
            if (nation_id_2 < nation_id_1)
                std::swap(nation_id_1, nation_id_2);
            auto const i = nation_id_1 * (nation_id_1 + 1) / 2 + nation_id_2;
            allied_.set(i, true);
        }

        dynamic_bitset_t<Const> allied_;
    };

    inline int bits_needed_for_nations(int n) { return n * (n + 1) / 2; }
    inline int blocks_needed_for_nations(int n)
    {
        return bits_needed_for_nations(n) / dynamic_bitset_bits_per_block +
                int(n % dynamic_bitset_bits_per_block != 0);
    }

    inline alliances_t<true>
    alliances(std::vector<dynamic_bitset_block_type> const & bits)
    {
        return {bits};
    }
    inline alliances_t<false> alliances(std::vector<dynamic_bitset_block_type> & bits)
    {
        return {bits};
    }
}

inline bool allied(game_state_t const & gs, int nation_id_1, int nation_id_2)
{
    return detail::alliances(gs.alliances).allied(nation_id_1, nation_id_2);
}

inline void form_alliance(game_state_t & gs, int nation_id_1, int nation_id_2)
{
    return detail::alliances(gs.alliances)
        .form_alliance(nation_id_1, nation_id_2);
}

inline void break_alliance(game_state_t & gs, int nation_id_1, int nation_id_2)
{
    return detail::alliances(gs.alliances)
        .break_alliance(
        nation_id_1, nation_id_2);
}

inline void
allies_of(std::vector<int> & retval, game_state_t const & gs, int nation_id)
{
    retval.clear();
    auto const alliances_ = detail::alliances(gs.alliances);
    for (int i = 0, last = (int)gs.nations.size(); i != last; ++i) {
        if (i == nation_id)
            continue;
        if (alliances_.allied(i, nation_id))
            retval.push_back(i);
    }
}

struct candidate_planet
{
    double score_;
    int planet_id_;
    planet_t * planet_;
};

template<typename F>
std::vector<candidate_planet> scored_planets(game_state_t & gs, F && f)
{
    std::vector<candidate_planet> retval(gs.planets.size());
    int i = 0;
    std::ranges::transform(gs.planets, retval.begin(), [&](auto & e) {
        return candidate_planet{f(e), i++, &e};
    });
    return retval;
}

inline void only_top_planets(std::vector<candidate_planet> & all_planets, int n)
{
    check(n < (int)all_planets.size());
    std::ranges::partial_sort(
        all_planets,
        all_planets.begin() + n,
        std::ranges::greater{},
        &candidate_planet::score_);
    all_planets.resize(n);
}
