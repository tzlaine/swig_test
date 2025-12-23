#pragma once

#include "check.hpp"
#include "concurrent_queue.hpp"
#include "constants.hpp"
#include "game_data.hpp"
#include "hex_operations.hpp"
#include "logging.hpp"
#include "map_util.hpp"
#include "game_data_serialization.hpp"
#include "proximity_grid.hpp"


#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/container/flat_set.hpp>

#include <filesystem>
#include <fstream>


namespace detail {
    inline double plus_minus_to_sigma(double plus_minus)
    {
        return plus_minus / 3.0;
    }

#if defined(BUILD_FOR_TEST)
    inline double g_testing_detection_dist = 1.0;
#endif
}

// obligations of proximity_grid
inline int owner(fleet_t const * f) { return f->id.nation_id; }
inline point_2d position(fleet_t const * f)
{
    return {f->position.world_pos_x, f->position.world_pos_y};
}
inline double detection_dist_sq(
    game_state_t const & gs,
    fleet_t const * detector,
    fleet_t const * other_fleet)
{
#if defined(BUILD_FOR_TEST)
    return detail::g_testing_detection_dist * detail::g_testing_detection_dist;
#endif
    // TODO: Detection logic goes here.  NOTE: Must match logic in
    // Amap_actor_base.cpp!
    double dist = 1.0;
    return dist * dist;
}

struct client_view
{
    client_view() = default;
    client_view(std::span<std::byte const> src);

    int map_width() const { return map_width_; }
    int map_height() const { return map_height_; }

    boost::optional<hex_t const &> hex(int i) const;
    boost::optional<system_t const &> system(int i) const;
    boost::optional<planet_t const &> planet(int i) const;
    boost::optional<nation_t const &> nation(int i) const;

private:
    int map_width_ = 0;
    int map_height_ = 0;
    std::vector<indexed_object<hex_t>> hexes_;
    std::vector<indexed_object<system_t>> systems_;
    std::vector<indexed_object<planet_t>> planets_;
    std::vector<indexed_object<nation_t>> nations_;
};

struct model
{
    model();
    ~model();

    hex_t const & hex(hex_coord_t hc) const
    {
        auto const i = hex_index_t(hc, game_state_->map_width);
        check(i < (int)game_state_->hexes.size());
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
    TArray<uint8> serialize_for_client(int nation_id);
#endif

    proximity_grid<fleet_t const> & proximity() { return proximity_grid_; }

    void generate_galaxy(game_start_params_t const & params,
                         concurrent_queue<int> & percent_complete,
                         std::atomic_bool & fully_complete);
    void generate_after_galaxy(game_start_params_t const & params);

    // TODO: These should take a by-ref param that has a map of nation-ID ->
    // updates.  As each mutation to the game_state_t is done, it should be
    // added to the list for whichever nation(s) can see that mutation.  The
    // calling Agame_state should provide the map with only the nation-IDs of
    // the active human players.  It can then send the updates to the
    // associated player(s).
    void day_tick();
    void month_tick();
    void year_tick();

    void set_speed(int speed) { mutable_state().play_speed = speed; }

    void save(std::filesystem::path path)
    {
        // Ignore save requests that overlap with the current save.
        if (saving_.fetch_add(1) == 0)
            save_queue_.try_push(std::pair(game_state(), std::move(path)));
    }

    void load(std::filesystem::path const & path)
    {
        game_state_ = boost::shared_ptr<game_state_t>(new game_state_t{});
        deserialize_message(*game_state_, path);
        proximity_grid_ = proximity_grid<fleet_t const>(*game_state_);
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

    game_state_t & mutable_state()
    {
        copy_before_write();
        return *game_state_;
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
