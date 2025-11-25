#include "Amap_actor_base.h"

#include "game_data.hpp"


Amap_actor_base::Amap_actor_base() {}

double detection_dist_sq(
    game_state_t const & gs,
    Amap_actor_base const * detector,
    Amap_actor_base const * other_actor)
{
    // TODO: Detection logic goes here.  NOTE: Must match logic in model.hpp!
    double dist = 1.0;
    return dist * dist;
}
