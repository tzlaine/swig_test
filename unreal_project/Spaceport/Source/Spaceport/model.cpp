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
#include "generate_galaxy.hpp"
#include "rng.hpp"


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
        params, *game_state_, nullptr, &percent_complete, &fully_complete);
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
