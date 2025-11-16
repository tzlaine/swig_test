#pragma once

#include "concurrent_queue.hpp"
#include "constants.hpp"
#include "game_data.hpp"
#include "hex_operations.hpp"
#include "logging.hpp"
#include "map_util.hpp"

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

private:
    // This must be called at the top of every mutating member function.
    void copy_before_write()
    {
        if (game_state_.unique())
            return;
        game_state_ =
            boost::shared_ptr<game_state_t>(new game_state_t (*game_state_));
    }

    boost::shared_ptr<game_state_t> game_state_;

    std::atomic_int saving_{0};
    std::string serialized_bytes_; // to be used in the save thread *only*

    struct reset_saving
    {
        reset_saving(std::atomic_int & saving) : saving_(&saving) {}
        ~reset_saving()  { *saving_ = 0; }
        std::atomic_int * saving_ = nullptr;
    };
    struct reset_save_state
    {
        reset_save_state(boost::shared_ptr<game_state_t const> & save_state) :
            save_state_(&save_state)
        {}
        ~reset_save_state()  { save_state_->reset(); }
        boost::shared_ptr<game_state_t const> * save_state_ = nullptr;
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

            pb_message::game_data::game_state_t as_protobuf;
            {
                reset_save_state save_state_reseter(save_state);
                as_protobuf = to_protobuf(*save_state);
            }

            serialized_bytes_.clear();
            as_protobuf.SerializeToString(&serialized_bytes_);

            std::ofstream ofs(path, std::ios::binary);
            if (!ofs) {
                UE_LOG(general, Error,
                       TEXT("Unable to open save file %s for writing."), *FSTRINGIZE(path));
            }
            ofs.write(serialized_bytes_.data(), serialized_bytes_.size());
            if (!ofs) {
                UE_LOG(general, Error,
                       TEXT("Unable to write contents to save file %s."), *FSTRINGIZE(path));
            }
        }
    }
};
