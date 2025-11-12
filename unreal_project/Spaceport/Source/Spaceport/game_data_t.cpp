#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "game_data_t.hpp"
#include "generate_galaxy.hpp"
#include "rng.hpp"


game_data_t::game_data_t () : save_thread_(&game_data_t::save_worker, this) {}

game_data_t::~game_data_t ()
{
    save_queue_.done();
}

void game_data_t::generate_galaxy(game_start_params_t const & params,
                                  concurrent_queue<int> & percent_complete,
                                  std::atomic_bool & fully_complete)
{
    game_state_.reset(new game_state_t);
    generation::generate_galaxy(
        params, *game_state_, nullptr, &percent_complete, &fully_complete);
}

void game_data_t::day_tick()
{
}

void game_data_t::month_tick()
{
}

void game_data_t::year_tick()
{
}
