#include "materials.h"

#include "Aplayer_controller.h"
#include "utility.hpp"


Umaterials_t const & materials()
{
    auto * pc = player_controller();
    check(pc);
    return pc->materials();
}
