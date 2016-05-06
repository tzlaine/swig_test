// WARNING: Generated code.
// This file was generated from base_types (proto3)
#pragma once

#include <base_types.pb.h>


#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>




enum class season_t {
    spring = 0,
    fall = 1,
};

enum class tug_mission_t {
    no_mission = 0,
    A = 1,
    B = 2,
    C = 3,
    D = 4,
    E = 5,
    F = 6,
    G = 7,
    H = 8,
    I = 9,
    J1 = 10,
    J2 = 11,
    K1 = 12,
    K2 = 13,
    L = 14,
    M = 15,
    N = 16,
    O = 17,
};

enum class carrier_type_t {
    not_a_carrier = 0,
    single_ship_carrier = 1,
    light_carrier = 2,
    medium_carrier = 3,
    heavy_carrier = 4,
};

enum class escort_type_t {
    not_an_escort = 0,
    light_escort = 1,
    heavy_escort = 2,
};

enum class war_footing_t {
    war = 0,
    limited_war = 1,
    peace = 2,
};

struct turn_t
{
    int year;
    season_t season;
};

struct hex_coord_t
{
    int x;
    int y;
};



pb_message::turn_t to_protobuf (const ::turn_t& value);
::turn_t from_protobuf (const pb_message::turn_t& msg);

pb_message::hex_coord_t to_protobuf (const ::hex_coord_t& value);
::hex_coord_t from_protobuf (const pb_message::hex_coord_t& msg);
