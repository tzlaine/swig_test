#pragma once


enum struct map_mode : int {
    galaxy_map,
    system_map,
    transition_to_galaxy_map,
    transition_to_system_map
};

struct map_transition_state;
