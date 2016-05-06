// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "game_data.hpp"
#include <base_types.hpp>






pb_message::game_data::unit_t to_protobuf (const ::unit_t& value)
{
    pb_message::game_data::unit_t retval;
    retval.set_unit_id(value.unit_id);
    retval.set_owner(value.owner);
    retval.set_original_owner(value.original_owner);
    retval.set_fighters(value.fighters);
    retval.set_pfs(value.pfs);
    retval.set_tug_mission(static_cast< pb_message::tug_mission_t >(value.tug_mission));
    return retval;
}

::unit_t from_protobuf (const pb_message::game_data::unit_t& msg)
{
    ::unit_t retval;
    retval.unit_id = msg.unit_id();
    retval.owner = msg.owner();
    retval.original_owner = msg.original_owner();
    retval.fighters = msg.fighters();
    retval.pfs = msg.pfs();
    retval.tug_mission = static_cast<std::remove_reference<decltype(retval.tug_mission)>::type>(msg.tug_mission());
    return retval;
}

pb_message::game_data::fleet_t to_protobuf (const ::fleet_t& value)
{
    pb_message::game_data::fleet_t retval;
    for (const auto& x : value.units) {
        retval.add_units()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::fleet_t from_protobuf (const pb_message::game_data::fleet_t& msg)
{
    ::fleet_t retval;
    {
        retval.units.resize(msg.units_size());
        auto it = retval.units.begin();
        for (const auto& x : msg.units()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

pb_message::game_data::fleets_t to_protobuf (const ::fleets_t& value)
{
    pb_message::game_data::fleets_t retval;
    for (const auto& x : value.fleets) {
        (*retval.mutable_fleets())[x.first] = to_protobuf(x.second);
    }
    return retval;
}

::fleets_t from_protobuf (const pb_message::game_data::fleets_t& msg)
{
    ::fleets_t retval;
    {
        for (const auto& x : msg.fleets()) {
            retval.fleets[x.first] = from_protobuf(x.second);
        }
    }
    return retval;
}

pb_message::game_data::planet_t to_protobuf (const ::planet_t& value)
{
    pb_message::game_data::planet_t retval;
    retval.set_owner(value.owner);
    retval.set_original_owner(value.original_owner);
    retval.set_type(static_cast< pb_message::game_data::planet_t::type_t >(value.type));
    retval.mutable_units()->CopyFrom(to_protobuf(value.units));
    return retval;
}

::planet_t from_protobuf (const pb_message::game_data::planet_t& msg)
{
    ::planet_t retval;
    retval.owner = msg.owner();
    retval.original_owner = msg.original_owner();
    retval.type = static_cast<std::remove_reference<decltype(retval.type)>::type>(msg.type());
    retval.units = from_protobuf(msg.units());
    return retval;
}

pb_message::game_data::hex_zone_fixture_t to_protobuf (const ::hex_zone_fixture_t& value)
{
    pb_message::game_data::hex_zone_fixture_t retval;
    retval.set_type(static_cast< pb_message::game_data::hex_zone_fixture_t::type_t >(value.type));
    retval.mutable_base()->CopyFrom(to_protobuf(value.base));
    retval.mutable_planet()->CopyFrom(to_protobuf(value.planet));
    return retval;
}

::hex_zone_fixture_t from_protobuf (const pb_message::game_data::hex_zone_fixture_t& msg)
{
    ::hex_zone_fixture_t retval;
    retval.type = static_cast<std::remove_reference<decltype(retval.type)>::type>(msg.type());
    retval.base = from_protobuf(msg.base());
    retval.planet = from_protobuf(msg.planet());
    return retval;
}

pb_message::game_data::hex_zone_t to_protobuf (const ::hex_zone_t& value)
{
    pb_message::game_data::hex_zone_t retval;
    retval.set_name(value.name);
    for (const auto& x : value.fixtures) {
        retval.add_fixtures()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::hex_zone_t from_protobuf (const pb_message::game_data::hex_zone_t& msg)
{
    ::hex_zone_t retval;
    retval.name = msg.name();
    {
        retval.fixtures.resize(msg.fixtures_size());
        auto it = retval.fixtures.begin();
        for (const auto& x : msg.fixtures()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

pb_message::game_data::hex_t to_protobuf (const ::hex_t& value)
{
    pb_message::game_data::hex_t retval;
    retval.mutable_coord()->CopyFrom(to_protobuf(value.coord));
    retval.set_province_id(value.province_id);
    for (const auto& x : value.zones) {
        retval.add_zones()->CopyFrom(to_protobuf(x));
    }
    retval.mutable_fleets()->CopyFrom(to_protobuf(value.fleets));
    return retval;
}

::hex_t from_protobuf (const pb_message::game_data::hex_t& msg)
{
    ::hex_t retval;
    retval.coord = from_protobuf(msg.coord());
    retval.province_id = msg.province_id();
    {
        retval.zones.resize(msg.zones_size());
        auto it = retval.zones.begin();
        for (const auto& x : msg.zones()) {
            *it++ = from_protobuf(x);
        }
    }
    retval.fleets = from_protobuf(msg.fleets());
    return retval;
}

pb_message::game_data::province_t to_protobuf (const ::province_t& value)
{
    pb_message::game_data::province_t retval;
    retval.set_id(value.id);
    retval.set_owner(value.owner);
    retval.set_original_owner(value.original_owner);
    return retval;
}

::province_t from_protobuf (const pb_message::game_data::province_t& msg)
{
    ::province_t retval;
    retval.id = msg.id();
    retval.owner = msg.owner();
    retval.original_owner = msg.original_owner();
    return retval;
}

pb_message::game_data::map_t to_protobuf (const ::map_t& value)
{
    pb_message::game_data::map_t retval;
    retval.set_width(value.width);
    retval.set_height(value.height);
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(to_protobuf(x));
    }
    for (const auto& x : value.provinces) {
        retval.add_provinces()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::map_t from_protobuf (const pb_message::game_data::map_t& msg)
{
    ::map_t retval;
    retval.width = msg.width();
    retval.height = msg.height();
    {
        retval.hexes.resize(msg.hexes_size());
        auto it = retval.hexes.begin();
        for (const auto& x : msg.hexes()) {
            *it++ = from_protobuf(x);
        }
    }
    {
        retval.provinces.resize(msg.provinces_size());
        auto it = retval.provinces.begin();
        for (const auto& x : msg.provinces()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

pb_message::game_data::offmap_area_t to_protobuf (const ::offmap_area_t& value)
{
    pb_message::game_data::offmap_area_t retval;
    retval.set_owner(value.owner);
    retval.mutable_fleets()->CopyFrom(to_protobuf(value.fleets));
    return retval;
}

::offmap_area_t from_protobuf (const pb_message::game_data::offmap_area_t& msg)
{
    ::offmap_area_t retval;
    retval.owner = msg.owner();
    retval.fleets = from_protobuf(msg.fleets());
    return retval;
}

pb_message::game_data::offmap_areas_t to_protobuf (const ::offmap_areas_t& value)
{
    pb_message::game_data::offmap_areas_t retval;
    for (const auto& x : value.areas) {
        (*retval.mutable_areas())[x.first] = to_protobuf(x.second);
    }
    return retval;
}

::offmap_areas_t from_protobuf (const pb_message::game_data::offmap_areas_t& msg)
{
    ::offmap_areas_t retval;
    {
        for (const auto& x : msg.areas()) {
            retval.areas[x.first] = from_protobuf(x.second);
        }
    }
    return retval;
}

pb_message::game_data::team_t to_protobuf (const ::team_t& value)
{
    pb_message::game_data::team_t retval;
    retval.set_name(value.name);
    for (const auto& x : value.nations) {
        retval.add_nations(x);
    }
    return retval;
}

::team_t from_protobuf (const pb_message::game_data::team_t& msg)
{
    ::team_t retval;
    retval.name = msg.name();
    {
        retval.nations.resize(msg.nations_size());
        auto it = retval.nations.begin();
        for (const auto& x : msg.nations()) {
            *it++ = x;
        }
    }
    return retval;
}
