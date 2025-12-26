// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)
#pragma once

#include "game_data.hpp"
#include <base_types.hpp>
#include <base_types_serialization.hpp>

#include <serialization.hpp>
#if defined(BUILD_FOR_TEST)
#include <iosfwd>
#endif

namespace detail {

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(nation_and_object_id_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.nation_id, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.object_id, 2, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<nation_and_object_id_t>(nation_and_object_id_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "nation_and_object_id_t"sv;
        constexpr std::array<std::string_view, 3> field_names = {{"<UNKOWN_FIELD>"sv,
          "nation_id"sv, "object_id"sv}};
        std::array<int, 2> expected_field_numbers = {{
          1, 2}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 2;
    
        auto read_field = [] (nation_and_object_id_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.nation_id, src);
            case 2: return detail::deserialize_impl(x.object_id, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(game_start_params_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.habitable_systems_per_hex_mean, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.habitable_systems_per_hex_plus_minus, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.systems_per_hex, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.map_height, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.player_id_to_nation_id, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.ai_opponents, 6, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<game_start_params_t>(game_start_params_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "game_start_params_t"sv;
        constexpr std::array<std::string_view, 7> field_names = {{"<UNKOWN_FIELD>"sv,
          "habitable_systems_per_hex_mean"sv, "habitable_systems_per_hex_plus_minus"sv, "systems_per_hex"sv, "map_height"sv, "player_id_to_nation_id"sv, "ai_opponents"sv}};
        std::array<int, 6> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 6;
    
        auto read_field = [] (game_start_params_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.habitable_systems_per_hex_mean, src);
            case 2: return detail::deserialize_impl(x.habitable_systems_per_hex_plus_minus, src);
            case 3: return detail::deserialize_impl(x.systems_per_hex, src);
            case 4: return detail::deserialize_impl(x.map_height, src);
            case 5: return detail::deserialize_impl(x.player_id_to_nation_id, src);
            case 6: return detail::deserialize_impl(x.ai_opponents, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(unit_design_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.hull, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.armor, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.propulsion, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.weapons, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.shields, 6, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 7; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.detection, 7, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 8; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.stealth, 8, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 9; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.automation, 9, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 10; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.attack, 10, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 11; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.defense, 11, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 12; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.ground_attack, 12, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<unit_design_t>(unit_design_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "unit_design_t"sv;
        constexpr std::array<std::string_view, 13> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "hull"sv, "armor"sv, "propulsion"sv, "weapons"sv, "shields"sv, "detection"sv, "stealth"sv, "automation"sv, "attack"sv, "defense"sv, "ground_attack"sv}};
        std::array<int, 12> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 12;
    
        auto read_field = [] (unit_design_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.hull, src);
            case 3: return detail::deserialize_impl(x.armor, src);
            case 4: return detail::deserialize_impl(x.propulsion, src);
            case 5: return detail::deserialize_impl(x.weapons, src);
            case 6: return detail::deserialize_impl(x.shields, src);
            case 7: return detail::deserialize_impl(x.detection, src);
            case 8: return detail::deserialize_impl(x.stealth, src);
            case 9: return detail::deserialize_impl(x.automation, src);
            case 10: return detail::deserialize_impl(x.attack, src);
            case 11: return detail::deserialize_impl(x.defense, src);
            case 12: return detail::deserialize_impl(x.ground_attack, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(unit_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.health, 2, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<unit_t>(unit_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "unit_t"sv;
        constexpr std::array<std::string_view, 3> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "health"sv}};
        std::array<int, 2> expected_field_numbers = {{
          1, 2}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 2;
    
        auto read_field = [] (unit_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.health, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(fleet_position_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.world_pos_x, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.world_pos_y, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.system_id, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.at_permanent_location, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.location_index, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.object_index, 6, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 7; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.is_garrison, 7, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<fleet_position_t>(fleet_position_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "fleet_position_t"sv;
        constexpr std::array<std::string_view, 8> field_names = {{"<UNKOWN_FIELD>"sv,
          "world_pos_x"sv, "world_pos_y"sv, "system_id"sv, "at_permanent_location"sv, "location_index"sv, "object_index"sv, "is_garrison"sv}};
        std::array<int, 7> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 7;
    
        auto read_field = [] (fleet_position_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.world_pos_x, src);
            case 2: return detail::deserialize_impl(x.world_pos_y, src);
            case 3: return detail::deserialize_impl(x.system_id, src);
            case 4: return detail::deserialize_impl(x.at_permanent_location, src);
            case 5: return detail::deserialize_impl(x.location_index, src);
            case 6: return detail::deserialize_impl(x.object_index, src);
            case 7: return detail::deserialize_impl(x.is_garrison, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(fleet_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.mission, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.units, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.fuel, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.rounds, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.missiles, 6, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 7; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.fighters, 7, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 8; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.position, 8, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<fleet_t>(fleet_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "fleet_t"sv;
        constexpr std::array<std::string_view, 9> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "mission"sv, "units"sv, "fuel"sv, "rounds"sv, "missiles"sv, "fighters"sv, "position"sv}};
        std::array<int, 8> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 8;
    
        auto read_field = [] (fleet_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.mission, src);
            case 3: return detail::deserialize_impl(x.units, src);
            case 4: return detail::deserialize_impl(x.fuel, src);
            case 5: return detail::deserialize_impl(x.rounds, src);
            case 6: return detail::deserialize_impl(x.missiles, src);
            case 7: return detail::deserialize_impl(x.fighters, src);
            case 8: return detail::deserialize_impl(x.position, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(fleets_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.fleet_ids, 1, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<fleets_t>(fleets_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "fleets_t"sv;
        constexpr std::array<std::string_view, 2> field_names = {{"<UNKOWN_FIELD>"sv,
          "fleet_ids"sv}};
        std::array<int, 1> expected_field_numbers = {{
          1}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 1;
    
        auto read_field = [] (fleets_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.fleet_ids, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(settlement_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.planet_id, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.original_owner, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.population, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.infrastructure, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.water, 6, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 7; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.food, 7, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 8; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.energy, 8, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 9; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.metal, 9, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 10; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.fuel, 10, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 11; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.garrison, 11, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<settlement_t>(settlement_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "settlement_t"sv;
        constexpr std::array<std::string_view, 12> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "planet_id"sv, "original_owner"sv, "population"sv, "infrastructure"sv, "water"sv, "food"sv, "energy"sv, "metal"sv, "fuel"sv, "garrison"sv}};
        std::array<int, 11> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 11;
    
        auto read_field = [] (settlement_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.planet_id, src);
            case 3: return detail::deserialize_impl(x.original_owner, src);
            case 4: return detail::deserialize_impl(x.population, src);
            case 5: return detail::deserialize_impl(x.infrastructure, src);
            case 6: return detail::deserialize_impl(x.water, src);
            case 7: return detail::deserialize_impl(x.food, src);
            case 8: return detail::deserialize_impl(x.energy, src);
            case 9: return detail::deserialize_impl(x.metal, src);
            case 10: return detail::deserialize_impl(x.fuel, src);
            case 11: return detail::deserialize_impl(x.garrison, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(planet_effect_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.name, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.reason, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.value, 3, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<planet_effect_t>(planet_effect_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "planet_effect_t"sv;
        constexpr std::array<std::string_view, 4> field_names = {{"<UNKOWN_FIELD>"sv,
          "name"sv, "reason"sv, "value"sv}};
        std::array<int, 3> expected_field_numbers = {{
          1, 2, 3}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 3;
    
        auto read_field = [] (planet_effect_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.name, src);
            case 2: return detail::deserialize_impl(x.reason, src);
            case 3: return detail::deserialize_impl(x.value, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(planet_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.system_id, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.planet_type, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.mass_kg, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.radius_km, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.orbit_au, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.orbital_period_y, 6, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 7; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.gravity_g, 7, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 8; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.axial_tilt_d, 8, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 9; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.day_h, 9, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 10; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.surface_temperature_k, 10, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 11; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.magnetosphere_strength, 11, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 12; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.atmospheric_pressure, 12, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 13; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.o2_co2_suitability, 13, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 14; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.ocean_coverage, 14, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 15; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.growth_factor, 15, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 16; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.atmosphere_type, 16, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 17; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.water, 17, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 18; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.food, 18, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 19; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.energy, 19, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 20; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.metal, 20, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 21; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.fuel, 21, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 22; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.infrastructure_cost_factor, 22, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 23; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.orbital_pos_r, 23, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 24; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.max_population, 24, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 25; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.effects, 25, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 26; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.settlement_ids, 26, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<planet_t>(planet_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "planet_t"sv;
        constexpr std::array<std::string_view, 27> field_names = {{"<UNKOWN_FIELD>"sv,
          "system_id"sv, "planet_type"sv, "mass_kg"sv, "radius_km"sv, "orbit_au"sv, "orbital_period_y"sv, "gravity_g"sv, "axial_tilt_d"sv, "day_h"sv, "surface_temperature_k"sv, "magnetosphere_strength"sv, "atmospheric_pressure"sv, "o2_co2_suitability"sv, "ocean_coverage"sv, "growth_factor"sv, "atmosphere_type"sv, "water"sv, "food"sv, "energy"sv, "metal"sv, "fuel"sv, "infrastructure_cost_factor"sv, "orbital_pos_r"sv, "max_population"sv, "effects"sv, "settlement_ids"sv}};
        std::array<int, 26> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 26;
    
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
            case 12: return detail::deserialize_impl(x.atmospheric_pressure, src);
            case 13: return detail::deserialize_impl(x.o2_co2_suitability, src);
            case 14: return detail::deserialize_impl(x.ocean_coverage, src);
            case 15: return detail::deserialize_impl(x.growth_factor, src);
            case 16: return detail::deserialize_impl(x.atmosphere_type, src);
            case 17: return detail::deserialize_impl(x.water, src);
            case 18: return detail::deserialize_impl(x.food, src);
            case 19: return detail::deserialize_impl(x.energy, src);
            case 20: return detail::deserialize_impl(x.metal, src);
            case 21: return detail::deserialize_impl(x.fuel, src);
            case 22: return detail::deserialize_impl(x.infrastructure_cost_factor, src);
            case 23: return detail::deserialize_impl(x.orbital_pos_r, src);
            case 24: return detail::deserialize_impl(x.max_population, src);
            case 25: return detail::deserialize_impl(x.effects, src);
            case 26: return detail::deserialize_impl(x.settlement_ids, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(location_object_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.bases, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
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

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(system_location_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.objects, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
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

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(star_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.star_class, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.temperature_k, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.solar_masses, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.solar_luminosities, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
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

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(system_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.name, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.hex_id, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.star, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.permanent_locations, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.temporary_locations, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.world_pos_x, 6, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 7; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.world_pos_y, 7, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 8; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.first_planet, 8, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 9; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.last_planet, 9, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<system_t>(system_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "system_t"sv;
        constexpr std::array<std::string_view, 10> field_names = {{"<UNKOWN_FIELD>"sv,
          "name"sv, "hex_id"sv, "star"sv, "permanent_locations"sv, "temporary_locations"sv, "world_pos_x"sv, "world_pos_y"sv, "first_planet"sv, "last_planet"sv}};
        std::array<int, 9> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8, 9}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 9;
    
        auto read_field = [] (system_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.name, src);
            case 2: return detail::deserialize_impl(x.hex_id, src);
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

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(hex_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.coord, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.province_id, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.first_system, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.last_system, 4, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<hex_t>(hex_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "hex_t"sv;
        constexpr std::array<std::string_view, 5> field_names = {{"<UNKOWN_FIELD>"sv,
          "coord"sv, "province_id"sv, "first_system"sv, "last_system"sv}};
        std::array<int, 4> expected_field_numbers = {{
          1, 2, 3, 4}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 4;
    
        auto read_field = [] (hex_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.coord, src);
            case 2: return detail::deserialize_impl(x.province_id, src);
            case 3: return detail::deserialize_impl(x.first_system, src);
            case 4: return detail::deserialize_impl(x.last_system, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(province_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.hex_coords, 2, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<province_t>(province_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "province_t"sv;
        constexpr std::array<std::string_view, 3> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "hex_coords"sv}};
        std::array<int, 2> expected_field_numbers = {{
          1, 2}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 2;
    
        auto read_field = [] (province_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.hex_coords, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(nation_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.id, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.home_planet, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.capitol_settlement, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.unit_designs, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.provinces, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.settlements, 6, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 7; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.fleets, 7, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 8; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.hexes_seen, 8, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 9; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.systems_present_in, 9, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 10; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.systems_visited, 10, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 11; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.planets_surveyed, 11, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 12; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.settlements_seen, 12, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 13; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.foreign_designs_seen, 13, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 14; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.foreign_designs_glimpsed, 14, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 15; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.defeated, 15, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<nation_t>(nation_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "nation_t"sv;
        constexpr std::array<std::string_view, 16> field_names = {{"<UNKOWN_FIELD>"sv,
          "id"sv, "home_planet"sv, "capitol_settlement"sv, "unit_designs"sv, "provinces"sv, "settlements"sv, "fleets"sv, "hexes_seen"sv, "systems_present_in"sv, "systems_visited"sv, "planets_surveyed"sv, "settlements_seen"sv, "foreign_designs_seen"sv, "foreign_designs_glimpsed"sv, "defeated"sv}};
        std::array<int, 15> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 15;
    
        auto read_field = [] (nation_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.id, src);
            case 2: return detail::deserialize_impl(x.home_planet, src);
            case 3: return detail::deserialize_impl(x.capitol_settlement, src);
            case 4: return detail::deserialize_impl(x.unit_designs, src);
            case 5: return detail::deserialize_impl(x.provinces, src);
            case 6: return detail::deserialize_impl(x.settlements, src);
            case 7: return detail::deserialize_impl(x.fleets, src);
            case 8: return detail::deserialize_impl(x.hexes_seen, src);
            case 9: return detail::deserialize_impl(x.systems_present_in, src);
            case 10: return detail::deserialize_impl(x.systems_visited, src);
            case 11: return detail::deserialize_impl(x.planets_surveyed, src);
            case 12: return detail::deserialize_impl(x.settlements_seen, src);
            case 13: return detail::deserialize_impl(x.foreign_designs_seen, src);
            case 14: return detail::deserialize_impl(x.foreign_designs_glimpsed, src);
            case 15: return detail::deserialize_impl(x.defeated, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

    template<ser_op Op, ser_field_op FieldOp, typename OStream, int N = 1>
    std::ptrdiff_t serialize_message_impl(game_state_t const & x, int field_number, OStream * os, std::array<int, N> const & elisions = {{0}})
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        if (std::ranges::none_of(elisions, [](int i) { return i == 1; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.map_width, 1, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 2; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.map_height, 2, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 3; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.hexes, 3, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 4; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.systems, 4, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 5; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.planets, 5, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 6; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.nations, 6, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 7; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.alliances, 7, os);
        if (std::ranges::none_of(elisions, [](int i) { return i == 8; }))
            retval += detail::serialize_impl<Op, ser_field_op::write>(x.play_speed, 8, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<game_state_t>(game_state_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "game_state_t"sv;
        constexpr std::array<std::string_view, 9> field_names = {{"<UNKOWN_FIELD>"sv,
          "map_width"sv, "map_height"sv, "hexes"sv, "systems"sv, "planets"sv, "nations"sv, "alliances"sv, "play_speed"sv}};
        std::array<int, 8> expected_field_numbers = {{
          1, 2, 3, 4, 5, 6, 7, 8}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 8;
    
        auto read_field = [] (game_state_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.map_width, src);
            case 2: return detail::deserialize_impl(x.map_height, src);
            case 3: return detail::deserialize_impl(x.hexes, src);
            case 4: return detail::deserialize_impl(x.systems, src);
            case 5: return detail::deserialize_impl(x.planets, src);
            case 6: return detail::deserialize_impl(x.nations, src);
            case 7: return detail::deserialize_impl(x.alliances, src);
            case 8: return detail::deserialize_impl(x.play_speed, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

}


