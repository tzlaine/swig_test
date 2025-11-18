// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)

#include "game_data.hpp"
#include <base_types.hpp>
#include <base_types_serialization.hpp>

#include <serialization.hpp>
#if defined(BUILD_FOR_TEST)
#include <iosfwd>
#endif

namespace detail {

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(game_start_params_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.habitable_systems_per_hex_mean, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.habitable_systems_per_hex_plus_minus, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.systems_per_hex, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.map_height, 4, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<game_start_params_t>(game_start_params_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "game_start_params_t"sv;
        constexpr std::array<std::string_view, 5> field_names = {{"<UNKOWN_FIELD>"sv,
          "habitable_systems_per_hex_mean"sv, "habitable_systems_per_hex_plus_minus"sv, "systems_per_hex"sv, "map_height"sv}};
        std::array<int, 4> expected_field_numbers = {{
          1, 2, 3, 4}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 4;
    
        auto read_field = [] (game_start_params_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.habitable_systems_per_hex_mean, src);
            case 2: return detail::deserialize_impl(x.habitable_systems_per_hex_plus_minus, src);
            case 3: return detail::deserialize_impl(x.systems_per_hex, src);
            case 4: return detail::deserialize_impl(x.map_height, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(unit_design_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.owner, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.hull, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.armor, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.propulsion, 5, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.weapons, 6, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.shields, 7, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.detection, 8, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.stealth, 9, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.automation, 10, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.attack, 11, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.defense, 12, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.ground_attack, 13, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<unit_design_t>(unit_design_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "unit_design_t"sv;
        constexpr std::array<std::string_view, 14> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "owner"sv, "hull"sv, "armor"sv, "propulsion"sv, "weapons"sv, "shields"sv, "detection"sv, "stealth"sv, "automation"sv, "attack"sv, "defense"sv, "ground_attack"sv}};
        std::array<int, 13> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 13;
    
        auto read_field = [] (unit_design_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.owner, src);
            case 3: return detail::deserialize_impl(x.hull, src);
            case 4: return detail::deserialize_impl(x.armor, src);
            case 5: return detail::deserialize_impl(x.propulsion, src);
            case 6: return detail::deserialize_impl(x.weapons, src);
            case 7: return detail::deserialize_impl(x.shields, src);
            case 8: return detail::deserialize_impl(x.detection, src);
            case 9: return detail::deserialize_impl(x.stealth, src);
            case 10: return detail::deserialize_impl(x.automation, src);
            case 11: return detail::deserialize_impl(x.attack, src);
            case 12: return detail::deserialize_impl(x.defense, src);
            case 13: return detail::deserialize_impl(x.ground_attack, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(unit_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.design_id, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.design_owner, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.health, 3, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<unit_t>(unit_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "unit_t"sv;
        constexpr std::array<std::string_view, 4> field_names = {{"<UNKOWN_FIELD>"sv,
          "design_id"sv, "design_owner"sv, "health"sv}};
        std::array<int, 3> expected_field_numbers = {{
          1, 2, 3}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 3;
    
        auto read_field = [] (unit_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.design_id, src);
            case 2: return detail::deserialize_impl(x.design_owner, src);
            case 3: return detail::deserialize_impl(x.health, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(fleet_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.mission, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.units, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.fuel, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.rounds, 5, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.missiles, 6, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.fighters, 7, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<fleet_t>(fleet_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "fleet_t"sv;
        constexpr std::array<std::string_view, 8> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "mission"sv, "units"sv, "fuel"sv, "rounds"sv, "missiles"sv, "fighters"sv}};
        std::array<int, 7> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 7;
    
        auto read_field = [] (fleet_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.mission, src);
            case 3: return detail::deserialize_impl(x.units, src);
            case 4: return detail::deserialize_impl(x.fuel, src);
            case 5: return detail::deserialize_impl(x.rounds, src);
            case 6: return detail::deserialize_impl(x.missiles, src);
            case 7: return detail::deserialize_impl(x.fighters, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(fleets_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.fleets, 1, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<fleets_t>(fleets_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "fleets_t"sv;
        constexpr std::array<std::string_view, 2> field_names = {{"<UNKOWN_FIELD>"sv,
          "fleets"sv}};
        std::array<int, 1> expected_field_numbers = {{
          1}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 1;
    
        auto read_field = [] (fleets_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.fleets, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(planet_effect_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.name, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.description, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.amount, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.months_of_effect, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.months_remaining, 5, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.target, 6, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.target_modifiers, 7, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.operation, 8, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<planet_effect_t>(planet_effect_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "planet_effect_t"sv;
        constexpr std::array<std::string_view, 9> field_names = {{"<UNKOWN_FIELD>"sv,
          "name"sv, "description"sv, "amount"sv, "months_of_effect"sv, "months_remaining"sv, "target"sv, "target_modifiers"sv, "operation"sv}};
        std::array<int, 8> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 8;
    
        auto read_field = [] (planet_effect_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.name, src);
            case 2: return detail::deserialize_impl(x.description, src);
            case 3: return detail::deserialize_impl(x.amount, src);
            case 4: return detail::deserialize_impl(x.months_of_effect, src);
            case 5: return detail::deserialize_impl(x.months_remaining, src);
            case 6: return detail::deserialize_impl(x.target, src);
            case 7: return detail::deserialize_impl(x.target_modifiers, src);
            case 8: return detail::deserialize_impl(x.operation, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(planet_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.system_id, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.planet_type, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.mass_kg, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.radius_km, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.orbit_au, 5, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.orbital_period_y, 6, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.gravity_g, 7, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.axial_tilt_d, 8, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.day_h, 9, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.surface_temperature_k, 10, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.magnetosphere_strength, 11, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.atmopsheric_pressure, 12, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.o2_co2_suitability, 13, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.ocean_coverage, 14, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.growth_factor, 15, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.atmosphere_type, 16, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.water, 17, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.food, 18, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.energy, 19, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.metal, 20, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.fuel, 21, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.population, 22, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.infrastructure, 23, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.max_population, 24, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.owner, 25, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.original_owner, 26, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.garrison, 27, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.effects, 28, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<planet_t>(planet_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "planet_t"sv;
        constexpr std::array<std::string_view, 29> field_names = {{"<UNKOWN_FIELD>"sv,
          "system_id"sv, "planet_type"sv, "mass_kg"sv, "radius_km"sv, "orbit_au"sv, "orbital_period_y"sv, "gravity_g"sv, "axial_tilt_d"sv, "day_h"sv, "surface_temperature_k"sv, "magnetosphere_strength"sv, "atmopsheric_pressure"sv, "o2_co2_suitability"sv, "ocean_coverage"sv, "growth_factor"sv, "atmosphere_type"sv, "water"sv, "food"sv, "energy"sv, "metal"sv, "fuel"sv, "population"sv, "infrastructure"sv, "max_population"sv, "owner"sv, "original_owner"sv, "garrison"sv, "effects"sv}};
        std::array<int, 28> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 28;
    
        auto read_field = [] (planet_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.system_id, src);
            case 2: return detail::deserialize_impl(x.planet_type, src);
            case 3: return detail::deserialize_impl(x.mass_kg, src);
            case 4: return detail::deserialize_impl(x.radius_km, src);
            case 5: return detail::deserialize_impl(x.orbit_au, src);
            case 6: return detail::deserialize_impl(x.orbital_period_y, src);
            case 7: return detail::deserialize_impl(x.gravity_g, src);
            case 8: return detail::deserialize_impl(x.axial_tilt_d, src);
            case 9: return detail::deserialize_impl(x.day_h, src);
            case 10: return detail::deserialize_impl(x.surface_temperature_k, src);
            case 11: return detail::deserialize_impl(x.magnetosphere_strength, src);
            case 12: return detail::deserialize_impl(x.atmopsheric_pressure, src);
            case 13: return detail::deserialize_impl(x.o2_co2_suitability, src);
            case 14: return detail::deserialize_impl(x.ocean_coverage, src);
            case 15: return detail::deserialize_impl(x.growth_factor, src);
            case 16: return detail::deserialize_impl(x.atmosphere_type, src);
            case 17: return detail::deserialize_impl(x.water, src);
            case 18: return detail::deserialize_impl(x.food, src);
            case 19: return detail::deserialize_impl(x.energy, src);
            case 20: return detail::deserialize_impl(x.metal, src);
            case 21: return detail::deserialize_impl(x.fuel, src);
            case 22: return detail::deserialize_impl(x.population, src);
            case 23: return detail::deserialize_impl(x.infrastructure, src);
            case 24: return detail::deserialize_impl(x.max_population, src);
            case 25: return detail::deserialize_impl(x.owner, src);
            case 26: return detail::deserialize_impl(x.original_owner, src);
            case 27: return detail::deserialize_impl(x.garrison, src);
            case 28: return detail::deserialize_impl(x.effects, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(location_object_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.bases, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.planet_id, 2, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<location_object_t>(location_object_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "location_object_t"sv;
        constexpr std::array<std::string_view, 3> field_names = {{"<UNKOWN_FIELD>"sv,
          "bases"sv, "planet_id"sv}};
        std::array<int, 2> expected_field_numbers = {{
          1, 2}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 2;
    
        auto read_field = [] (location_object_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.bases, src);
            case 2: return detail::deserialize_impl(x.planet_id, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(system_location_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.objects, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.units, 2, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<system_location_t>(system_location_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "system_location_t"sv;
        constexpr std::array<std::string_view, 3> field_names = {{"<UNKOWN_FIELD>"sv,
          "objects"sv, "units"sv}};
        std::array<int, 2> expected_field_numbers = {{
          1, 2}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 2;
    
        auto read_field = [] (system_location_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.objects, src);
            case 2: return detail::deserialize_impl(x.units, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(star_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.star_class, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.temperature_k, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.solar_masses, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.solar_luminosities, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.solar_radii, 5, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<star_t>(star_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "star_t"sv;
        constexpr std::array<std::string_view, 6> field_names = {{"<UNKOWN_FIELD>"sv,
          "star_class"sv, "temperature_k"sv, "solar_masses"sv, "solar_luminosities"sv, "solar_radii"sv}};
        std::array<int, 5> expected_field_numbers = {{
          1, 2, 3, 4, 5}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 5;
    
        auto read_field = [] (star_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.star_class, src);
            case 2: return detail::deserialize_impl(x.temperature_k, src);
            case 3: return detail::deserialize_impl(x.solar_masses, src);
            case 4: return detail::deserialize_impl(x.solar_luminosities, src);
            case 5: return detail::deserialize_impl(x.solar_radii, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(system_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.name, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.coord, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.star, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.permanent_locations, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.temporary_locations, 5, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.world_pos_x, 6, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.world_pos_y, 7, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.first_planet, 8, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.last_planet, 9, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<system_t>(system_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "system_t"sv;
        constexpr std::array<std::string_view, 10> field_names = {{"<UNKOWN_FIELD>"sv,
          "name"sv, "coord"sv, "star"sv, "permanent_locations"sv, "temporary_locations"sv, "world_pos_x"sv, "world_pos_y"sv, "first_planet"sv, "last_planet"sv}};
        std::array<int, 9> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8, 9}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 9;
    
        auto read_field = [] (system_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.name, src);
            case 2: return detail::deserialize_impl(x.coord, src);
            case 3: return detail::deserialize_impl(x.star, src);
            case 4: return detail::deserialize_impl(x.permanent_locations, src);
            case 5: return detail::deserialize_impl(x.temporary_locations, src);
            case 6: return detail::deserialize_impl(x.world_pos_x, src);
            case 7: return detail::deserialize_impl(x.world_pos_y, src);
            case 8: return detail::deserialize_impl(x.first_planet, src);
            case 9: return detail::deserialize_impl(x.last_planet, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(hex_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.coord, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.province_id, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.first_system, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.last_system, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.fleets, 5, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<hex_t>(hex_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "hex_t"sv;
        constexpr std::array<std::string_view, 6> field_names = {{"<UNKOWN_FIELD>"sv,
          "coord"sv, "province_id"sv, "first_system"sv, "last_system"sv, "fleets"sv}};
        std::array<int, 5> expected_field_numbers = {{
          1, 2, 3, 4, 5}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 5;
    
        auto read_field = [] (hex_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.coord, src);
            case 2: return detail::deserialize_impl(x.province_id, src);
            case 3: return detail::deserialize_impl(x.first_system, src);
            case 4: return detail::deserialize_impl(x.last_system, src);
            case 5: return detail::deserialize_impl(x.fleets, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(province_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.owner, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.hex_coords, 3, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<province_t>(province_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "province_t"sv;
        constexpr std::array<std::string_view, 4> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "owner"sv, "hex_coords"sv}};
        std::array<int, 3> expected_field_numbers = {{
          1, 2, 3}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 3;
    
        auto read_field = [] (province_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.owner, src);
            case 3: return detail::deserialize_impl(x.hex_coords, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(nation_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.unit_designs, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.provinces, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.fleets, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.planets, 5, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.defeated, 6, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<nation_t>(nation_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "nation_t"sv;
        constexpr std::array<std::string_view, 7> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "unit_designs"sv, "provinces"sv, "fleets"sv, "planets"sv, "defeated"sv}};
        std::array<int, 6> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 6;
    
        auto read_field = [] (nation_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.unit_designs, src);
            case 3: return detail::deserialize_impl(x.provinces, src);
            case 4: return detail::deserialize_impl(x.fleets, src);
            case 5: return detail::deserialize_impl(x.planets, src);
            case 6: return detail::deserialize_impl(x.defeated, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(game_state_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.map_width, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.map_height, 2, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.hexes, 3, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.systems, 4, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.planets, 5, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.nations, 6, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<game_state_t>(game_state_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "game_state_t"sv;
        constexpr std::array<std::string_view, 7> field_names = {{"<UNKOWN_FIELD>"sv,
          "map_width"sv, "map_height"sv, "hexes"sv, "systems"sv, "planets"sv, "nations"sv}};
        std::array<int, 6> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 6;
    
        auto read_field = [] (game_state_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.map_width, src);
            case 2: return detail::deserialize_impl(x.map_height, src);
            case 3: return detail::deserialize_impl(x.hexes, src);
            case 4: return detail::deserialize_impl(x.systems, src);
            case 5: return detail::deserialize_impl(x.planets, src);
            case 6: return detail::deserialize_impl(x.nations, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

}


