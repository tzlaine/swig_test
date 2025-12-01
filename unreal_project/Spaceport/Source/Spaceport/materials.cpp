#include "materials.h"

#include "Aplayer_controller_base.h"
#include "utility.hpp"


Umaterials_t const & materials()
{
    auto * pc = player_controller_base();
    check(pc);
    return pc->materials();
}
