#include "combat.hpp"

#include "rng.hpp"

#include <ranges>


float pd_defense_probability(
    combat_unit const & attacker, combat_unit const & defender)
{
    return base_pd_attack_factor *
           (1 +
            pd_hit_weapons_factor *
                (defender.unit_->weapons - attacker.unit_->weapons) +
            pd_hit_acceleration_factor * (combat_acceleration(defender) -
                                          combat_acceleration(attacker)));
}

float pd_hit_probability(
    combat_unit const & attacker, combat_unit const & defender)
{
    return base_pd_attack_factor *
           (1 + pd_hit_acceleration_factor * (combat_acceleration(defender) -
                                              combat_acceleration(attacker)));
}

float missile_hit_probability(
    combat_unit const & attacker, combat_unit const & defender)
{
    return base_missile_attack_factor *
           (1 +
            missile_hit_acceleration_factor * (combat_acceleration(defender) -
                                               combat_acceleration(attacker)));
}

float combat_acceleration(combat_unit const & cu)
{
    float const max_combat_acceleration =
        call_lua_func("max_combat_acceleration", *cu.nation_);
    float const unit_max_acceleration =
        call_lua_func("unit_max_acceleration", *cu.design_);
    return std::min(unit_max_acceleration, max_combat_acceleration);
}

void apply_hit(
    combat_unit & cu, int hit_location, ignore_explosions_t ignore_explosions)
{
    unit_t & unit = *cu.unit_;
    unit_design_t const & design = *cu.design_;
    auto & table = unit.hit_table;

    auto const destroy_location = [&](bool equipment_here) {
        table[hit_location] = (signed char)hit_table_entry_t::hit_destroyed;
        unit.crew -= int(crew_required_per_hull_point);
        if (equipment_here)
            unit.crew -= int(crew_required_per_equipment_point);
    };

    if (table[hit_location] < 0) {
        signed char const cargo_kind[1] = {table[hit_location]};
        auto end_r = std::ranges::find_end(unit.cargo, cargo_kind);
        check(!end_r.empty());
        unit.cargo.erase(end_r.begin());
        if (cargo_kind[0] == (signed char)cargo_kind_t::fuel) {
            table[hit_location] = (signed char)hit_table_entry_t::hit_fuel;
            apply_hit(cu, hit_location, ignore_explosions_t::no);
        } else if (cargo_kind[0] == (signed char)cargo_kind_t::rounds) {
            table[hit_location] = (signed char)hit_table_entry_t::hit_rounds;
            apply_hit(cu, hit_location, ignore_explosions_t::no);
        } else if (cargo_kind[0] == (signed char)cargo_kind_t::missiles) {
            table[hit_location] = (signed char)hit_table_entry_t::hit_missiles;
            apply_hit(cu, hit_location, ignore_explosions_t::no);
        } else {
            destroy_location(false);
        }
        return;
    }

    auto const entry = hit_table_entry_t(table[hit_location]);
    switch (entry) {
    default:
    case hit_table_entry_t::invalid_hit_table_entry:
    case hit_table_entry_t::hit_unused: break;

    case hit_table_entry_t::hit_propulsion:
        unit.propulsion -= 1.0f / space_required_per_equipment_point;
        cu.acceleration_ = combat_acceleration(cu);
        destroy_location(true);
        break;
    case hit_table_entry_t::hit_weapons:
        unit.weapons -= 1.0f / space_required_per_equipment_point;
        destroy_location(true);
        break;
    case hit_table_entry_t::hit_shields:
        unit.shields -= 1.0f / space_required_per_equipment_point;
        destroy_location(true);
        break;
    case hit_table_entry_t::hit_detection:
        unit.detection -= 1.0f / space_required_per_equipment_point;
        destroy_location(true);
        break;
    case hit_table_entry_t::hit_stealth:
        unit.stealth -= 1.0f / space_required_per_equipment_point;
        destroy_location(true);
        break;

    case hit_table_entry_t::hit_water:
        unit.water -= 1.0f;
        destroy_location(false);
        break;
    case hit_table_entry_t::hit_supplies:
        unit.supplies -= 1.0f;
        destroy_location(false);
        break;

    case hit_table_entry_t::hit_fuel:
    case hit_table_entry_t::hit_rounds:
    case hit_table_entry_t::hit_missiles: {
        if (entry == hit_table_entry_t::hit_fuel)
            unit.fuel -= unit.fuel / design.fuel;
        if (entry == hit_table_entry_t::hit_rounds)
            unit.rounds -= unit.rounds / design.rounds;
        if (entry == hit_table_entry_t::hit_missiles)
            unit.missiles -= unit.missiles / design.missiles;
        destroy_location(false);

        int explosion_lo = hit_location;
        for (int i = explosion_lo; 0 <= i && explosion_lo <= i; --i) {
            if (table[i] == (signed char)hit_table_entry_t::hit_fuel) {
                explosion_lo =
                    std::min(explosion_lo, i - pd_ammo_explosion_radius);
            } else if (
                table[explosion_lo] ==
                    (signed char)hit_table_entry_t::hit_rounds ||
                table[explosion_lo] ==
                    (signed char)hit_table_entry_t::hit_missiles) {
                explosion_lo =
                    std::min(explosion_lo, i - missile_ammo_explosion_radius);
            }
        }

        int explosion_hi = hit_location + 1;
        for (int i = explosion_hi - 1, last = (int)table.size();
             i < last && i < explosion_hi;
             ++i) {
            if (table[i] == (signed char)hit_table_entry_t::hit_fuel) {
                explosion_hi =
                    std::max(explosion_hi, i + pd_ammo_explosion_radius);
            } else if (
                table[explosion_hi] ==
                    (signed char)hit_table_entry_t::hit_rounds ||
                table[explosion_hi] ==
                    (signed char)hit_table_entry_t::hit_missiles) {
                explosion_hi =
                    std::max(explosion_hi, i + missile_ammo_explosion_radius);
            }
        }

        for (int i = explosion_lo; i < explosion_hi; ++i) {
            apply_hit(cu, i, ignore_explosions_t::yes);
        }

        break;
    }

    case hit_table_entry_t::hit_fighters:
        // TODO: Need to handle what happens after combat if the active
        // fighters cannot land.  Need to have a story for fighter attacks and
        // pd defenses against them too.
        unit.fighters -= 1;
        destroy_location(true);
        break;

    case hit_table_entry_t::hit_cargo: /* unreachable */ break;

    case hit_table_entry_t::hit_destroyed:
        for (int i = 0; i < 1 << 20; ++i) {
            if (hit_location + i < table.size() &&
                hit_table_entry_t(table[hit_location + i]) !=
                    hit_table_entry_t::hit_destroyed) {
                apply_hit(cu, hit_location + i, ignore_explosions);
                break;
            } else if (
                0 < hit_location - i &&
                hit_table_entry_t(table[hit_location - i]) !=
                    hit_table_entry_t::hit_destroyed) {
                apply_hit(cu, hit_location - i, ignore_explosions);
                break;
            }
        }
        break;

    case hit_table_entry_t::hit_crew_space: {
        float const assigned_crew = 1.0f / space_required_per_1k_crew;
        int const offduty_assigned_crew =
            assigned_crew * (1 - crew_onduty_factor) + 0.5f;
        unit.crew -= offduty_assigned_crew;
        destroy_location(false);
        break;
    }
    }
}

