// WARNING: Generated code.
// This file was generated from base_types (proto3)
#pragma once

#include "config.hpp"
PUSH_WARNING
DISABLE_WARNING(4996)
DISABLE_WARNING(4127)
DISABLE_WARNING(4100)
#include <base_types.pb.h>
POP_WARNING


#include <flags.hpp>

#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>




enum class mission_t {
    invalid_mission = 0,
    no_mission = 1,
    explore = 2,
    attack = 3,
    evade = 4,
    intercept = 5,
    convoy_guard = 6,
    ground_attack = 7,
    privateer = 8,
    found_coloy = 9,
    expand_colony = 10,
    deliver_armies = 64,
    deliver_repair_yard = 65,
    deliver_mobile_base = 66,
    build_outpost = 67,
    build_starbase = 68,
    upgrade_base = 69,
};
inline auto operator<=>(mission_t x, mission_t y) { return (int)x <=> (int)y; }

struct hex_coord_t
{
    int x = -1;
    int y = -1;
    bool operator==(hex_coord_t const &) const = default;
};



pb_message::hex_coord_t to_protobuf (const ::hex_coord_t& value);
::hex_coord_t from_protobuf (const pb_message::hex_coord_t& msg);
