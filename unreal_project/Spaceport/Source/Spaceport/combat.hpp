#pragma once

#include <vector>


struct unit_t;
struct unit_design_t;
struct nation_t;

float pd_defense_probability(
    unit_t const & attacker,
    unit_design_t const & attacker_design,
    nation_t const & attacker_nation,
    unit_t const & defender,
    unit_design_t const & defender_design,
    nation_t const & defender_nation);

float pd_hit_probability(
    unit_t const & attacker,
    unit_design_t const & attacker_design,
    nation_t const & attacker_nation,
    unit_t const & defender,
    unit_design_t const & defender_design,
    nation_t const & defender_nation);

float missile_hit_probability(
    unit_t const & attacker,
    unit_design_t const & attacker_design,
    nation_t const & attacker_nation,
    unit_t const & defender,
    unit_design_t const & defender_design,
    nation_t const & defender_nation);

float combat_acceleration(
    unit_t const & unit, unit_design_t const & design, nation_t const & nation);

// TODO: Add a pre-generated array of unit doubles, and use these instead of
// doing each roll individually.  This will make the code race free, and
// probably make it faster independently of that.  The array should be pretty
// big (maybe 8k values?), and there should be a seed that determines where to
// enter the array, and each subsequent read from the array should use modular
// indexing.

enum struct ignore_explosions_t { no, yes };

void apply_hit(
    unit_t & unit,
    int hit_location,
    unit_design_t const & design,
    ignore_explosions_t ignore_explosions);

void load_cargo(
    std::vector<signed char> & cargo, std::vector<signed char> & hit_table);

void damage_unit(
    float damage,
    unit_t & unit,
    unit_design_t const & design,
    std::vector<double> & rolls,
    int roll_index);

void attack(
    unit_t & attacker,
    unit_design_t const & attacker_design,
    nation_t const & attacker_nation,
    unit_t & defender,
    unit_design_t const & defender_design,
    nation_t const & defender_nation,
    std::vector<double> & rolls,
    int roll_index);

// TODO: If not the model, pass something that can record the new orders after
// the engagement, ship lossses, etc.
// TODO: void encounter(model & m, std::vector<fleet_t *> fleets_1, std::vector<fleet_t *> fleets_2);
