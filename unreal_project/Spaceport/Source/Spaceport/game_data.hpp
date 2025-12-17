// WARNING: Generated code.
// This file was generated from game_data (proto3)
#pragma once

#include "config.hpp"
PUSH_WARNING
DISABLE_WARNING(4996)
DISABLE_WARNING(4127)
DISABLE_WARNING(4100)
#include <game_data.pb.h>
POP_WARNING
#include <base_types.hpp>

#include <flags.hpp>

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
inline auto operator<=>(planet_type_t x, planet_type_t y) { return (int)x <=> (int)y; }

enum class atmosphere_type_t {
    invalid_atmosphere_type = 0,
    reduced_type_a = 1,
    oxidized_type_b = 2,
    carbon_rich_type_c = 3,
    high_temperature = 4,
    gas_giant_atmosphere = 5,
    ice_giant_atmosphere = 6,
};
inline auto operator<=>(atmosphere_type_t x, atmosphere_type_t y) { return (int)x <=> (int)y; }

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
inline auto operator<=>(planet_effect_target_t x, planet_effect_target_t y) { return (int)x <=> (int)y; }

enum class planet_effect_mod_t : unsigned int {
    invalid_planet_effect_mod = 0,
    monthly = 1,
    cost = 2,
};
template<> inline flags<planet_effect_mod_t> all_flags<planet_effect_mod_t>() { return flags(planet_effect_mod_t::invalid_planet_effect_mod) | planet_effect_mod_t::monthly | planet_effect_mod_t::cost; }
inline flags<planet_effect_mod_t> operator|(planet_effect_mod_t x, planet_effect_mod_t y) { return flags(x) | y; }
inline flags<planet_effect_mod_t> operator~(planet_effect_mod_t x) { return ~flags(x); }

enum class effect_op_t {
    invalid_effect_op = 0,
    add = 1,
    multiply = 2,
};
inline auto operator<=>(effect_op_t x, effect_op_t y) { return (int)x <=> (int)y; }

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
inline auto operator<=>(star_class_t x, star_class_t y) { return (int)x <=> (int)y; }

struct nation_and_object_id_t
{
    int nation_id = -1;
    int object_id = -1;
    bool operator==(nation_and_object_id_t const &) const = default;
};

struct game_start_params_t
{
    float habitable_systems_per_hex_mean = -1.0f;
    float habitable_systems_per_hex_plus_minus = -1.0f;
    int systems_per_hex = -1;
    int map_height = -1;
    boost::container::flat_map<int, int> player_id_to_nation_id;
    bool operator==(game_start_params_t const &) const = default;
};

struct unit_design_t
{
    nation_and_object_id_t id = {};
    int hull = -1;
    int armor = -1;
    int propulsion = -1;
    int weapons = -1;
    int shields = -1;
    int detection = -1;
    int stealth = -1;
    int automation = -1;
    int attack = -1;
    int defense = -1;
    int ground_attack = -1;
    bool operator==(unit_design_t const &) const = default;
};

struct unit_t
{
    nation_and_object_id_t id = {};
    int health = -1;
    bool operator==(unit_t const &) const = default;
};

struct fleet_position_t
{
    double world_pos_x = -1.0;
    double world_pos_y = -1.0;
    int system_id = -1;
    bool at_permanent_location = false;
    int location_index = -1;
    int object_index = -1;
    bool is_garrison = false;
    bool operator==(fleet_position_t const &) const = default;
};

struct fleet_t
{
    nation_and_object_id_t id = {};
    mission_t mission = mission_t::invalid_mission;
    std::vector<unit_t> units = {};
    float fuel = -1.0f;
    int rounds = -1;
    int missiles = -1;
    int fighters = -1;
    fleet_position_t position = {};
    bool operator==(fleet_t const &) const = default;
};

struct fleets_t
{
    boost::container::flat_map<int, int> fleet_ids;
    bool operator==(fleets_t const &) const = default;
};

struct planet_effect_t
{
    adobe::name_t name = adobe::name_t("");
    adobe::name_t description = adobe::name_t("");
    float amount = -1.0f;
    int months_of_effect = -1;
    int months_remaining = -1;
    planet_effect_target_t target = planet_effect_target_t::invalid_planet_effect_target;
    unsigned int target_modifiers = 0;
    effect_op_t operation = effect_op_t::invalid_effect_op;
    bool operator==(planet_effect_t const &) const = default;
};

