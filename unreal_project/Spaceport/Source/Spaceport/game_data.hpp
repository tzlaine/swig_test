// WARNING: Generated code.
// This file was generated from game_data (proto3)
#pragma once

#include <game_data.pb.h>
#include <base_types.hpp>

#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>




enum class planet_type_t {
    invalid_planet_type = 0,
    rocky = 1,
    gas_giant = 2,
    ice_giant = 3,
};

enum class atmosphere_type_t {
    invalid_atmosphere_type = 0,
    reduced_type_a = 1,
    oxidized_type_b = 2,
    carbon_rich_type_c = 3,
    high_temperature = 4,
    gas_giant_atmosphere = 5,
    ice_giant_atmosphere = 6,
};

enum class planet_effect_target_t {
    invalid_planet_effect_target = 0,
    gravity = 1,
    axial_tilt = 2,
    day = 3,
    surface_temperature = 4,
    magnetosphere_strength = 5,
    atmopsheric_pressure = 6,
    o2_co2_suitability = 7,
    growth_factor = 8,
    water = 9,
    food = 10,
    energy = 11,
    metal = 12,
    fuel = 13,
    population = 14,
    infrastructure = 15,
    max_population = 16,
};

enum class planet_effect_mod_t {
    invalid_planet_effect_mod = 0,
    monthly = 1,
    cost = 2,
};

enum class effect_op_t {
    invalid_effect_op = 0,
    add = 1,
    multiply = 2,
};

enum class star_class_t {
    invalid_star_class = 0,
    o = 1,
    b = 2,
    a = 3,
    f = 4,
    g = 5,
    k = 6,
    m = 7,
};

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

struct planet_effect_t
{
    adobe::name_t name;
    adobe::name_t description;
    float amount;
    int months_of_effect;
    int months_remaining;
    planet_effect_target_t target;
    unsigned int target_modifiers;
    effect_op_t operation;
};

struct planet_t
{
    int system_id;
    planet_type_t planet_type;
    double mass_kg;
    double radius_km;
    float orbit_au;
    float orbital_period_y;
    float gravity_g;
    float axial_tilt_d;
    float day_h;
    float surface_temperature_k;
    float magnetosphere_strength;
    float atmopsheric_pressure;
    float o2_co2_suitability;
    float ocean_coverage;
    float growth_factor;
    atmosphere_type_t atmosphere_type;
    int water;
    int food;
    int energy;
    int metal;
    int fuel;
    float population;
    float infrastructure;
    int max_population;
    int owner;
    int original_owner;
    fleet_t garrison;
    std::vector<planet_effect_t> effects;
};

struct location_object_t
{
    fleet_t bases;
    std::ptrdiff_t planet_id;
};

struct system_location_t
{
    std::vector<location_object_t> objects;
    fleets_t units;
};

struct star_t
{
    star_class_t star_class;
    double temperature_k;
    double solar_masses;
    double solar_luminosities;
    double solar_radii;
};

struct system_t
{
    adobe::name_t name;
    hex_coord_t coord;
    star_t star;
    std::vector<system_location_t> permanent_locations;
    std::vector<system_location_t> temporary_locations;
    double world_pos_x;
    double world_pos_y;
    std::ptrdiff_t first_planet;
    std::ptrdiff_t last_planet;
};

struct hex_t
{
    hex_coord_t coord;
    int province_id;
    std::ptrdiff_t first_system;
    std::ptrdiff_t last_system;
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

pb_message::game_data::planet_effect_t to_protobuf (const ::planet_effect_t& value);
::planet_effect_t from_protobuf (const pb_message::game_data::planet_effect_t& msg);

pb_message::game_data::planet_t to_protobuf (const ::planet_t& value);
::planet_t from_protobuf (const pb_message::game_data::planet_t& msg);

pb_message::game_data::location_object_t to_protobuf (const ::location_object_t& value);
::location_object_t from_protobuf (const pb_message::game_data::location_object_t& msg);

pb_message::game_data::system_location_t to_protobuf (const ::system_location_t& value);
::system_location_t from_protobuf (const pb_message::game_data::system_location_t& msg);

pb_message::game_data::star_t to_protobuf (const ::star_t& value);
::star_t from_protobuf (const pb_message::game_data::star_t& msg);

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
