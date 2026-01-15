// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)
#pragma once

#include "lua.hpp"

#include "game_data.hpp"
#include <base_types.hpp>

namespace detail {

    inline void game_data_register_enums(sol::state & l)
    {

        auto cargo_kind_t_enum = l.new_enum<cargo_kind_t>("cargo_kind_t", {
                {"invalid_cargo_kind", cargo_kind_t::invalid_cargo_kind},
                {"water", cargo_kind_t::water},
                {"supplies", cargo_kind_t::supplies},
                {"fuel", cargo_kind_t::fuel},
                {"rounds", cargo_kind_t::rounds},
                {"missiles", cargo_kind_t::missiles},
                {"troops", cargo_kind_t::troops},
                {"colonists", cargo_kind_t::colonists},
                {"cargo_metal", cargo_kind_t::cargo_metal},
                {"cargo_energy", cargo_kind_t::cargo_energy},
                {"cargo_fuel_minerals", cargo_kind_t::cargo_fuel_minerals},
                {"cargo_water", cargo_kind_t::cargo_water},
                {"cargo_food", cargo_kind_t::cargo_food},
        });

        auto hit_table_entry_t_enum = l.new_enum<hit_table_entry_t>("hit_table_entry_t", {
                {"invalid_hit_table_entry", hit_table_entry_t::invalid_hit_table_entry},
                {"hit_propulsion", hit_table_entry_t::hit_propulsion},
                {"hit_weapons", hit_table_entry_t::hit_weapons},
                {"hit_shields", hit_table_entry_t::hit_shields},
                {"hit_detection", hit_table_entry_t::hit_detection},
                {"hit_stealth", hit_table_entry_t::hit_stealth},
                {"hit_fuel", hit_table_entry_t::hit_fuel},
                {"hit_water", hit_table_entry_t::hit_water},
                {"hit_supplies", hit_table_entry_t::hit_supplies},
                {"hit_rounds", hit_table_entry_t::hit_rounds},
                {"hit_missiles", hit_table_entry_t::hit_missiles},
                {"hit_fighters", hit_table_entry_t::hit_fighters},
                {"hit_cargo", hit_table_entry_t::hit_cargo},
                {"hit_destroyed", hit_table_entry_t::hit_destroyed},
                {"hit_unused", hit_table_entry_t::hit_unused},
        });

        auto planet_type_t_enum = l.new_enum<planet_type_t>("planet_type_t", {
                {"invalid_planet_type", planet_type_t::invalid_planet_type},
                {"rocky", planet_type_t::rocky},
                {"gas_giant", planet_type_t::gas_giant},
                {"ice_giant", planet_type_t::ice_giant},
        });

        auto atmosphere_type_t_enum = l.new_enum<atmosphere_type_t>("atmosphere_type_t", {
                {"invalid_atmosphere_type", atmosphere_type_t::invalid_atmosphere_type},
                {"reduced_type_a", atmosphere_type_t::reduced_type_a},
                {"oxidized_type_b", atmosphere_type_t::oxidized_type_b},
                {"carbon_rich_type_c", atmosphere_type_t::carbon_rich_type_c},
                {"high_temperature", atmosphere_type_t::high_temperature},
                {"gas_giant_atmosphere", atmosphere_type_t::gas_giant_atmosphere},
                {"ice_giant_atmosphere", atmosphere_type_t::ice_giant_atmosphere},
        });

        auto star_class_t_enum = l.new_enum<star_class_t>("star_class_t", {
                {"invalid_star_class", star_class_t::invalid_star_class},
                {"o", star_class_t::o},
                {"b", star_class_t::b},
                {"a", star_class_t::a},
                {"f", star_class_t::f},
                {"g", star_class_t::g},
                {"k", star_class_t::k},
                {"m", star_class_t::m},
        });

    }
}


