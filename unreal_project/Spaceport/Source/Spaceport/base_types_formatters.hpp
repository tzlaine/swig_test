// WARNING: Generated code.
// This file was generated from base_types.proto (proto3)
#pragma once

#include "base_types.hpp"



#include <format>
#if defined(BUILD_FOR_TEST)
#include <iosfwd>
#endif


template <>
struct std::formatter<mission_t> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(mission_t t, FormatContext & ctx) const {
        std::string_view name;
        using namespace std::literals;
        switch (t) {
            case mission_t::invalid_mission: name = "INVALID"sv; break;
            case mission_t::idle: name = "idle"sv; break;
            case mission_t::exercise: name = "exercise"sv; break;
            case mission_t::explore: name = "explore"sv; break;
            case mission_t::intercept_in_system: name = "intercept_in_system"sv; break;
            case mission_t::protect_system_location: name = "protect_system_location"sv; break;
            case mission_t::hunt_enemy_fleets: name = "hunt_enemy_fleets"sv; break;
            case mission_t::guard_supply: name = "guard_supply"sv; break;
            case mission_t::attack_settlement_defenses: name = "attack_settlement_defenses"sv; break;
            case mission_t::attack_settlement_infrastructure: name = "attack_settlement_infrastructure"sv; break;
            case mission_t::attack_settlement_population: name = "attack_settlement_population"sv; break;
            case mission_t::blockade_settlement: name = "blockade_settlement"sv; break;
            case mission_t::found_settlement: name = "found_settlement"sv; break;
            case mission_t::expand_settlement: name = "expand_settlement"sv; break;
            case mission_t::deliver_divisions: name = "deliver_divisions"sv; break;
            case mission_t::deliver_mobile_repair_yard: name = "deliver_mobile_repair_yard"sv; break;
            case mission_t::deliver_mobile_base: name = "deliver_mobile_base"sv; break;
            case mission_t::build_outpost: name = "build_outpost"sv; break;
            case mission_t::build_starbase: name = "build_starbase"sv; break;
            case mission_t::upgrade_base: name = "upgrade_base"sv; break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<hex_coord_t> {
    constexpr auto parse(std::format_parse_context & ctx)
    { return ctx.begin(); }

    template <typename Ctx>
    auto format(hex_coord_t const & x, Ctx & ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "hex_coord_t(");

        out = std::format_to(out, " x={}", x.x);
        out = std::format_to(out, " y={}", x.y);

        return std::format_to(out, " )");
    }
};
#if defined(BUILD_FOR_TEST)
inline std::ostream & operator<<(std::ostream & os, hex_coord_t const & x)
{ return os << std::format("{}", x); }
#endif