struct planet_t
{
    int system_id = -1;
    planet_type_t planet_type = planet_type_t::invalid_planet_type;
    double mass_kg = -1.0;
    double radius_km = -1.0;
    float orbit_au = -1.0f;
    float orbital_period_y = -1.0f;
    float gravity_g = -1.0f;
    float axial_tilt_d = -1.0f;
    float day_h = -1.0f;
    float surface_temperature_k = -1.0f;
    float magnetosphere_strength = -1.0f;
    float atmopsheric_pressure = -1.0f;
    float o2_co2_suitability = -1.0f;
    float ocean_coverage = -1.0f;
    float growth_factor = -1.0f;
    atmosphere_type_t atmosphere_type = atmosphere_type_t::invalid_atmosphere_type;
    int water = -1;
    int food = -1;
    int energy = -1;
    int metal = -1;
    int fuel = -1;
    float population = -1.0f;
    float infrastructure = -1.0f;
    float orbital_pos_r = -1.0f;
    int max_population = -1;
    int owner = -1;
    int original_owner = -1;
    nation_and_object_id_t garrison = {};
    std::vector<planet_effect_t> effects = {};
    bool operator==(planet_t const &) const = default;
};

struct location_object_t
{
    std::vector<nation_and_object_id_t> bases = {};
    std::size_t planet_id;
    bool operator==(location_object_t const &) const = default;
};

struct system_location_t
{
    std::vector<location_object_t> objects = {};
    fleets_t units = {};
    bool operator==(system_location_t const &) const = default;
};

struct star_t
{
    star_class_t star_class = star_class_t::invalid_star_class;
    double temperature_k = -1.0;
    double solar_masses = -1.0;
    double solar_luminosities = -1.0;
    double solar_radii = -1.0;
    bool operator==(star_t const &) const = default;
};

struct system_t
{
    adobe::name_t name = adobe::name_t("");
    hex_coord_t coord = {};
    star_t star = {};
    std::vector<system_location_t> permanent_locations = {};
    std::vector<system_location_t> temporary_locations = {};
    double world_pos_x = -1.0;
    double world_pos_y = -1.0;
    std::size_t first_planet;
    std::size_t last_planet;
    bool operator==(system_t const &) const = default;
};

struct hex_t
{
    hex_coord_t coord = {};
    int province_id = -1;
    std::size_t first_system;
    std::size_t last_system;
    bool operator==(hex_t const &) const = default;
};

struct province_t
{
    nation_and_object_id_t id = {};
    std::vector<hex_coord_t> hex_coords = {};
    bool operator==(province_t const &) const = default;
};

struct nation_t
{
    int id = -1;
    std::vector<unit_design_t> unit_designs = {};
    std::vector<province_t> provinces = {};
    std::vector<fleet_t> fleets = {};
    std::vector<int> hexes_seen;
    std::vector<int> systems_present_in;
    std::vector<int> systems_visited;
    std::vector<int> planets_present_on;
    std::vector<int> planets_surveyed;
    std::vector<nation_and_object_id_t> foreign_designs_seen = {};
    std::vector<nation_and_object_id_t> foreign_designs_glimpsed = {};
    bool defeated = false;
    bool operator==(nation_t const &) const = default;
};

struct game_state_t
{
    int map_width = -1;
    int map_height = -1;
    std::vector<hex_t> hexes = {};
    std::vector<system_t> systems = {};
    std::vector<planet_t> planets = {};
    std::vector<nation_t> nations = {};
    std::vector<unsigned int> alliances;
    int play_speed = -1;
    bool operator==(game_state_t const &) const = default;
};



pb_message::game_data::nation_and_object_id_t to_protobuf (const ::nation_and_object_id_t& value);
::nation_and_object_id_t from_protobuf (const pb_message::game_data::nation_and_object_id_t& msg);

pb_message::game_data::game_start_params_t to_protobuf (const ::game_start_params_t& value);
::game_start_params_t from_protobuf (const pb_message::game_data::game_start_params_t& msg);

pb_message::game_data::unit_design_t to_protobuf (const ::unit_design_t& value);
::unit_design_t from_protobuf (const pb_message::game_data::unit_design_t& msg);

pb_message::game_data::unit_t to_protobuf (const ::unit_t& value);
::unit_t from_protobuf (const pb_message::game_data::unit_t& msg);

pb_message::game_data::fleet_position_t to_protobuf (const ::fleet_position_t& value);
::fleet_position_t from_protobuf (const pb_message::game_data::fleet_position_t& msg);

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
