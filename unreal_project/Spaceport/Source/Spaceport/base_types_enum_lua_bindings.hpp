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
                {"idle", mission_t::idle},
                {"exercise", mission_t::exercise},
                {"explore", mission_t::explore},
                {"intercept_in_system", mission_t::intercept_in_system},
                {"protect_system_location", mission_t::protect_system_location},
                {"hunt_enemy_fleets", mission_t::hunt_enemy_fleets},
                {"guard_supply", mission_t::guard_supply},
                {"attack_settlement_defenses", mission_t::attack_settlement_defenses},
                {"attack_settlement_infrastructure", mission_t::attack_settlement_infrastructure},
                {"attack_settlement_population", mission_t::attack_settlement_population},
                {"blockade_settlement", mission_t::blockade_settlement},
                {"found_settlement", mission_t::found_settlement},
                {"expand_settlement", mission_t::expand_settlement},
                {"deliver_divisions", mission_t::deliver_divisions},
                {"deliver_mobile_repair_yard", mission_t::deliver_mobile_repair_yard},
                {"deliver_mobile_base", mission_t::deliver_mobile_base},
                {"build_outpost", mission_t::build_outpost},
                {"build_starbase", mission_t::build_starbase},
                {"upgrade_base", mission_t::upgrade_base},
        });

    }
}


