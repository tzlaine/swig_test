// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "game_data.hpp"
#include <base_types.hpp>






namespace {

    bool bool_from_bin (unsigned char*& bin)
    {
        bool retval = static_cast<bool>(*bin);
        bin += 1;
        return retval;
    }

    int int_from_bin (unsigned char*& bin)
    {
        int retval =
            (bin[0] << 24) +
            (bin[1] << 16) +
            (bin[2] <<  8) +
            (bin[3] <<  0);
        bin += 4;
        return retval;
    }

    float float_from_bin (unsigned char*& bin)
    {
        float retval;
        memcpy(&retval, bin, sizeof(float));
        bin += sizeof(float);
        return retval;
    }

    double double_from_bin (unsigned char*& bin)
    {
        double retval;
        memcpy(&retval, bin, sizeof(double));
        bin += sizeof(double);
        return retval;
    }

    template <typename T>
    T enum_from_bin (unsigned char*& bin)
    {
        int i = int_from_bin(bin);
        return static_cast<T>(i);
    }

    std::string string_from_bin (unsigned char*& bin)
    {
        std::size_t length = int_from_bin(bin);
        std::string retval((const char*)bin, length);
        bin += length;
        return retval;
    }

    void to_bin (bool b, std::vector<unsigned char>& bin)
    {
        bin.push_back(static_cast<unsigned char>(b));
    }

    void to_bin (int i, std::vector<unsigned char>& bin)
    {
        bin.push_back(static_cast<unsigned char>((i >> 24) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >> 16) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >>  8) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >>  0) & 0xff));
    }

    void to_bin (float f, std::vector<unsigned char>& bin)
    {
        bin.resize(bin.size() + sizeof(float));
        memcpy(&bin[bin.size() - sizeof(float)], &f, sizeof(float));
    }

    void to_bin (double d, std::vector<unsigned char>& bin)
    {
        bin.resize(bin.size() + sizeof(double));
        memcpy(&bin[bin.size() - sizeof(double)], &d, sizeof(double));
    }

    template <typename T>
    void to_bin (T e, std::vector<unsigned char>& bin)
    {
        int i = static_cast<int>(e);
        to_bin(i, bin);
    }

    void to_bin (const std::string s, std::vector<unsigned char>& bin)
    {
        int length = static_cast<int>(s.size());
        to_bin(length, bin);
        bin.resize(bin.size() + length);
        std::copy(s.begin(), s.end(), bin.end() - length);
    }

}


message::game_data::unit_t to_protobuf (const ::unit_t& value)
{
    message::game_data::unit_t retval;
    retval.set_unit_id(value.unit_id);
    retval.set_owner(value.owner);
    retval.set_original_owner(value.original_owner);
    return retval;
}

::unit_t from_protobuf (const message::game_data::unit_t& msg)
{
    ::unit_t retval;
    retval.unit_id = msg.unit_id();
    retval.owner = msg.owner();
    retval.original_owner = msg.original_owner();
    return retval;
}

void to_bin (const ::unit_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.unit_id, bin);
    to_bin(value.owner, bin);
    to_bin(value.original_owner, bin);
}

::unit_t unit_t_from_bin (unsigned char*& bin)
{
    ::unit_t retval;
    retval.unit_id = int_from_bin(bin);
    retval.owner = int_from_bin(bin);
    retval.original_owner = int_from_bin(bin);
    return retval;
}

message::game_data::fleet_t to_protobuf (const ::fleet_t& value)
{
    message::game_data::fleet_t retval;
    for (const auto& x : value.units) {
        retval.add_units()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::fleet_t from_protobuf (const message::game_data::fleet_t& msg)
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

void to_bin (const ::fleet_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.units.size());
        to_bin(length, bin);
        for (const auto& x : value.units) {
            to_bin(x, bin);
        }
    }
}

::fleet_t fleet_t_from_bin (unsigned char*& bin)
{
    ::fleet_t retval;
    {
        int length = int_from_bin(bin);
        retval.units.resize(length);
        for (int i = 0; i < length; ++i) {
            auto x = unit_t_from_bin(bin);
            retval.units[i] = x;
        }
    }
    return retval;
}

message::game_data::fleets_t to_protobuf (const ::fleets_t& value)
{
    message::game_data::fleets_t retval;
    for (const auto& x : value.fleets) {
        (*retval.mutable_fleets())[x.first] = to_protobuf(x.second);
    }
    return retval;
}

::fleets_t from_protobuf (const message::game_data::fleets_t& msg)
{
    ::fleets_t retval;
    {
        for (const auto& x : msg.fleets()) {
            retval.fleets[x.first] = from_protobuf(x.second);
        }
    }
    return retval;
}

