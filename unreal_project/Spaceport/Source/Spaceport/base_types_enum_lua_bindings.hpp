// WARNING: Generated code.
// This file was generated from base_types.proto (proto3)
#pragma once

#include "lua.hpp"

#include "base_types.hpp"


namespace detail {

    inline void base_types_register_enums(sol::state & l)
    {

        auto mission_t_enum = l.new_enum<mission_t>("mission_t", {
                {"invalid_mission", mission_t::invalid_mission},
                {"no_mission", mission_t::no_mission},
                {"explore", mission_t::explore},
                {"attack", mission_t::attack},
                {"evade", mission_t::evade},
                {"intercept", mission_t::intercept},
                {"convoy_guard", mission_t::convoy_guard},
                {"ground_attack", mission_t::ground_attack},
                {"privateer", mission_t::privateer},
                {"found_coloy", mission_t::found_coloy},
                {"expand_colony", mission_t::expand_colony},
                {"deliver_armies", mission_t::deliver_armies},
                {"deliver_repair_yard", mission_t::deliver_repair_yard},
                {"deliver_mobile_base", mission_t::deliver_mobile_base},
                {"build_outpost", mission_t::build_outpost},
                {"build_starbase", mission_t::build_starbase},
                {"upgrade_base", mission_t::upgrade_base},
        });

    }
}


