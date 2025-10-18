#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "game_data_t.hpp"


namespace {
    // TODO
}

game_data_t::game_data_t () : save_thread_(&game_data_t::save_worker, this) {}