void load_cargo(
    std::vector<signed char> & cargo, std::vector<signed char> & hit_table)
{
    auto const advance = [&](auto it) {
        return std::find_if(it, hit_table.end(), [](signed char x) {
            return x == (signed char)hit_table_entry_t::hit_cargo;
        });
    };

    check(
        cargo.size() <=
        std::ranges::count(
            hit_table, (signed char)hit_table_entry_t::hit_cargo));

    auto it = advance(hit_table.begin());
    for (auto c : cargo) {
        check(it < hit_table.end());
        *it = c;
        it = advance(it);
    }
}

void damage_unit(unit_damage ud, std::vector<double> & rolls, int & roll_index)
{
    unit_t & unit = *ud.combat_unit_->unit_;
    unit_design_t const & design = *ud.combat_unit_->design_;
    float damage = ud.damage_;

    damage -= unit.shields;

    if (damage < 0.0f)
        return;

    float const armor_damage =
        std::min(damage * unit.armor / design.armor, unit.armor);
    unit.armor -= armor_damage;
    damage -= armor_damage;

    if (damage < 0.001)
        return;

    if (unit.hit_table.empty()) {
        unit.hit_table = design.hit_table;
        load_cargo(unit.cargo, unit.hit_table);
    }

    auto const roll_location = [&] {
        int const roll = next_roll(rolls, roll_index) * unit.hit_table.size();
        return roll == (int)unit.hit_table.size() ? roll - 1 : roll;
    };

    int hit_location = roll_location();
    for (int i = 0, last = int(damage + 0.5f); i < last; ++i) {
        if (!ud.missile_damage_)
            hit_location = roll_location();
        apply_hit(*ud.combat_unit_, hit_location, ignore_explosions_t::no);
    }
}

