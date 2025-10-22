// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "game_data.hpp"
#include <base_types.hpp>






pb_message::game_data::unit_design_t to_protobuf (const ::unit_design_t& value)
{
    pb_message::game_data::unit_design_t retval;
    retval.set_id(value.id);
    retval.set_owner(value.owner);
    retval.set_hull(value.hull);
    retval.set_armor(value.armor);
    retval.set_propulsion(value.propulsion);
    retval.set_weapons(value.weapons);
    retval.set_shields(value.shields);
    retval.set_detection(value.detection);
    retval.set_stealth(value.stealth);
    retval.set_automation(value.automation);
    retval.set_attack(value.attack);
    retval.set_defense(value.defense);
    retval.set_ground_attack(value.ground_attack);
    return retval;
}

::unit_design_t from_protobuf (const pb_message::game_data::unit_design_t& msg)
{
    ::unit_design_t retval;
    retval.id = msg.id();
    retval.owner = msg.owner();
    retval.hull = msg.hull();
    retval.armor = msg.armor();
    retval.propulsion = msg.propulsion();
    retval.weapons = msg.weapons();
    retval.shields = msg.shields();
    retval.detection = msg.detection();
    retval.stealth = msg.stealth();
    retval.automation = msg.automation();
    retval.attack = msg.attack();
    retval.defense = msg.defense();
    retval.ground_attack = msg.ground_attack();
    return retval;
}

pb_message::game_data::unit_t to_protobuf (const ::unit_t& value)
{
    pb_message::game_data::unit_t retval;
    retval.set_design_id(value.design_id);
    retval.set_design_owner(value.design_owner);
    retval.set_health(value.health);
    return retval;
}

::unit_t from_protobuf (const pb_message::game_data::unit_t& msg)
{
    ::unit_t retval;
    retval.design_id = msg.design_id();
    retval.design_owner = msg.design_owner();
    retval.health = msg.health();
    return retval;
}

pb_message::game_data::fleet_t to_protobuf (const ::fleet_t& value)
{
    pb_message::game_data::fleet_t retval;
    retval.set_id(value.id);
    retval.set_mission(static_cast< pb_message::mission_t >(value.mission));
    for (const auto& x : value.units) {
        retval.add_units()->CopyFrom(to_protobuf(x));
    }
    retval.set_fuel(value.fuel);
    retval.set_rounds(value.rounds);
    retval.set_missiles(value.missiles);
    retval.set_fighters(value.fighters);
    return retval;
}

