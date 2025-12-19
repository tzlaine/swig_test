// WARNING: Generated code.
// This file was generated from game_data.proto (proto3)
#pragma once

#include "lua.hpp"

#include "game_data.hpp"
#include <base_types.hpp>

namespace detail {

    inline void game_data_register_enums(sol::state & l)
    {

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

        auto planet_effect_target_t_enum = l.new_enum<planet_effect_target_t>("planet_effect_target_t", {
                {"invalid_planet_effect_target", planet_effect_target_t::invalid_planet_effect_target},
                {"gravity", planet_effect_target_t::gravity},
                {"axial_tilt", planet_effect_target_t::axial_tilt},
                {"day", planet_effect_target_t::day},
                {"surface_temperature", planet_effect_target_t::surface_temperature},
                {"magnetosphere_strength", planet_effect_target_t::magnetosphere_strength},
                {"atmopsheric_pressure", planet_effect_target_t::atmopsheric_pressure},
                {"o2_co2_suitability", planet_effect_target_t::o2_co2_suitability},
                {"growth_factor", planet_effect_target_t::growth_factor},
                {"water", planet_effect_target_t::water},
                {"food", planet_effect_target_t::food},
                {"energy", planet_effect_target_t::energy},
                {"metal", planet_effect_target_t::metal},
                {"fuel", planet_effect_target_t::fuel},
                {"population", planet_effect_target_t::population},
                {"infrastructure", planet_effect_target_t::infrastructure},
                {"max_population", planet_effect_target_t::max_population},
        });

        auto planet_effect_mod_t_enum = l.new_enum<planet_effect_mod_t>("planet_effect_mod_t", {
                {"invalid_planet_effect_mod", planet_effect_mod_t::invalid_planet_effect_mod},
                {"monthly", planet_effect_mod_t::monthly},
                {"cost", planet_effect_mod_t::cost},
        });

        auto effect_op_t_enum = l.new_enum<effect_op_t>("effect_op_t", {
                {"invalid_effect_op", effect_op_t::invalid_effect_op},
                {"add", effect_op_t::add},
                {"multiply", effect_op_t::multiply},
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