void to_bin (const ::fleets_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.fleets.size());
        to_bin(length, bin);
        for (const auto& x : value.fleets) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
}

::fleets_t fleets_t_from_bin (unsigned char*& bin)
{
    ::fleets_t retval;
    {
        int length = int_from_bin(bin);
        for (int i = 0; i < length; ++i) {
            auto key = int_from_bin(bin);
            auto value = fleet_t_from_bin(bin);
            retval.fleets[key] = value;
        }
    }
    return retval;
}

message::game_data::planet_t to_protobuf (const ::planet_t& value)
{
    message::game_data::planet_t retval;
    retval.set_owner(value.owner);
    retval.set_original_owner(value.original_owner);
    return retval;
}

::planet_t from_protobuf (const message::game_data::planet_t& msg)
{
    ::planet_t retval;
    retval.owner = msg.owner();
    retval.original_owner = msg.original_owner();
    return retval;
}

void to_bin (const ::planet_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.owner, bin);
    to_bin(value.original_owner, bin);
}

::planet_t planet_t_from_bin (unsigned char*& bin)
{
    ::planet_t retval;
    retval.owner = int_from_bin(bin);
    retval.original_owner = int_from_bin(bin);
    return retval;
}

message::game_data::hex_zone_fixture_t to_protobuf (const ::hex_zone_fixture_t& value)
{
    message::game_data::hex_zone_fixture_t retval;
    retval.set_type(static_cast< message::game_data::hex_zone_fixture_t::type_t >(value.type));
    retval.mutable_base()->CopyFrom(to_protobuf(value.base));
    retval.mutable_planet()->CopyFrom(to_protobuf(value.planet));
    return retval;
}

::hex_zone_fixture_t from_protobuf (const message::game_data::hex_zone_fixture_t& msg)
{
    ::hex_zone_fixture_t retval;
    retval.type = static_cast<std::remove_reference<decltype(retval.type)>::type>(msg.type());
    retval.base = from_protobuf(msg.base());
    retval.planet = from_protobuf(msg.planet());
    return retval;
}

void to_bin (const ::hex_zone_fixture_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.type, bin);
    to_bin(value.base, bin);
    to_bin(value.planet, bin);
}

::hex_zone_fixture_t hex_zone_fixture_t_from_bin (unsigned char*& bin)
{
    ::hex_zone_fixture_t retval;
    retval.type = enum_from_bin<std::remove_reference<decltype(retval.type)>::type>(bin);
    retval.base = unit_t_from_bin(bin);
    retval.planet = planet_t_from_bin(bin);
    return retval;
}

message::game_data::hex_zone_t to_protobuf (const ::hex_zone_t& value)
{
    message::game_data::hex_zone_t retval;
    retval.set_name(value.name);
    for (const auto& x : value.fixtures) {
        retval.add_fixtures()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::hex_zone_t from_protobuf (const message::game_data::hex_zone_t& msg)
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

void to_bin (const ::hex_zone_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.name, bin);
    {
        int length = static_cast<int>(value.fixtures.size());
        to_bin(length, bin);
        for (const auto& x : value.fixtures) {
            to_bin(x, bin);
        }
    }
}

::hex_zone_t hex_zone_t_from_bin (unsigned char*& bin)
{
    ::hex_zone_t retval;
    retval.name = string_from_bin(bin);
    {
        int length = int_from_bin(bin);
        retval.fixtures.resize(length);
        for (int i = 0; i < length; ++i) {
            auto x = hex_zone_fixture_t_from_bin(bin);
            retval.fixtures[i] = x;
        }
    }
    return retval;
}

message::game_data::hex_t to_protobuf (const ::hex_t& value)
{
    message::game_data::hex_t retval;
    retval.mutable_coord()->CopyFrom(to_protobuf(value.coord));
    retval.set_province_id(value.province_id);
    for (const auto& x : value.hex_zones) {
        retval.add_hex_zones()->CopyFrom(to_protobuf(x));
    }
    retval.mutable_fleets()->CopyFrom(to_protobuf(value.fleets));
    return retval;
}

::hex_t from_protobuf (const message::game_data::hex_t& msg)
{
    ::hex_t retval;
    retval.coord = from_protobuf(msg.coord());
    retval.province_id = msg.province_id();
    {
        retval.hex_zones.resize(msg.hex_zones_size());
        auto it = retval.hex_zones.begin();
        for (const auto& x : msg.hex_zones()) {
            *it++ = from_protobuf(x);
        }
    }
    retval.fleets = from_protobuf(msg.fleets());
    return retval;
}

void to_bin (const ::hex_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.coord, bin);
    to_bin(value.province_id, bin);
    {
        int length = static_cast<int>(value.hex_zones.size());
        to_bin(length, bin);
        for (const auto& x : value.hex_zones) {
            to_bin(x, bin);
        }
    }
    to_bin(value.fleets, bin);
}

