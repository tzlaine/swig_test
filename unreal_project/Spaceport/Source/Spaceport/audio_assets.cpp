#include "ui_defaults.h"

#include "Aplayer_controller_base.h"
#include "utility.hpp"


bool have_audio_assets() { return !!::world() && !!player_controller_base(); }

Uaudio_assets_t const & audio_assets()
{
    auto * pc = player_controller_base();
    check(pc);
    return pc->audio_assets();
}
