#pragma once

#include "game_data.hpp"
#include "hex_operations.hpp"
#include "concurrent_queue.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/container/flat_set.hpp>

#include <filesystem>
#include <fstream>


struct game_data_t
{
    game_data_t();

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
            reset_save_state save_state_reseter(save_state);

            pb_message::game_data::game_state_t as_protobuf =
                to_protobuf(*save_state);

            serialized_bytes_.clear();
            as_protobuf.SerializeToString(&serialized_bytes_);

            std::ofstream ofs(path, std::ios::binary);
            if (!ofs) {
                // log and return false on failure.
            }
            ofs.write(serialized_bytes_.data(), serialized_bytes_.size());
            if (!ofs) {
                // log and return false on failure.
            }
        }
    }
};
