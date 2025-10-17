// WARNING: Generated code.
// This file was generated from base_types (proto3)
#pragma once

#include <base_types.pb.h>


#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>




enum class mission_t {
    no_mission = 0,
    explore = 1,
    attack = 2,
    evade = 3,
    intercept = 4,
    convoy_guard = 5,
    ground_attack = 6,
    privateer = 7,
    found_coloy = 8,
    expand_colony = 9,
    deliver_armies = 64,
    deliver_repair_yard = 65,
    deliver_mobile_base = 66,
    build_outpost = 67,
    build_starbase = 68,
    upgrade_base = 69,
};

struct hex_coord_t
{
    int x;
    int y;
};



pb_message::hex_coord_t to_protobuf (const ::hex_coord_t& value);
::hex_coord_t from_protobuf (const pb_message::hex_coord_t& msg);
