#include "ui_defaults.h"

#include "Aplayer_controller_base.h"
#include "utility.hpp"


Uui_defaults_t const & ui_defaults()
{
    auto * pc = player_controller_base();
    check(pc);
    return pc->ui_defaults();
}
