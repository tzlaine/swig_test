// WARNING: Generated code.
// This file was generated from base_types.proto (proto3)

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
            case mission_t::no_mission: name = "no_mission"sv; break;
            case mission_t::explore: name = "explore"sv; break;
            case mission_t::attack: name = "attack"sv; break;
            case mission_t::evade: name = "evade"sv; break;
            case mission_t::intercept: name = "intercept"sv; break;
            case mission_t::convoy_guard: name = "convoy_guard"sv; break;
            case mission_t::ground_attack: name = "ground_attack"sv; break;
            case mission_t::privateer: name = "privateer"sv; break;
            case mission_t::found_coloy: name = "found_coloy"sv; break;
            case mission_t::expand_colony: name = "expand_colony"sv; break;
            case mission_t::deliver_armies: name = "deliver_armies"sv; break;
            case mission_t::deliver_repair_yard: name = "deliver_repair_yard"sv; break;
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



