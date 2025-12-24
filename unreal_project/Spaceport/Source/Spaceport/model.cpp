#if defined(BUILD_FOR_TEST)
#define INSTRUMENT_MODEL_GALAXY_GENERATION 0
#if INSTRUMENT_MODEL_GALAXY_GENERATION
#include <format>
#include <iostream>
#endif
#else
#include "Spaceport.h"
#endif

#include "model.hpp"
#include "serialization.hpp"
#include "generate_galaxy.hpp"
#include "rng.hpp"


client_view::client_view(std::span<std::byte const> src)
{
    hexes_.clear();
    systems_.clear();
    planets_.clear();
    nations_.clear();
    detail::deserialize_for_client(
        map_width_, map_height_, hexes_, systems_, planets_, nations_, src);
}

boost::optional<hex_t const &> client_view::hex(int i) const
{
    auto const it = std::ranges::lower_bound(
        hexes_, i, std::ranges::less{}, &indexed_object<hex_t>::index_);
    if (it == hexes_.end() || it->index_ != i)
        return {};
    return it->object_;
}

boost::optional<system_t const &> client_view::system(int i) const
{
    auto const it = std::ranges::lower_bound(
        systems_, i, std::ranges::less{}, &indexed_object<system_t>::index_);
    if (it == systems_.end() || it->index_ != i)
        return {};
    return it->object_;
}

boost::optional<planet_t const &> client_view::planet(int i) const
{
    auto const it = std::ranges::lower_bound(
        planets_, i, std::ranges::less{}, &indexed_object<planet_t>::index_);
    if (it == planets_.end() || it->index_ != i)
        return {};
    return it->object_;
}

boost::optional<nation_t const &> client_view::nation(int i) const
{
    auto const it = std::ranges::lower_bound(
        nations_, i, std::ranges::less{}, &indexed_object<nation_t>::index_);
    if (it == nations_.end() || it->index_ != i)
        return {};
    return it->object_;
}


model::model () : save_thread_(&model::save_worker, this) {}

model::~model ()
{
    save_queue_.done();
}

void model::generate_galaxy(
    game_start_params_t const & params,
    concurrent_queue<int> & percent_complete,
    std::atomic_bool & fully_complete)
{
    game_state_.reset(new game_state_t);
    generation::generate_galaxy(
        params, *game_state_, &percent_complete, &fully_complete);
}

void model::generate_after_galaxy(game_start_params_t const & params)
{
    generation::generate_nations(params, *game_state_);
    game_state_->alliances.resize(
        detail::blocks_needed_for_nations(game_state_->nations.size()));
    proximity_grid_ = proximity_grid<fleet_t const >(*game_state_);
    repopulate_grid();
#if INSTRUMENT_MODEL_GALAXY_GENERATION
    std::cout << std::format(
        "Galaxy generated; w={}, h={}\n",
        game_state_->map_width,
        game_state_->map_height);
    std::cout << std::format(
        " {} hexes, occupying {}MB\n",
        game_state_->hexes.size(),
        serialized_size(game_state_->hexes) / 1024.0 / 1024.0);
    std::cout << std::format(
        " {} systems, occupying {}MB\n",
        game_state_->systems.size(),
        serialized_size(game_state_->systems) / 1024.0 / 1024.0);
    std::cout << std::format(
        " {} planets, occupying {}MB\n",
        game_state_->planets.size(),
        serialized_size(game_state_->planets) / 1024.0 / 1024.0);
    std::cout << std::format(
        " {} nations, occupying {}MB\n",
        game_state_->nations.size(),
        serialized_size(game_state_->nations) / 1024.0 / 1024.0);
#endif
}

void model::day_tick()
{
}

void model::month_tick()
{
}

void model::year_tick()
{
}
