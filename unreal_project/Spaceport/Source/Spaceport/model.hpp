#pragma once

#include "concurrent_queue.hpp"
#include "constants.hpp"
#include "game_data.hpp"
#include "hex_operations.hpp"
#include "logging.hpp"
#include "map_util.hpp"
#include "game_data_serialization.hpp"
#include "proximity_grid.hpp"


#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/container/flat_set.hpp>

#include <filesystem>
#include <fstream>


// TODO
constexpr static int small_map_height = 11;
constexpr static int medium_map_height = 21;
constexpr static int large_map_height = 41;

inline game_start_params_t default_game_start_params()
{
    return {
        .habitable_systems_per_hex_mean = 5.0f,
        .habitable_systems_per_hex_plus_minus = 2.0f,
        .systems_per_hex = default_systems_per_hex,
        .map_height = 11
    };
}

// TODO: Move to a utility header
inline double plus_minus_to_sigma(double plus_minus)
{
    return plus_minus / 3.0;
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

enum struct fleet_visitation { garrisons, no_garrisons };

template<typename GameState, typename F>
void visit_fleets(
    GameState & gs,
    F && f,
    int nation_id = nation_none,
    fleet_visitation garrisons = fleet_visitation::no_garrisons)
{
    if (garrisons == fleet_visitation::garrisons) {
        for (auto & planet : gs.planets) {
            if (nation_id == nation_none || planet.owner == nation_id)
                f(planet.garrison);
        }
    }

    auto const process_system_location = [&](auto & sys_loc) {
        for (auto & object : sys_loc.objects) {
            if (!object.bases.units.empty() && nation_id == nation_none ||
                object.bases.id.nation_id == nation_id) {
                f(object.bases);
            }
        }
        if (nation_id == nation_none) {
            for (auto & [nation, fleet] : sys_loc.units.fleets) {
                f(fleet);
            }
        } else {
            auto it = sys_loc.units.fleets.find(nation_id);
            if (it != sys_loc.units.fleets.end())
                f(it->second);
        }
    };
    for (auto & system : gs.systems) {
        for (auto & sys_loc : system.permanent_locations) {
            process_system_location(sys_loc);
        }
        for (auto & sys_loc : system.temporary_locations) {
            process_system_location(sys_loc);
        }
    }

    auto const process_map_fleets = [&](auto & map_fleets) {
        for (auto & fleet : map_fleets) {
            f(fleet);
        }
    };
    if (nation_id == nation_none) {
        for (auto & nation : gs.nations) {
            process_map_fleets(nation.map_fleets);
        }
    } else {
        process_map_fleets(gs.nations[nation_id].map_fleets);
    }
}

// obligations of proximity_grid
inline int owner(fleet_t const * f) { return f->owner; }
inline point_2d position(fleet_t const * f)
{
    return {f->world_pos_x, f->world_pos_y};
}
inline double detection_dist_sq(
    game_state_t const & gs,
    fleet_t const * detector,
    fleet_t const * other_fleet)
{
    double dist = 1.0; // TODO: Detection logic goes here.
    return dist * dist;
}


struct model
{
    model();
    ~model();

    hex_t const & hex(hex_coord_t hc) const
    {
        auto const i = hex_index_t(hc, game_state_->map_width);
        assert(i < (int)game_state_->hexes.size());
        return game_state_->hexes[i];
    }

    std::vector<hex_t> const & hexes() const
    {
        return game_state_->hexes;
    }

    std::vector<system_t> const & systems() const
    {
        return game_state_->systems;
    }

    std::vector<planet_t> const & planets() const
    {
        return game_state_->planets;
    }

    std::vector<nation_t> const & nations() const
    {
        return game_state_->nations;
    }

    boost::shared_ptr<game_state_t const> game_state() const
    {
        return {game_state_};
    }

#if !defined(BUILD_FOR_TEST)
    TArray<uint8> serialize_for_client(int nation_id) const;
#endif

    void generate_galaxy(game_start_params_t const & params,
                         concurrent_queue<int> & percent_complete,
                         std::atomic_bool & fully_complete);

    void day_tick();
    void month_tick();
    void year_tick();

    void save(std::filesystem::path path)
    {
        // Ignore save requests that overlap with the current save.
        if (saving_.fetch_add(1) == 0)
            save_queue_.try_push(std::pair(game_state(), std::move(path)));
    }

    void load(std::filesystem::path const & path)
    {
        game_state_ = boost::shared_ptr<game_state_t>(new game_state_t);
        deserialize_message(*game_state_, path);
        proximity_grid_ = proximity_grid<fleet_t const>(
            world_map_extent(*game_state_),
            max_detection_radius_before_stealth(*game_state_));
        repopulate_grid();
    }

private:
    // This must be called at the top of every mutating member function.
    void copy_before_write()
    {
        if (game_state_.unique())
            return;
        game_state_ =
            boost::shared_ptr<game_state_t>(new game_state_t (*game_state_));
        repopulate_grid();
    }

    boost::shared_ptr<game_state_t> game_state_;

    proximity_grid<fleet_t const> proximity_grid_;
    void repopulate_grid()
    {
        proximity_grid_.clear_pointers();
        auto f = [this](auto & fleet) { proximity_grid_.insert(fleet); };
        visit_fleets(*game_state_, f);
    }

    std::atomic_int saving_{0};
    std::string serialized_bytes_; // to be used in the save thread *only*

    struct reset_saving
    {
        reset_saving(std::atomic_int & saving) : saving_(&saving) {}
        ~reset_saving()  { *saving_ = 0; }
        std::atomic_int * saving_ = nullptr;
    };

    concurrent_queue<
        std::pair<boost::shared_ptr<game_state_t const>,
                  std::filesystem::path>> save_queue_;
    std::jthread save_thread_;
    void save_worker()
    {
        std::pair<boost::shared_ptr<game_state_t const>, std::filesystem::path>
            save_state_and_path;
        while (save_queue_.pop(save_state_and_path)) {
            auto [save_state, path] = save_state_and_path;
            reset_saving saving_reseter(saving_);
            auto temp_path = path;
            temp_path += "_temp";
            serialize_message(*save_state, temp_path);
            std::filesystem::rename(temp_path, path);
        }
    }
};
