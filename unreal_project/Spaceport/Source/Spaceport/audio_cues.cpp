#include "ui_defaults.h"

#include "Aplayer_controller_base.h"
#include "utility.hpp"


bool have_audio_cues() { return !!::world() && !!player_controller_base(); }

Uaudio_cues_t const & audio_cues()
{
    auto * pc = player_controller_base();
    check(pc);
    return pc->audio_cues();
}