::fleet_t from_protobuf (const pb_message::game_data::fleet_t& msg)
{
    ::fleet_t retval;
    retval.id = msg.id();
    retval.mission = static_cast<std::remove_reference<decltype(retval.mission)>::type>(msg.mission());
    {
        retval.units.resize(msg.units_size());
        auto it = retval.units.begin();
        for (const auto& x : msg.units()) {
            *it++ = from_protobuf(x);
        }
    }
    retval.fuel = msg.fuel();
    retval.rounds = msg.rounds();
    retval.missiles = msg.missiles();
    retval.fighters = msg.fighters();
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

pb_message::game_data::planet_effect_t to_protobuf (const ::planet_effect_t& value)
{
    pb_message::game_data::planet_effect_t retval;
    retval.set_name(value.name.c_str());
    retval.set_description(value.description.c_str());
    retval.set_target(static_cast< pb_message::game_data::planet_effect_target_t >(value.target));
    retval.set_one_time_effect(value.one_time_effect);
    retval.set_monthly_effect(value.monthly_effect);
    retval.set_months_of_effect(value.months_of_effect);
    retval.set_months_remaining(value.months_remaining);
    retval.set_effects_are_permanent(value.effects_are_permanent);
    retval.set_affects_cost(value.affects_cost);
    retval.set_multiplicative(value.multiplicative);
    return retval;
}

::planet_effect_t from_protobuf (const pb_message::game_data::planet_effect_t& msg)
{
    ::planet_effect_t retval;
    retval.name = adobe::name_t(msg.name().c_str());
    retval.description = adobe::name_t(msg.description().c_str());
    retval.target = static_cast<std::remove_reference<decltype(retval.target)>::type>(msg.target());
    retval.one_time_effect = msg.one_time_effect();
    retval.monthly_effect = msg.monthly_effect();
    retval.months_of_effect = msg.months_of_effect();
    retval.months_remaining = msg.months_remaining();
    retval.effects_are_permanent = msg.effects_are_permanent();
    retval.affects_cost = msg.affects_cost();
    retval.multiplicative = msg.multiplicative();
    return retval;
}

pb_message::game_data::planet_t to_protobuf (const ::planet_t& value)
{
    pb_message::game_data::planet_t retval;
    retval.set_system_id(value.system_id);
    retval.set_planet_type(static_cast< pb_message::game_data::planet_type_t >(value.planet_type));
    retval.set_mass_kg(value.mass_kg);
    retval.set_radius_km(value.radius_km);
    retval.set_orbit_au(value.orbit_au);
    retval.set_gravity_g(value.gravity_g);
    retval.set_axial_tilt_d(value.axial_tilt_d);
    retval.set_day_h(value.day_h);
    retval.set_surface_temperature_k(value.surface_temperature_k);
    retval.set_magnetosphere_strength(value.magnetosphere_strength);
    retval.set_atmopsheric_pressure(value.atmopsheric_pressure);
    retval.set_o2_co2_suitability(value.o2_co2_suitability);
    retval.set_ocean_coverage(value.ocean_coverage);
    retval.set_growth_factor(value.growth_factor);
    retval.set_atmosphere_type(static_cast< pb_message::game_data::atmosphere_type_t >(value.atmosphere_type));
    retval.set_water(value.water);
    retval.set_food(value.food);
    retval.set_energy(value.energy);
    retval.set_metal(value.metal);
    retval.set_fuel(value.fuel);
    retval.set_population(value.population);
    retval.set_infrastructure(value.infrastructure);
    retval.set_max_population(value.max_population);
    retval.set_owner(value.owner);
    retval.set_original_owner(value.original_owner);
    retval.mutable_garrison()->CopyFrom(to_protobuf(value.garrison));
    for (const auto& x : value.effects) {
        retval.add_effects()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::planet_t from_protobuf (const pb_message::game_data::planet_t& msg)
{
    ::planet_t retval;
    retval.system_id = msg.system_id();
    retval.planet_type = static_cast<std::remove_reference<decltype(retval.planet_type)>::type>(msg.planet_type());
    retval.mass_kg = msg.mass_kg();
    retval.radius_km = msg.radius_km();
    retval.orbit_au = msg.orbit_au();
    retval.gravity_g = msg.gravity_g();
    retval.axial_tilt_d = msg.axial_tilt_d();
    retval.day_h = msg.day_h();
    retval.surface_temperature_k = msg.surface_temperature_k();
    retval.magnetosphere_strength = msg.magnetosphere_strength();
    retval.atmopsheric_pressure = msg.atmopsheric_pressure();
    retval.o2_co2_suitability = msg.o2_co2_suitability();
    retval.ocean_coverage = msg.ocean_coverage();
    retval.growth_factor = msg.growth_factor();
    retval.atmosphere_type = static_cast<std::remove_reference<decltype(retval.atmosphere_type)>::type>(msg.atmosphere_type());
    retval.water = msg.water();
    retval.food = msg.food();
    retval.energy = msg.energy();
    retval.metal = msg.metal();
    retval.fuel = msg.fuel();
    retval.population = msg.population();
    retval.infrastructure = msg.infrastructure();
    retval.max_population = msg.max_population();
    retval.owner = msg.owner();
    retval.original_owner = msg.original_owner();
    retval.garrison = from_protobuf(msg.garrison());
    {
        retval.effects.resize(msg.effects_size());
        auto it = retval.effects.begin();
        for (const auto& x : msg.effects()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

pb_message::game_data::location_object_t to_protobuf (const ::location_object_t& value)
{
    pb_message::game_data::location_object_t retval;
    retval.mutable_bases()->CopyFrom(to_protobuf(value.bases));
    retval.set_planet_id(value.planet_id);
    return retval;
}

::location_object_t from_protobuf (const pb_message::game_data::location_object_t& msg)
{
    ::location_object_t retval;
    retval.bases = from_protobuf(msg.bases());
    retval.planet_id = msg.planet_id();
    return retval;
}

pb_message::game_data::system_location_t to_protobuf (const ::system_location_t& value)
{
    pb_message::game_data::system_location_t retval;
    for (const auto& x : value.objects) {
        retval.add_objects()->CopyFrom(to_protobuf(x));
    }
    retval.mutable_units()->CopyFrom(to_protobuf(value.units));
    return retval;
}

::system_location_t from_protobuf (const pb_message::game_data::system_location_t& msg)
{
    ::system_location_t retval;
    {
        retval.objects.resize(msg.objects_size());
        auto it = retval.objects.begin();
        for (const auto& x : msg.objects()) {
            *it++ = from_protobuf(x);
        }
    }
    retval.units = from_protobuf(msg.units());
    return retval;
}

pb_message::game_data::star_t to_protobuf (const ::star_t& value)
{
    pb_message::game_data::star_t retval;
    retval.set_star_class(static_cast< pb_message::game_data::star_class_t >(value.star_class));
    retval.set_temperature_k(value.temperature_k);
    retval.set_solar_masses(value.solar_masses);
    retval.set_solar_luminosities(value.solar_luminosities);
    retval.set_solar_radii(value.solar_radii);
    return retval;
}

::star_t from_protobuf (const pb_message::game_data::star_t& msg)
{
    ::star_t retval;
    retval.star_class = static_cast<std::remove_reference<decltype(retval.star_class)>::type>(msg.star_class());
    retval.temperature_k = msg.temperature_k();
    retval.solar_masses = msg.solar_masses();
    retval.solar_luminosities = msg.solar_luminosities();
    retval.solar_radii = msg.solar_radii();
    return retval;
}

pb_message::game_data::system_t to_protobuf (const ::system_t& value)
{
    pb_message::game_data::system_t retval;
    retval.set_name(value.name.c_str());
    retval.mutable_coord()->CopyFrom(to_protobuf(value.coord));
    retval.mutable_star()->CopyFrom(to_protobuf(value.star));
    for (const auto& x : value.permanent_locations) {
        retval.add_permanent_locations()->CopyFrom(to_protobuf(x));
    }
    for (const auto& x : value.temporary_locations) {
        retval.add_temporary_locations()->CopyFrom(to_protobuf(x));
    }
    retval.set_world_pos_x(value.world_pos_x);
    retval.set_world_pos_y(value.world_pos_y);
    retval.set_first_planet(value.first_planet);
    retval.set_last_planet(value.last_planet);
    return retval;
}

::system_t from_protobuf (const pb_message::game_data::system_t& msg)
{
    ::system_t retval;
    retval.name = adobe::name_t(msg.name().c_str());
    retval.coord = from_protobuf(msg.coord());
    retval.star = from_protobuf(msg.star());
    {
        retval.permanent_locations.resize(msg.permanent_locations_size());
        auto it = retval.permanent_locations.begin();
        for (const auto& x : msg.permanent_locations()) {
            *it++ = from_protobuf(x);
        }
    }
    {
        retval.temporary_locations.resize(msg.temporary_locations_size());
        auto it = retval.temporary_locations.begin();
        for (const auto& x : msg.temporary_locations()) {
            *it++ = from_protobuf(x);
        }
    }
    retval.world_pos_x = msg.world_pos_x();
    retval.world_pos_y = msg.world_pos_y();
    retval.first_planet = msg.first_planet();
    retval.last_planet = msg.last_planet();
    return retval;
}

pb_message::game_data::hex_t to_protobuf (const ::hex_t& value)
{
    pb_message::game_data::hex_t retval;
    retval.mutable_coord()->CopyFrom(to_protobuf(value.coord));
    retval.set_province_id(value.province_id);
    retval.set_first_system(value.first_system);
    retval.set_last_system(value.last_system);
    retval.mutable_fleets()->CopyFrom(to_protobuf(value.fleets));
    return retval;
}

::hex_t from_protobuf (const pb_message::game_data::hex_t& msg)
{
    ::hex_t retval;
    retval.coord = from_protobuf(msg.coord());
    retval.province_id = msg.province_id();
    retval.first_system = msg.first_system();
    retval.last_system = msg.last_system();
    retval.fleets = from_protobuf(msg.fleets());
    return retval;
}

pb_message::game_data::province_t to_protobuf (const ::province_t& value)
{
    pb_message::game_data::province_t retval;
    retval.set_id(value.id);
    retval.set_owner(value.owner);
    for (const auto& x : value.hex_coords) {
        retval.add_hex_coords()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::province_t from_protobuf (const pb_message::game_data::province_t& msg)
{
    ::province_t retval;
    retval.id = msg.id();
    retval.owner = msg.owner();
    {
        retval.hex_coords.resize(msg.hex_coords_size());
        auto it = retval.hex_coords.begin();
        for (const auto& x : msg.hex_coords()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

pb_message::game_data::nation_t to_protobuf (const ::nation_t& value)
{
    pb_message::game_data::nation_t retval;
    retval.set_id(value.id);
    for (const auto& x : value.unit_designs) {
        retval.add_unit_designs()->CopyFrom(to_protobuf(x));
    }
    for (const auto& x : value.provinces) {
        retval.add_provinces()->CopyFrom(to_protobuf(x));
    }
    for (const auto& x : value.fleets) {
        retval.add_fleets(x);
    }
    for (const auto& x : value.planets) {
        retval.add_planets(x);
    }
    retval.set_defeated(value.defeated);
    return retval;
}

::nation_t from_protobuf (const pb_message::game_data::nation_t& msg)
{
    ::nation_t retval;
    retval.id = msg.id();
    {
        retval.unit_designs.resize(msg.unit_designs_size());
        auto it = retval.unit_designs.begin();
        for (const auto& x : msg.unit_designs()) {
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
    {
        retval.fleets.resize(msg.fleets_size());
        auto it = retval.fleets.begin();
        for (const auto& x : msg.fleets()) {
            *it++ = x;
        }
    }
    {
        retval.planets.resize(msg.planets_size());
        auto it = retval.planets.begin();
        for (const auto& x : msg.planets()) {
            *it++ = x;
        }
    }
    retval.defeated = msg.defeated();
    return retval;
}

pb_message::game_data::game_state_t to_protobuf (const ::game_state_t& value)
{
    pb_message::game_data::game_state_t retval;
    retval.set_map_width(value.map_width);
    retval.set_map_height(value.map_height);
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(to_protobuf(x));
    }
    for (const auto& x : value.systems) {
        retval.add_systems()->CopyFrom(to_protobuf(x));
    }
    for (const auto& x : value.planets) {
        retval.add_planets()->CopyFrom(to_protobuf(x));
    }
    for (const auto& x : value.nations) {
        retval.add_nations()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::game_state_t from_protobuf (const pb_message::game_data::game_state_t& msg)
{
    ::game_state_t retval;
    retval.map_width = msg.map_width();
    retval.map_height = msg.map_height();
    {
        retval.hexes.resize(msg.hexes_size());
        auto it = retval.hexes.begin();
        for (const auto& x : msg.hexes()) {
            *it++ = from_protobuf(x);
        }
    }
    {
        retval.systems.resize(msg.systems_size());
        auto it = retval.systems.begin();
        for (const auto& x : msg.systems()) {
            *it++ = from_protobuf(x);
        }
    }
    {
        retval.planets.resize(msg.planets_size());
        auto it = retval.planets.begin();
        for (const auto& x : msg.planets()) {
            *it++ = from_protobuf(x);
        }
    }
    {
        retval.nations.resize(msg.nations_size());
        auto it = retval.nations.begin();
        for (const auto& x : msg.nations()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}
