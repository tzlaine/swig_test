#ifndef BUILD_FOR_TEST
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

void model::generate_galaxy(game_start_params_t const & params,
                                  concurrent_queue<int> & percent_complete,
                                  std::atomic_bool & fully_complete)
{
    game_state_.reset(new game_state_t);
    generation::generate_galaxy(
        params, *game_state_, nullptr, &percent_complete, &fully_complete);
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
