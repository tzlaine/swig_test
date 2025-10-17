// WARNING: Generated code.
// This file was generated from game_data (proto3)
#pragma once

#include <game_data.pb.h>
#include <base_types.hpp>

#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>




struct unit_design_t
{
    int id;
    int owner;
    int hull;
    int armor;
    int propulsion;
    int weapons;
    int shields;
    int detection;
    int stealth;
    int automation;
    int attack;
    int defense;
    int ground_attack;
};

struct unit_t
{
    int design_id;
    int design_owner;
    int health;
};

struct fleet_t
{
    unsigned int id;
    mission_t mission;
    std::vector<unit_t> units;
    float fuel;
    int rounds;
    int missiles;
    int fighters;
};

struct fleets_t
{
    boost::container::flat_map<int, fleet_t> fleets;
};

struct planet_t
{
    int system_id;
    int water;
    int food;
    int enery;
    int metal;
    int fuel;
    int population;
    int infrastructure;
    int max_population;
    int owner;
    int original_owner;
    fleet_t garrison;
};

struct location_object_t
{
    fleet_t bases;
    planet_t planet;
};

struct system_location_t
{
    std::vector<location_object_t> objects;
    fleets_t units;
};

struct system_t
{
    adobe::name_t name;
    hex_coord_t coord;
    std::vector<system_location_t> permanent_locations;
    std::vector<system_location_t> temporary_locations;
};

struct hex_t
{
    hex_coord_t coord;
    int province_id;
    std::vector<system_t> systems;
    fleets_t fleets;
};

struct province_t
{
    int id;
    int owner;
    std::vector<hex_coord_t> hex_coords;
};

struct nation_t
{
    int id;
    std::vector<unit_design_t> unit_designs;
    std::vector<province_t> provinces;
    std::vector<unsigned int> fleets;
    std::vector<int> planets;
    bool defeated;
};

struct game_state_t
{
    int map_width;
    int map_height;
    std::vector<hex_t> hexes;
    std::vector<system_t> systems;
    std::vector<planet_t> planets;
    std::vector<nation_t> nations;
};



pb_message::game_data::unit_design_t to_protobuf (const ::unit_design_t& value);
::unit_design_t from_protobuf (const pb_message::game_data::unit_design_t& msg);

pb_message::game_data::unit_t to_protobuf (const ::unit_t& value);
::unit_t from_protobuf (const pb_message::game_data::unit_t& msg);

pb_message::game_data::fleet_t to_protobuf (const ::fleet_t& value);
::fleet_t from_protobuf (const pb_message::game_data::fleet_t& msg);

pb_message::game_data::fleets_t to_protobuf (const ::fleets_t& value);
::fleets_t from_protobuf (const pb_message::game_data::fleets_t& msg);

pb_message::game_data::planet_t to_protobuf (const ::planet_t& value);
::planet_t from_protobuf (const pb_message::game_data::planet_t& msg);

pb_message::game_data::location_object_t to_protobuf (const ::location_object_t& value);
::location_object_t from_protobuf (const pb_message::game_data::location_object_t& msg);

pb_message::game_data::system_location_t to_protobuf (const ::system_location_t& value);
::system_location_t from_protobuf (const pb_message::game_data::system_location_t& msg);

pb_message::game_data::system_t to_protobuf (const ::system_t& value);
::system_t from_protobuf (const pb_message::game_data::system_t& msg);

pb_message::game_data::hex_t to_protobuf (const ::hex_t& value);
::hex_t from_protobuf (const pb_message::game_data::hex_t& msg);

pb_message::game_data::province_t to_protobuf (const ::province_t& value);
::province_t from_protobuf (const pb_message::game_data::province_t& msg);

pb_message::game_data::nation_t to_protobuf (const ::nation_t& value);
::nation_t from_protobuf (const pb_message::game_data::nation_t& msg);

pb_message::game_data::game_state_t to_protobuf (const ::game_state_t& value);
::game_state_t from_protobuf (const pb_message::game_data::game_state_t& msg);
