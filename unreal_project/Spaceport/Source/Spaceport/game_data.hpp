// WARNING: Generated code.
// This file was generated from game_data (proto3)
#pragma once

#include <game_data.pb.h>
#include <base_types.hpp>

#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>




struct unit_t
{
    int unit_id;
    int owner;
    int original_owner;
    tug_mission_t tug_mission;
};

struct fleet_t
{
    std::vector<unit_t> units;
};

struct fleets_t
{
    boost::container::flat_map<int, fleet_t> fleets;
};

struct planet_t
{

    enum class type_t {
        minor = 0,
        major = 1,
        capital = 2,
    };
    int owner;
    int original_owner;
    planet_t::type_t type;
    fleets_t units;
};

struct hex_zone_fixture_t
{

    enum class type_t {
        type_base = 0,
        type_planet = 1,
    };
    hex_zone_fixture_t::type_t type;
    unit_t base;
    planet_t planet;
};

struct hex_zone_t
{
    std::string name;
    std::vector<hex_zone_fixture_t> fixtures;
};

struct hex_t
{
    hex_coord_t coord;
    int province_id;
    std::vector<hex_zone_t> zones;
    fleets_t fleets;
};

struct province_t
{
    int id;
    int owner;
    int original_owner;
};

struct map_t
{
    int width;
    int height;
    std::vector<hex_t> hexes;
    std::vector<province_t> provinces;
};

struct offmap_area_t
{
    int owner;
    fleets_t fleets;
};

struct offmap_areas_t
{
    boost::container::flat_map<int, offmap_area_t> areas;
};



pb_message::game_data::unit_t to_protobuf (const ::unit_t& value);
::unit_t from_protobuf (const pb_message::game_data::unit_t& msg);

pb_message::game_data::fleet_t to_protobuf (const ::fleet_t& value);
::fleet_t from_protobuf (const pb_message::game_data::fleet_t& msg);

pb_message::game_data::fleets_t to_protobuf (const ::fleets_t& value);
::fleets_t from_protobuf (const pb_message::game_data::fleets_t& msg);

pb_message::game_data::planet_t to_protobuf (const ::planet_t& value);
::planet_t from_protobuf (const pb_message::game_data::planet_t& msg);

pb_message::game_data::hex_zone_fixture_t to_protobuf (const ::hex_zone_fixture_t& value);
::hex_zone_fixture_t from_protobuf (const pb_message::game_data::hex_zone_fixture_t& msg);

pb_message::game_data::hex_zone_t to_protobuf (const ::hex_zone_t& value);
::hex_zone_t from_protobuf (const pb_message::game_data::hex_zone_t& msg);

pb_message::game_data::hex_t to_protobuf (const ::hex_t& value);
::hex_t from_protobuf (const pb_message::game_data::hex_t& msg);

pb_message::game_data::province_t to_protobuf (const ::province_t& value);
::province_t from_protobuf (const pb_message::game_data::province_t& msg);

pb_message::game_data::map_t to_protobuf (const ::map_t& value);
::map_t from_protobuf (const pb_message::game_data::map_t& msg);

pb_message::game_data::offmap_area_t to_protobuf (const ::offmap_area_t& value);
::offmap_area_t from_protobuf (const pb_message::game_data::offmap_area_t& msg);

pb_message::game_data::offmap_areas_t to_protobuf (const ::offmap_areas_t& value);
::offmap_areas_t from_protobuf (const pb_message::game_data::offmap_areas_t& msg);
