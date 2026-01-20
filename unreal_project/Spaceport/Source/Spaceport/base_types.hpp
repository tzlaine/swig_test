// WARNING: Generated code.
// This file was generated from base_types (proto3)
#pragma once

#include "config.hpp"


#include <flags.hpp>

#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>




enum class mission_t {
    invalid_mission = 0,
    idle = 1,
    exercise = 2,
    explore = 3,
    intercept_in_system = 4,
    protect_system_location = 5,
    hunt_enemy_fleets = 6,
    guard_supply = 7,
    attack_settlement_defenses = 8,
    attack_settlement_infrastructure = 9,
    attack_settlement_population = 10,
    blockade_settlement = 11,
    found_settlement = 64,
    expand_settlement = 65,
    deliver_divisions = 66,
    deliver_mobile_repair_yard = 67,
    deliver_mobile_base = 68,
    build_outpost = 69,
    build_starbase = 70,
    upgrade_base = 71,
};
inline auto operator<=>(mission_t x, mission_t y) { return (int)x <=> (int)y; }

struct hex_coord_t
{
    int x = -1;
    int y = -1;
    bool operator==(hex_coord_t const &) const = default;
};


