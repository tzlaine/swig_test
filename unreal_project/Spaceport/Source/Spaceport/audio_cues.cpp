#include "ui_defaults.h"

#include "Aplayer_controller_base.h"
#include "utility.hpp"


Uaudio_cues_t const & audio_cues()
{
    auto * pc = player_controller_base();
    check(pc);
    return pc->audio_cues();
}