bool unit_disabled(combat_unit const & cu)
{
    if (cu.disabled_)
        return true;
    if (cu.unit_->fuel < 0.0000001f || cu.unit_->propulsion < 1.0f)
        return true;
    if (cu.unit_->crew / float(cu.design_->crew) <
        crew_onduty_factor * minimum_viable_crew_factor) {
        return true;
    }
    return false;
}

bool unit_destroyed(combat_unit const & cu)
{
    if (cu.destroyed_)
        return true;
    // TODO
    return false;
}

void attack(
    combat_unit & attacker,
    combat_unit & defender,
    std::vector<double> & rolls,
    int roll_index,
    std::vector<unit_damage> & damage,
    bool defender_is_from_side_2)
{
    bool const pd_attack = attacker.unit_->rounds;
    if (pd_attack)
        --attacker.unit_->rounds;

    bool missile_attack = attacker.unit_->missiles;
    if (missile_attack)
        --attacker.unit_->missiles;

    if (missile_attack && defender.unit_->rounds) {
        --defender.unit_->rounds;
        if (next_roll(rolls, roll_index) <
            pd_defense_probability(attacker, defender)) {
            missile_attack = false;
        }
    }

    if (!pd_attack && !missile_attack)
        return;

    float const attack_strength = attacker.unit_->weapons;
    if (pd_attack &&
        next_roll(rolls, roll_index) < pd_hit_probability(attacker, defender)) {
        damage.push_back(
            {&defender,
             attack_strength / 2.0f,
             false,
             defender_is_from_side_2});
    }
    if (missile_attack && next_roll(rolls, roll_index) <
                              missile_hit_probability(attacker, defender)) {
        damage.push_back(
            {&defender, attack_strength / 2.0f, true, defender_is_from_side_2});
    }
}

combat_unit & pick_target(
    combat_unit & attacker,
    combat_units & side,
    std::vector<double> & rolls,
    int & roll_index)
{
    if (attacker.prev_target_ && !attacker.prev_target_->destroyed_ &&
        !attacker.prev_target_->disabled_) {
        if (next_roll(rolls, roll_index) <
            keep_previous_combat_target_probability) {
            return *attacker.prev_target_;
        }
    }
    int const roll = next_roll(rolls, roll_index) * side.target_table_.size();
    int const i = roll == side.target_table_.size() ? roll - 1 : roll;
    return side.combat_units_[side.target_table_[i]];
}

void battle_round(
    combat_units & side_1,
    combat_units & side_2,
    std::vector<double> & rolls,
    int roll_index,
    std::vector<unit_damage> & damage)
{
    damage.clear();
    for (auto & attacker : side_1.combat_units_) {
        if (attacker.destroyed_ || attacker.disabled_)
            continue;
        auto & defender = pick_target(attacker, side_2, rolls, roll_index);
        attacker.prev_target_ = &defender;
        attack(attacker, defender, rolls, roll_index, damage, true);
    }
    for (auto & attacker : side_2.combat_units_) {
        if (attacker.destroyed_ || attacker.disabled_)
            continue;
        auto & defender = pick_target(attacker, side_1, rolls, roll_index);
        attacker.prev_target_ = &defender;
        attack(attacker, defender, rolls, roll_index, damage, false);
    }
    for (auto const & d : damage) {
        damage_unit(d, rolls, roll_index);
        if (unit_disabled(*d.combat_unit_))
            (d.unit_is_from_side_2_ ? side_2 : side_1).disable(*d.combat_unit_);
        if (unit_destroyed(*d.combat_unit_))
            (d.unit_is_from_side_2_ ? side_2 : side_1).destroy(*d.combat_unit_);
    }
}

void battle(
    game_state_t const & gs,
    std::vector<fleet_t *> const & fleets_1,
    std::vector<fleet_t *> const & fleets_2,
    std::vector<double> & rolls,
    int roll_index,
    std::vector<unit_damage> & damage)
{
    combat_units side_1(gs, fleets_1);
    combat_units side_2(gs, fleets_2);
    bool keep_fighting = false;
    while (keep_fighting) {
        battle_round(side_1, side_2, rolls, roll_index, damage);
    }
}

// TODO: Take unit and fleet XP into account.
void encounter(
    game_state_t const & gs,
    std::vector<fleet_t *> const & fleets_1,
    std::vector<fleet_t *> const & fleets_2,
    std::vector<double> & rolls,
    int roll_index,
    std::vector<unit_damage> & damage)
{
    // TODO: handle all the posible cases of fleets wanting to engage or not,
    // etc.
    bool fight = false; // TODO
    if (fight) {
        std::vector<unit_damage> damage;
        battle(gs, fleets_1, fleets_2, rolls, roll_index, damage);
    }
}
