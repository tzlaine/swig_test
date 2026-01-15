// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)
#pragma once

#include "game_data.hpp"
#include <base_types.hpp>
#include <base_types_formatters.hpp>

#include <format>
#if defined(BUILD_FOR_TEST)
#include <iosfwd>
#endif


template <>
struct std::formatter<cargo_kind_t> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(cargo_kind_t t, FormatContext & ctx) const {
        std::string_view name;
        using namespace std::literals;
        switch (t) {
            case cargo_kind_t::invalid_cargo_kind: name = "INVALID"sv; break;
            case cargo_kind_t::water: name = "water"sv; break;
            case cargo_kind_t::supplies: name = "supplies"sv; break;
            case cargo_kind_t::fuel: name = "fuel"sv; break;
            case cargo_kind_t::rounds: name = "rounds"sv; break;
            case cargo_kind_t::missiles: name = "missiles"sv; break;
            case cargo_kind_t::troops: name = "troops"sv; break;
            case cargo_kind_t::colonists: name = "colonists"sv; break;
            case cargo_kind_t::cargo_metal: name = "cargo_metal"sv; break;
            case cargo_kind_t::cargo_energy: name = "cargo_energy"sv; break;
            case cargo_kind_t::cargo_fuel_minerals: name = "cargo_fuel_minerals"sv; break;
            case cargo_kind_t::cargo_water: name = "cargo_water"sv; break;
            case cargo_kind_t::cargo_food: name = "cargo_food"sv; break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<hit_table_entry_t> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(hit_table_entry_t t, FormatContext & ctx) const {
        std::string_view name;
        using namespace std::literals;
        switch (t) {
            case hit_table_entry_t::invalid_hit_table_entry: name = "INVALID"sv; break;
            case hit_table_entry_t::hit_propulsion: name = "hit_propulsion"sv; break;
            case hit_table_entry_t::hit_weapons: name = "hit_weapons"sv; break;
            case hit_table_entry_t::hit_shields: name = "hit_shields"sv; break;
            case hit_table_entry_t::hit_detection: name = "hit_detection"sv; break;
            case hit_table_entry_t::hit_stealth: name = "hit_stealth"sv; break;
            case hit_table_entry_t::hit_fuel: name = "hit_fuel"sv; break;
            case hit_table_entry_t::hit_water: name = "hit_water"sv; break;
            case hit_table_entry_t::hit_supplies: name = "hit_supplies"sv; break;
            case hit_table_entry_t::hit_rounds: name = "hit_rounds"sv; break;
            case hit_table_entry_t::hit_missiles: name = "hit_missiles"sv; break;
            case hit_table_entry_t::hit_fighters: name = "hit_fighters"sv; break;
            case hit_table_entry_t::hit_cargo: name = "hit_cargo"sv; break;
            case hit_table_entry_t::hit_destroyed: name = "hit_destroyed"sv; break;
            case hit_table_entry_t::hit_unused: name = "hit_unused"sv; break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<planet_type_t> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(planet_type_t t, FormatContext & ctx) const {
        std::string_view name;
        using namespace std::literals;
        switch (t) {
            case planet_type_t::invalid_planet_type: name = "INVALID"sv; break;
            case planet_type_t::rocky: name = "rocky"sv; break;
            case planet_type_t::gas_giant: name = "gas_giant"sv; break;
            case planet_type_t::ice_giant: name = "ice_giant"sv; break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<atmosphere_type_t> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(atmosphere_type_t t, FormatContext & ctx) const {
        std::string_view name;
        using namespace std::literals;
        switch (t) {
            case atmosphere_type_t::invalid_atmosphere_type: name = "INVALID"sv; break;
            case atmosphere_type_t::reduced_type_a: name = "reduced_type_a"sv; break;
            case atmosphere_type_t::oxidized_type_b: name = "oxidized_type_b"sv; break;
            case atmosphere_type_t::carbon_rich_type_c: name = "carbon_rich_type_c"sv; break;
            case atmosphere_type_t::high_temperature: name = "high_temperature"sv; break;
            case atmosphere_type_t::gas_giant_atmosphere: name = "gas_giant_atmosphere"sv; break;
            case atmosphere_type_t::ice_giant_atmosphere: name = "ice_giant_atmosphere"sv; break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<star_class_t> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(star_class_t t, FormatContext & ctx) const {
        std::string_view name;
        using namespace std::literals;
        switch (t) {
            case star_class_t::invalid_star_class: name = "INVALID"sv; break;
            case star_class_t::o: name = "o"sv; break;
            case star_class_t::b: name = "b"sv; break;
            case star_class_t::a: name = "a"sv; break;
            case star_class_t::f: name = "f"sv; break;
            case star_class_t::g: name = "g"sv; break;
            case star_class_t::k: name = "k"sv; break;
            case star_class_t::m: name = "m"sv; break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<nation_and_object_id_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(nation_and_object_id_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "nation_and_object_id_t(");

        out = std::format_to(out, " nation_id={}", x.nation_id);
        out = std::format_to(out, " object_id={}", x.object_id);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, nation_and_object_id_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<game_start_params_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(game_start_params_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "game_start_params_t(");

        out = std::format_to(out, " habitable_systems_per_hex_mean={}", x.habitable_systems_per_hex_mean);
        out = std::format_to(out, " habitable_systems_per_hex_plus_minus={}", x.habitable_systems_per_hex_plus_minus);
        out = std::format_to(out, " systems_per_hex={}", x.systems_per_hex);
        out = std::format_to(out, " map_height={}", x.map_height);
        out = std::format_to(out, " player_id_to_nation_id={{");
        for (auto && [key, value] : x.player_id_to_nation_id) {
            out = std::format_to(out, " {}:{}", key, value);
        };
        out = std::format_to(out, " }}");
        out = std::format_to(out, " ai_opponents={}", x.ai_opponents);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, game_start_params_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<unit_design_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(unit_design_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "unit_design_t(");

        out = std::format_to(out, " id={}", x.id);
        out = std::format_to(out, " hull={}", x.hull);
        out = std::format_to(out, " armor={}", x.armor);
        out = std::format_to(out, " propulsion={}", x.propulsion);
        out = std::format_to(out, " weapons={}", x.weapons);
        out = std::format_to(out, " shields={}", x.shields);
        out = std::format_to(out, " detection={}", x.detection);
        out = std::format_to(out, " stealth={}", x.stealth);
        out = std::format_to(out, " fuel={}", x.fuel);
        out = std::format_to(out, " water={}", x.water);
        out = std::format_to(out, " supplies={}", x.supplies);
        out = std::format_to(out, " rounds={}", x.rounds);
        out = std::format_to(out, " missiles={}", x.missiles);
        out = std::format_to(out, " fighters={}", x.fighters);
        out = std::format_to(out, " cargo={}", x.cargo);
        out = std::format_to(out, " propulsion_space={}", x.propulsion_space);
        out = std::format_to(out, " weapons_space={}", x.weapons_space);
        out = std::format_to(out, " shields_space={}", x.shields_space);
        out = std::format_to(out, " detection_space={}", x.detection_space);
        out = std::format_to(out, " stealth_space={}", x.stealth_space);
        out = std::format_to(out, " effective_propulsion={}", x.effective_propulsion);
        out = std::format_to(out, " effective_weapons={}", x.effective_weapons);
        out = std::format_to(out, " effective_shields={}", x.effective_shields);
        out = std::format_to(out, " effective_detection={}", x.effective_detection);
        out = std::format_to(out, " effective_stealth={}", x.effective_stealth);
        out = std::format_to(out, " propulsion_reliability={}", x.propulsion_reliability);
        out = std::format_to(out, " weapons_reliability={}", x.weapons_reliability);
        out = std::format_to(out, " shields_reliability={}", x.shields_reliability);
        out = std::format_to(out, " detection_reliability={}", x.detection_reliability);
        out = std::format_to(out, " stealth_reliability={}", x.stealth_reliability);
        out = std::format_to(out, " known_propulsion_failures={}", x.known_propulsion_failures);
        out = std::format_to(out, " known_weapons_failures={}", x.known_weapons_failures);
        out = std::format_to(out, " known_shields_failures={}", x.known_shields_failures);
        out = std::format_to(out, " known_detection_failures={}", x.known_detection_failures);
        out = std::format_to(out, " known_stealth_failures={}", x.known_stealth_failures);
        out = std::format_to(out, " design_months_remaining={}", x.design_months_remaining);
        out = std::format_to(out, " hit_table={{");
        for (auto c : x.hit_table) {
            out = std::format_to(out, " {}", (int)c);
        };
        out = std::format_to(out, " }}");
        out = std::format_to(out, " failed_design={}", x.failed_design);
        out = std::format_to(out, " obsolete={}", x.obsolete);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, unit_design_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<unit_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(unit_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "unit_t(");

        out = std::format_to(out, " id={}", x.id);
        out = std::format_to(out, " hull={}", x.hull);
        out = std::format_to(out, " armor={}", x.armor);
        out = std::format_to(out, " propulsion={}", x.propulsion);
        out = std::format_to(out, " weapons={}", x.weapons);
        out = std::format_to(out, " shields={}", x.shields);
        out = std::format_to(out, " detection={}", x.detection);
        out = std::format_to(out, " stealth={}", x.stealth);
        out = std::format_to(out, " fuel={}", x.fuel);
        out = std::format_to(out, " water={}", x.water);
        out = std::format_to(out, " supplies={}", x.supplies);
        out = std::format_to(out, " rounds={}", x.rounds);
        out = std::format_to(out, " missiles={}", x.missiles);
        out = std::format_to(out, " fighters={}", x.fighters);
        out = std::format_to(out, " cargo={{");
        for (auto c : x.cargo) {
            out = std::format_to(out, " {}", (int)c);
        };
        out = std::format_to(out, " }}");
        out = std::format_to(out, " organization={}", x.organization);
        out = std::format_to(out, " experience={}", x.experience);
        out = std::format_to(out, " crew={}", x.crew);
        out = std::format_to(out, " hit_table={{");
        for (auto c : x.hit_table) {
            out = std::format_to(out, " {}", (int)c);
        };
        out = std::format_to(out, " }}");

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, unit_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<fleet_position_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(fleet_position_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "fleet_position_t(");

        out = std::format_to(out, " world_pos_x={}", x.world_pos_x);
        out = std::format_to(out, " world_pos_y={}", x.world_pos_y);
        out = std::format_to(out, " system_id={}", x.system_id);
        out = std::format_to(out, " at_permanent_location={}", x.at_permanent_location);
        out = std::format_to(out, " location_index={}", x.location_index);
        out = std::format_to(out, " object_index={}", x.object_index);
        out = std::format_to(out, " is_garrison={}", x.is_garrison);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, fleet_position_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<fleet_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(fleet_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "fleet_t(");

        out = std::format_to(out, " id={}", x.id);
        out = std::format_to(out, " mission={}", x.mission);
        out = std::format_to(out, " units=[");
        for (auto && e : x.units) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " position={}", x.position);
        out = std::format_to(out, " fleet_experience={}", x.fleet_experience);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, fleet_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<fleets_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(fleets_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "fleets_t(");

        out = std::format_to(out, " fleet_ids={{");
        for (auto && [key, value] : x.fleet_ids) {
            out = std::format_to(out, " {}:{}", key, value);
        };
        out = std::format_to(out, " }}");

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, fleets_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<resource_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(resource_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "resource_t(");

        out = std::format_to(out, " availability={}", x.availability);
        out = std::format_to(out, " max_availability={}", x.max_availability);
        out = std::format_to(out, " stockpile={}", x.stockpile);
        out = std::format_to(out, " max_stockpile={}", x.max_stockpile);
        out = std::format_to(out, " stockpile_last_month={}", x.stockpile_last_month);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, resource_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<settlement_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(settlement_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "settlement_t(");

        out = std::format_to(out, " id={}", x.id);
        out = std::format_to(out, " planet_id={}", x.planet_id);
        out = std::format_to(out, " original_owner={}", x.original_owner);
        out = std::format_to(out, " population={}", x.population);
        out = std::format_to(out, " infrastructure={}", x.infrastructure);
        out = std::format_to(out, " water={}", x.water);
        out = std::format_to(out, " food={}", x.food);
        out = std::format_to(out, " energy={}", x.energy);
        out = std::format_to(out, " metal={}", x.metal);
        out = std::format_to(out, " fuel_minerals={}", x.fuel_minerals);
        out = std::format_to(out, " shipyard_pops={}", x.shipyard_pops);
        out = std::format_to(out, " infrastructure_upgrade_pops={}", x.infrastructure_upgrade_pops);
        out = std::format_to(out, " infrastructure_maintenance_pops={}", x.infrastructure_maintenance_pops);
        out = std::format_to(out, " infrastructure_repair_pops={}", x.infrastructure_repair_pops);
        out = std::format_to(out, " water_pops={}", x.water_pops);
        out = std::format_to(out, " food_pops={}", x.food_pops);
        out = std::format_to(out, " energy_pops={}", x.energy_pops);
        out = std::format_to(out, " metal_pops={}", x.metal_pops);
        out = std::format_to(out, " fuel_minerals_pops={}", x.fuel_minerals_pops);
        out = std::format_to(out, " fuel_refining_pops={}", x.fuel_refining_pops);
        out = std::format_to(out, " supply_manufaturing_pops={}", x.supply_manufaturing_pops);
        out = std::format_to(out, " round_manufaturing_pops={}", x.round_manufaturing_pops);
        out = std::format_to(out, " missile_manufaturing_pops={}", x.missile_manufaturing_pops);
        out = std::format_to(out, " fighter_manufaturing_pops={}", x.fighter_manufaturing_pops);
        out = std::format_to(out, " garrison=[");
        for (auto && e : x.garrison) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, settlement_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<planet_effect_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(planet_effect_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "planet_effect_t(");

        out = std::format_to(out, " name={}", x.name);
        out = std::format_to(out, " reason={}", x.reason);
        out = std::format_to(out, " value={}", x.value);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, planet_effect_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<planet_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(planet_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "planet_t(");

        out = std::format_to(out, " system_id={}", x.system_id);
        out = std::format_to(out, " planet_type={}", x.planet_type);
        out = std::format_to(out, " mass_kg={}", x.mass_kg);
        out = std::format_to(out, " radius_km={}", x.radius_km);
        out = std::format_to(out, " orbit_au={}", x.orbit_au);
        out = std::format_to(out, " orbital_period_y={}", x.orbital_period_y);
        out = std::format_to(out, " gravity_g={}", x.gravity_g);
        out = std::format_to(out, " axial_tilt_d={}", x.axial_tilt_d);
        out = std::format_to(out, " day_h={}", x.day_h);
        out = std::format_to(out, " surface_temperature_k={}", x.surface_temperature_k);
        out = std::format_to(out, " magnetosphere_strength={}", x.magnetosphere_strength);
        out = std::format_to(out, " atmospheric_pressure={}", x.atmospheric_pressure);
        out = std::format_to(out, " o2_co2_suitability={}", x.o2_co2_suitability);
        out = std::format_to(out, " ocean_coverage={}", x.ocean_coverage);
        out = std::format_to(out, " growth_factor={}", x.growth_factor);
        out = std::format_to(out, " atmosphere_type={}", x.atmosphere_type);
        out = std::format_to(out, " water={}", x.water);
        out = std::format_to(out, " food={}", x.food);
        out = std::format_to(out, " energy={}", x.energy);
        out = std::format_to(out, " metal={}", x.metal);
        out = std::format_to(out, " fuel_minerals={}", x.fuel_minerals);
        out = std::format_to(out, " infrastructure_cost_factor={}", x.infrastructure_cost_factor);
        out = std::format_to(out, " orbital_pos_r={}", x.orbital_pos_r);
        out = std::format_to(out, " max_population={}", x.max_population);
        out = std::format_to(out, " effects=[");
        for (auto && e : x.effects) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " settlement_ids=[");
        for (auto && e : x.settlement_ids) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, planet_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<location_object_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(location_object_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "location_object_t(");

        out = std::format_to(out, " bases=[");
        for (auto && e : x.bases) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " planet_id={}", x.planet_id);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, location_object_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<system_location_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(system_location_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "system_location_t(");

        out = std::format_to(out, " objects=[");
        for (auto && e : x.objects) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " units={}", x.units);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, system_location_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<star_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(star_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "star_t(");

        out = std::format_to(out, " star_class={}", x.star_class);
        out = std::format_to(out, " temperature_k={}", x.temperature_k);
        out = std::format_to(out, " solar_masses={}", x.solar_masses);
        out = std::format_to(out, " solar_luminosities={}", x.solar_luminosities);
        out = std::format_to(out, " solar_radii={}", x.solar_radii);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, star_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<system_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(system_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "system_t(");

        out = std::format_to(out, " name={}", x.name);
        out = std::format_to(out, " hex_id={}", x.hex_id);
        out = std::format_to(out, " star={}", x.star);
        out = std::format_to(out, " permanent_locations=[");
        for (auto && e : x.permanent_locations) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " temporary_locations=[");
        for (auto && e : x.temporary_locations) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " world_pos_x={}", x.world_pos_x);
        out = std::format_to(out, " world_pos_y={}", x.world_pos_y);
        out = std::format_to(out, " first_planet={}", x.first_planet);
        out = std::format_to(out, " last_planet={}", x.last_planet);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, system_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<hex_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(hex_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "hex_t(");

        out = std::format_to(out, " coord={}", x.coord);
        out = std::format_to(out, " province_id={}", x.province_id);
        out = std::format_to(out, " first_system={}", x.first_system);
        out = std::format_to(out, " last_system={}", x.last_system);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, hex_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<province_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(province_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "province_t(");

        out = std::format_to(out, " id={}", x.id);
        out = std::format_to(out, " hex_coords=[");
        for (auto && e : x.hex_coords) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, province_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<nation_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(nation_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "nation_t(");

        out = std::format_to(out, " id={}", x.id);
        out = std::format_to(out, " home_planet={}", x.home_planet);
        out = std::format_to(out, " capitol_settlement={}", x.capitol_settlement);
        out = std::format_to(out, " money={}", x.money);
        out = std::format_to(out, " unit_designs=[");
        for (auto && e : x.unit_designs) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " provinces=[");
        for (auto && e : x.provinces) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " settlements=[");
        for (auto && e : x.settlements) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " fleets=[");
        for (auto && e : x.fleets) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " transports={}", x.transports);
        out = std::format_to(out, " hexes_seen=[");
        for (auto && e : x.hexes_seen) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " systems_present_in=[");
        for (auto && e : x.systems_present_in) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " systems_visited=[");
        for (auto && e : x.systems_visited) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " planets_surveyed=[");
        for (auto && e : x.planets_surveyed) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " settlements_seen=[");
        for (auto && e : x.settlements_seen) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " foreign_designs_seen=[");
        for (auto && e : x.foreign_designs_seen) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " foreign_designs_glimpsed=[");
        for (auto && e : x.foreign_designs_glimpsed) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " construction_tech={}", x.construction_tech);
        out = std::format_to(out, " propulsion_tech={}", x.propulsion_tech);
        out = std::format_to(out, " weapons_tech={}", x.weapons_tech);
        out = std::format_to(out, " shields_tech={}", x.shields_tech);
        out = std::format_to(out, " stealth_tech={}", x.stealth_tech);
        out = std::format_to(out, " detection_tech={}", x.detection_tech);
        out = std::format_to(out, " automation_tech={}", x.automation_tech);
        out = std::format_to(out, " construction_tradition={}", x.construction_tradition);
        out = std::format_to(out, " propulsion_tradition={}", x.propulsion_tradition);
        out = std::format_to(out, " weapons_tradition={}", x.weapons_tradition);
        out = std::format_to(out, " shields_tradition={}", x.shields_tradition);
        out = std::format_to(out, " stealth_tradition={}", x.stealth_tradition);
        out = std::format_to(out, " detection_tradition={}", x.detection_tradition);
        out = std::format_to(out, " defeated={}", x.defeated);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, nation_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<date_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(date_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "date_t(");

        out = std::format_to(out, " year={}", x.year);
        out = std::format_to(out, " month={}", x.month);
        out = std::format_to(out, " day={}", x.day);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, date_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<game_state_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(game_state_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "game_state_t(");

        out = std::format_to(out, " map_width={}", x.map_width);
        out = std::format_to(out, " map_height={}", x.map_height);
        out = std::format_to(out, " hexes=[");
        for (auto && e : x.hexes) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " systems=[");
        for (auto && e : x.systems) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " planets=[");
        for (auto && e : x.planets) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " nations=[");
        for (auto && e : x.nations) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " alliances=[");
        for (auto && e : x.alliances) {
            out = std::format_to(out, " {}", e);
        };
        out = std::format_to(out, " ]");
        out = std::format_to(out, " play_speed={}", x.play_speed);
        out = std::format_to(out, " date={}", x.date);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, game_state_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<cost_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(cost_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "cost_t(");

        out = std::format_to(out, " money_cost={}", x.money_cost);
        out = std::format_to(out, " metal_cost={}", x.metal_cost);
        out = std::format_to(out, " energy_cost={}", x.energy_cost);
        out = std::format_to(out, " fuel_minerals_cost={}", x.fuel_minerals_cost);
        out = std::format_to(out, " fuel_cost={}", x.fuel_cost);
        out = std::format_to(out, " water_cost={}", x.water_cost);
        out = std::format_to(out, " food_cost={}", x.food_cost);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, cost_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<day_update_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(day_update_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "day_update_t(");

        out = std::format_to(out, " date={}", x.date);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, day_update_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<month_update_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(month_update_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "month_update_t(");

        out = std::format_to(out, " date={}", x.date);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, month_update_t const & x)
{ return os << std::format("{}", x); }
#endif

template <>
struct std::formatter<year_update_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(year_update_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "year_update_t(");

        out = std::format_to(out, " date={}", x.date);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, year_update_t const & x)
{ return os << std::format("{}", x); }
#endif



