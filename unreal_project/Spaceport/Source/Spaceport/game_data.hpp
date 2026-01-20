// WARNING: Generated code.
// This file was generated from game_data (proto3)
#pragma once

#include "config.hpp"
#include <base_types.hpp>

#include <flags.hpp>

#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>




enum class cargo_kind_t {
    invalid_cargo_kind = 0,
    water = -1,
    supplies = -2,
    fuel = -3,
    rounds = -4,
    missiles = -5,
    troops = -6,
    colonists = -7,
    cargo_metal = -8,
    cargo_energy = -9,
    cargo_fuel_minerals = -10,
    cargo_water = -11,
    cargo_food = -12,
};
inline auto operator<=>(cargo_kind_t x, cargo_kind_t y) { return (int)x <=> (int)y; }

enum class hit_table_entry_t {
    invalid_hit_table_entry = 0,
    hit_propulsion = 1,
    hit_weapons = 2,
    hit_shields = 3,
    hit_detection = 4,
    hit_stealth = 5,
    hit_fuel = 6,
    hit_water = 7,
    hit_supplies = 8,
    hit_rounds = 9,
    hit_missiles = 10,
    hit_fighters = 11,
    hit_cargo = 12,
    hit_destroyed = 13,
    hit_unused = 14,
    hit_crew_space = 15,
};
inline auto operator<=>(hit_table_entry_t x, hit_table_entry_t y) { return (int)x <=> (int)y; }

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
    int ai_opponents = -1;
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
    int fuel = -1;
    int water = -1;
    int supplies = -1;
    int rounds = -1;
    int missiles = -1;
    int fighters = -1;
    int cargo = -1;
    int crew = -1;
    int propulsion_space = -1;
    int weapons_space = -1;
    int shields_space = -1;
    int detection_space = -1;
    int stealth_space = -1;
    float effective_propulsion = -1.0f;
    float effective_weapons = -1.0f;
    float effective_shields = -1.0f;
    float effective_detection = -1.0f;
    float effective_stealth = -1.0f;
    int propulsion_reliability = -1;
    int weapons_reliability = -1;
    int shields_reliability = -1;
    int detection_reliability = -1;
    int stealth_reliability = -1;
    int known_propulsion_failures = -1;
    int known_weapons_failures = -1;
    int known_shields_failures = -1;
    int known_detection_failures = -1;
    int known_stealth_failures = -1;
    float design_months_remaining = -1.0f;
    std::vector<signed char> hit_table;
    bool failed_design = false;
    bool obsolete = false;
    bool operator==(unit_design_t const &) const = default;
};

struct unit_t
{
    nation_and_object_id_t id = {};
    int hull = -1;
    float armor = -1.0f;
    float propulsion = -1.0f;
    float weapons = -1.0f;
    float shields = -1.0f;
    float detection = -1.0f;
    float stealth = -1.0f;
    float fuel = -1.0f;
    float water = -1.0f;
    float supplies = -1.0f;
    int rounds = -1;
    int missiles = -1;
    int fighters = -1;
    std::vector<signed char> cargo;
    int organization = -1;
    int combat_experience = -1;
    int crew = -1;
    std::vector<signed char> hit_table;
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
    bool hide = false;
    int engagement_posture = -1;
    int supply_margin = -1;
    int ordnance_margin = -1;
    int return_for_repairs_health = -1;
    bool auto_detach_repair_fleets = false;
    nation_and_object_id_t detached_from = {};
    std::vector<unit_t> units = {};
    fleet_position_t position = {};
    int fleet_combat_experience = -1;
    bool operator==(fleet_t const &) const = default;
};

struct fleets_t
{
    boost::container::flat_map<int, int> fleet_ids;
    bool operator==(fleets_t const &) const = default;
};

struct resource_t
{
    int available = -1;
    int max_availability = -1;
    float stockpile = -1.0f;
    float max_stockpile = -1.0f;
    float stockpile_last_month = -1.0f;
    bool operator==(resource_t const &) const = default;
};

