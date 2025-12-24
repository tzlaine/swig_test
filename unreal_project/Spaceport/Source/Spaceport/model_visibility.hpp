#pragma once

#include "game_data.hpp"


enum struct visibility_kind { owner, neutral_or_enemy, unseen };

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    unit_design_t const & x);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    unit_t const & x);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    fleet_t const & x,
    int);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    fleets_t const & x);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    planet_effect_t const & x);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    planet_t const & x,
    int);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    location_object_t const & x);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    system_location_t const & x);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    star_t const & x);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    system_t const & x,
    int system_index);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    hex_t const & x,
    int);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    province_t const & x);

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    nation_t const & x,
    int);
