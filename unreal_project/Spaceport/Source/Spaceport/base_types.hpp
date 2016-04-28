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
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7,
    I = 8,
    J1 = 9,
    J2 = 10,
    K1 = 11,
    K2 = 12,
    L = 13,
    M = 14,
    N = 15,
    O = 16,
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



message::turn_t to_protobuf (const ::turn_t& value);
::turn_t from_protobuf (const message::turn_t& msg);

void to_bin (const ::turn_t& value, std::vector<unsigned char>& bin);
::turn_t turn_t_from_bin (unsigned char*& bin);

message::hex_coord_t to_protobuf (const ::hex_coord_t& value);
::hex_coord_t from_protobuf (const message::hex_coord_t& msg);

void to_bin (const ::hex_coord_t& value, std::vector<unsigned char>& bin);
::hex_coord_t hex_coord_t_from_bin (unsigned char*& bin);
