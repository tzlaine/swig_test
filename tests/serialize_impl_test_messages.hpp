#pragma once

#include <model.hpp>
#include <json2pb.h>
#include <generate_galaxy.hpp>
#include <serialization.hpp>

#include <gtest/gtest.h>

#include <format>
#include <strstream>


// clang-format off
namespace detail::by_hand {
    enum class mission_t {
        no_mission = 0,
        explore = 1,
        attack = 2,
        evade = 3,
        intercept = 4,
        convoy_guard = 5,
        ground_attack = 6,
        privateer = 7,
        found_coloy = 8,
        expand_colony = 9,
        deliver_armies = 64,
        deliver_repair_yard = 65,
        deliver_mobile_base = 66,
        build_outpost = 67,
        build_starbase = 68,
        upgrade_base = 69,
    };
    inline auto operator<=>(mission_t x, mission_t y) { return (int)x <=> (int)y; }

    struct unit_t
    {
        int design_id;
        int design_owner;
        int health;
        bool operator==(unit_t const &) const = default;
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
        bool operator==(fleet_t const &) const = default;
    };

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t
    serialize_message_impl(unit_t const & x, int field_number, std::ostream * os)
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

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t
    serialize_message_impl(fleet_t const & x, int field_number, std::ostream * os)
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

    inline std::span<std::byte const>
    deserialize_message_impl(unit_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "fleet_t"sv;
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

    inline std::span<std::byte const>
    deserialize_message_impl(fleet_t & x, std::span<std::byte const> src)
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
}
// clang-format on
