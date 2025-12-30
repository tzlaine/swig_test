#include "rng.hpp"


namespace detail {
    // TODO: Pretty sure use of this global from many galaxy gen threads races.
    rng_state g_rng_state;
}
