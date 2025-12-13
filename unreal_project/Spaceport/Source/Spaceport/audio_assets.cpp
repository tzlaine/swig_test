#include "ui_defaults.h"

#include "Aplayer_controller.h"
#include "utility.hpp"


bool have_audio_assets() { return !!::world() && !!player_controller(); }

Uaudio_assets_t const & audio_assets()
{
    auto * pc = player_controller();
    check(pc);
    return pc->audio_assets();
}