struct settlement_t
{
    nation_and_object_id_t id = {};
    int planet_id = -1;
    int original_owner = -1;
    float population = -1.0f;
    float infrastructure = -1.0f;
    resource_t water = {};
    resource_t food = {};
    resource_t energy = {};
    resource_t metal = {};
    resource_t fuel_minerals = {};
    resource_t fuel = {};
    resource_t supplies = {};
    resource_t crewpower = {};
    resource_t rounds = {};
    resource_t missiles = {};
    resource_t fighters = {};
    int shipyard_pops = -1;
    int infrastructure_upgrade_pops = -1;
    int infrastructure_maintenance_pops = -1;
    int infrastructure_repair_pops = -1;
    int water_pops = -1;
    int food_pops = -1;
    int energy_pops = -1;
    int metal_pops = -1;
    int fuel_minerals_pops = -1;
    int fuel_refining_pops = -1;
    int supply_manufaturing_pops = -1;
    int round_manufaturing_pops = -1;
    int missile_manufaturing_pops = -1;
    int fighter_manufaturing_pops = -1;
    std::vector<nation_and_object_id_t> garrison = {};
    bool operator==(settlement_t const &) const = default;
};

struct planet_effect_t
{
    adobe::name_t name = adobe::name_t("");
    adobe::name_t reason = adobe::name_t("");
    float value = -1.0f;
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
    float atmospheric_pressure = -1.0f;
    float o2_co2_suitability = -1.0f;
    float ocean_coverage = -1.0f;
    float growth_factor = -1.0f;
    atmosphere_type_t atmosphere_type = atmosphere_type_t::invalid_atmosphere_type;
    int water = -1;
    int food = -1;
    int energy = -1;
    int metal = -1;
    int fuel_minerals = -1;
    float infrastructure_cost_factor = -1.0f;
    float orbital_pos_r = -1.0f;
    int max_population = -1;
    std::vector<planet_effect_t> effects = {};
    std::vector<nation_and_object_id_t> settlement_ids = {};
    bool operator==(planet_t const &) const = default;
};

struct location_object_t
{
    std::vector<nation_and_object_id_t> bases = {};
    std::size_t planet_id = std::size_t(0) - 1;
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
    int hex_id = -1;
    star_t star = {};
    std::vector<system_location_t> permanent_locations = {};
    std::vector<system_location_t> temporary_locations = {};
    double world_pos_x = -1.0;
    double world_pos_y = -1.0;
    std::size_t first_planet = std::size_t(0) - 1;
    std::size_t last_planet = std::size_t(0) - 1;
    bool operator==(system_t const &) const = default;
};

struct hex_t
{
    hex_coord_t coord = {};
    int province_id = -1;
    std::size_t first_system = std::size_t(0) - 1;
    std::size_t last_system = std::size_t(0) - 1;
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
    int home_planet = -1;
    nation_and_object_id_t capitol_settlement = {};
    int money = -1;
    std::vector<unit_design_t> unit_designs = {};
    std::vector<province_t> provinces = {};
    std::vector<settlement_t> settlements = {};
    std::vector<fleet_t> fleets = {};
    int transports = -1;
    std::vector<int> hexes_seen;
    std::vector<int> systems_present_in;
    std::vector<int> systems_visited;
    std::vector<int> planets_surveyed;
    std::vector<settlement_t> settlements_seen = {};
    std::vector<nation_and_object_id_t> foreign_designs_seen = {};
    std::vector<nation_and_object_id_t> foreign_designs_glimpsed = {};
    float construction_tech = -1.0f;
    float propulsion_tech = -1.0f;
    float weapons_tech = -1.0f;
    float shields_tech = -1.0f;
    float stealth_tech = -1.0f;
    float detection_tech = -1.0f;
    float automation_tech = -1.0f;
    int construction_tradition = -1;
    int propulsion_tradition = -1;
    int weapons_tradition = -1;
    int shields_tradition = -1;
    int stealth_tradition = -1;
    int detection_tradition = -1;
    bool defeated = false;
    bool operator==(nation_t const &) const = default;
};

struct date_t
{
    int year = -1;
    int month = -1;
    int day = -1;
    bool operator==(date_t const &) const = default;
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
    date_t date = {};
    bool operator==(game_state_t const &) const = default;
};

struct cost_t
{
    float money_cost = -1.0f;
    float metal_cost = -1.0f;
    float energy_cost = -1.0f;
    float fuel_minerals_cost = -1.0f;
    float fuel_cost = -1.0f;
    float water_cost = -1.0f;
    float food_cost = -1.0f;
    bool operator==(cost_t const &) const = default;
};

struct day_update_t
{
    date_t date = {};
    bool operator==(day_update_t const &) const = default;
};

struct month_update_t
{
    date_t date = {};
    bool operator==(month_update_t const &) const = default;
};

struct year_update_t
{
    date_t date = {};
    bool operator==(year_update_t const &) const = default;
};


