#pragma once

struct game_start_params;
struct game_state_t;

void generate_galaxy(game_start_params const & params,
                     game_state_t & game_state);
