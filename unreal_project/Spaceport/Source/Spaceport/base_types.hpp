// WARNING: Generated code.
// This file was generated from base_types (proto3)
#pragma once

#include <base_types.pb.h>


#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>




enum class season_t {
    spring = 0,
    fall = 1,
};

enum class tug_mission_t {
    no_mission = 0,
    A_battle = 1,
    B_carrier = 2,
    C_mobile_base = 3,
    D_supply = 4,
    E_repair = 5,
    F_tow_frd = 6,
    G_pft = 7,
    H_economic_transfer = 8,
    I_crippled = 9,
    J1_base_repair = 10,
    J2_base_upgrade = 11,
    K1_pdu_transport = 12,
    K2_pdu_upgrade = 13,
    L_drone = 14,
    M_vanilla = 15,
    O_hydran_special_supply = 16,
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