::hex_t hex_t_from_bin (unsigned char*& bin)
{
    ::hex_t retval;
    retval.coord = hex_coord_t_from_bin(bin);
    retval.province_id = int_from_bin(bin);
    {
        int length = int_from_bin(bin);
        retval.hex_zones.resize(length);
        for (int i = 0; i < length; ++i) {
            auto x = hex_zone_t_from_bin(bin);
            retval.hex_zones[i] = x;
        }
    }
    retval.fleets = fleets_t_from_bin(bin);
    return retval;
}

message::game_data::province_t to_protobuf (const ::province_t& value)
{
    message::game_data::province_t retval;
    retval.set_owner(value.owner);
    retval.set_original_owner(value.original_owner);
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::province_t from_protobuf (const message::game_data::province_t& msg)
{
    ::province_t retval;
    retval.owner = msg.owner();
    retval.original_owner = msg.original_owner();
    {
        retval.hexes.resize(msg.hexes_size());
        auto it = retval.hexes.begin();
        for (const auto& x : msg.hexes()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

void to_bin (const ::province_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.owner, bin);
    to_bin(value.original_owner, bin);
    {
        int length = static_cast<int>(value.hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.hexes) {
            to_bin(x, bin);
        }
    }
}

::province_t province_t_from_bin (unsigned char*& bin)
{
    ::province_t retval;
    retval.owner = int_from_bin(bin);
    retval.original_owner = int_from_bin(bin);
    {
        int length = int_from_bin(bin);
        retval.hexes.resize(length);
        for (int i = 0; i < length; ++i) {
            auto x = hex_coord_t_from_bin(bin);
            retval.hexes[i] = x;
        }
    }
    return retval;
}

message::game_data::map_t to_protobuf (const ::map_t& value)
{
    message::game_data::map_t retval;
    retval.set_width(value.width);
    retval.set_height(value.height);
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::map_t from_protobuf (const message::game_data::map_t& msg)
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
    return retval;
}

void to_bin (const ::map_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.width, bin);
    to_bin(value.height, bin);
    {
        int length = static_cast<int>(value.hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.hexes) {
            to_bin(x, bin);
        }
    }
}

::map_t map_t_from_bin (unsigned char*& bin)
{
    ::map_t retval;
    retval.width = int_from_bin(bin);
    retval.height = int_from_bin(bin);
    {
        int length = int_from_bin(bin);
        retval.hexes.resize(length);
        for (int i = 0; i < length; ++i) {
            auto x = hex_t_from_bin(bin);
            retval.hexes[i] = x;
        }
    }
    return retval;
}

message::game_data::offmap_area_t to_protobuf (const ::offmap_area_t& value)
{
    message::game_data::offmap_area_t retval;
    retval.set_owner(value.owner);
    retval.mutable_fleets()->CopyFrom(to_protobuf(value.fleets));
    return retval;
}

::offmap_area_t from_protobuf (const message::game_data::offmap_area_t& msg)
{
    ::offmap_area_t retval;
    retval.owner = msg.owner();
    retval.fleets = from_protobuf(msg.fleets());
    return retval;
}

void to_bin (const ::offmap_area_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.owner, bin);
    to_bin(value.fleets, bin);
}

::offmap_area_t offmap_area_t_from_bin (unsigned char*& bin)
{
    ::offmap_area_t retval;
    retval.owner = int_from_bin(bin);
    retval.fleets = fleets_t_from_bin(bin);
    return retval;
}

message::game_data::offmap_areas_t to_protobuf (const ::offmap_areas_t& value)
{
    message::game_data::offmap_areas_t retval;
    for (const auto& x : value.areas) {
        (*retval.mutable_areas())[x.first] = to_protobuf(x.second);
    }
    return retval;
}

::offmap_areas_t from_protobuf (const message::game_data::offmap_areas_t& msg)
{
    ::offmap_areas_t retval;
    {
        for (const auto& x : msg.areas()) {
            retval.areas[x.first] = from_protobuf(x.second);
        }
    }
    return retval;
}

void to_bin (const ::offmap_areas_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.areas.size());
        to_bin(length, bin);
        for (const auto& x : value.areas) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
}

::offmap_areas_t offmap_areas_t_from_bin (unsigned char*& bin)
{
    ::offmap_areas_t retval;
    {
        int length = int_from_bin(bin);
        for (int i = 0; i < length; ++i) {
            auto key = int_from_bin(bin);
            auto value = offmap_area_t_from_bin(bin);
            retval.areas[key] = value;
        }
    }
    return retval;
}
