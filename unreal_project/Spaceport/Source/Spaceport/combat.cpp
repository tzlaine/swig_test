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
        call_lua_func("unit_max_acceleration", *cu.unit_, *cu.design_);
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

    auto const is_fuel = [&](int entry) {
        return entry == (signed char)hit_table_entry_t::hit_fuel ||
               entry == (signed char)cargo_kind_t::fuel;
    };
    auto const is_ammo = [&](int entry) {
        return entry == (signed char)hit_table_entry_t::hit_rounds ||
               entry == (signed char)hit_table_entry_t::hit_missiles ||
               entry == (signed char)cargo_kind_t::rounds ||
               entry == (signed char)cargo_kind_t::missiles;
    };

    if (table[hit_location] < 0) {
        signed char const cargo_kind[1] = {table[hit_location]};
        auto end_r = std::ranges::find_end(unit.cargo, cargo_kind);
        check(!end_r.empty());
        unit.cargo.erase(end_r.begin());
        if (is_fuel(table[hit_location])) {
            table[hit_location] = (signed char)hit_table_entry_t::hit_fuel;
            apply_hit(cu, hit_location, ignore_explosions);
        } else if (is_ammo(table[hit_location])) {
            table[hit_location] = (signed char)hit_table_entry_t::hit_rounds;
            apply_hit(cu, hit_location, ignore_explosions);
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
        unit.water -= unit.water / design.water;
        destroy_location(false);
        break;
    case hit_table_entry_t::hit_supplies:
        unit.supplies -= unit.supplies / design.supplies;
        destroy_location(false);
        break;

    case hit_table_entry_t::hit_fuel:
    case hit_table_entry_t::hit_rounds:
    case hit_table_entry_t::hit_missiles: {
        if (ignore_explosions == ignore_explosions_t::yes) {
            if (entry == hit_table_entry_t::hit_fuel)
                unit.fuel -= unit.fuel / design.fuel;
            if (entry == hit_table_entry_t::hit_rounds)
                unit.rounds -= unit.rounds / design.rounds;
            if (entry == hit_table_entry_t::hit_missiles)
                unit.missiles -= unit.missiles / design.missiles;
            destroy_location(false);
            break;
        }

        int explosion_lo = hit_location;
        int explosion_hi = hit_location + 1;

        for (int i = hit_location; 0 <= i && explosion_lo <= i; --i) {
            if (is_fuel(table[i])) {
                explosion_lo =
                    std::min(explosion_lo, i - fuel_explosion_radius);
                explosion_hi =
                    std::max(explosion_hi, i + 1 + fuel_explosion_radius);
            } else if (is_ammo(table[i])) {
                explosion_lo =
                    std::min(explosion_lo, i - ammo_explosion_radius);
                explosion_hi =
                    std::max(explosion_hi, i + 1 + ammo_explosion_radius);
            }
        }

        for (int i = hit_location, last = (int)table.size();
             i < last && i < explosion_hi;
             ++i) {
            if (is_fuel(table[i])) {
                explosion_lo =
                    std::min(explosion_lo, i - fuel_explosion_radius);
                explosion_hi =
                    std::max(explosion_hi, i + 1 + fuel_explosion_radius);
            } else if (is_ammo(table[i])) {
                explosion_lo =
                    std::min(explosion_lo, i - ammo_explosion_radius);
                explosion_hi =
                    std::max(explosion_hi, i + 1 + ammo_explosion_radius);
            }
        }

        explosion_lo = std::max(0, explosion_lo);
        explosion_hi = std::min(explosion_hi, (int)table.size());

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

    case hit_table_entry_t::hit_cargo: destroy_location(false); break;

    case hit_table_entry_t::hit_destroyed: {
        auto it = find_nearest_if(table, hit_location, [](auto entry) {
            return hit_table_entry_t(entry) != hit_table_entry_t::hit_destroyed;
        });
        if (it != table.end())
            apply_hit(cu, it - table.begin(), ignore_explosions);
        break;
    }

    case hit_table_entry_t::hit_crew_space: {
        float const assigned_crew = 1000.0f / space_required_per_1k_crew;
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
        apply_hit(*ud.combat_unit_, hit_location);
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
    int & roll_index,
    std::vector<unit_damage> & damage)
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
        damage.push_back({&defender, attack_strength / 2.0f, false});
    }
    if (missile_attack && next_roll(rolls, roll_index) <
                              missile_hit_probability(attacker, defender)) {
        damage.push_back({&defender, attack_strength / 2.0f, true});
    }
}

combat_unit & pick_target(
    combat_unit & attacker,
    combat_units & side,
    std::vector<double> & rolls,
    int & roll_index)
{
    check(!side.target_table_.empty());

    if (attacker.prev_target_ && unit_still_viable(*attacker.prev_target_)) {
        if (next_roll(rolls, roll_index) <
            keep_previous_combat_target_probability) {
            return *attacker.prev_target_;
        }
    }

    int const roll = next_roll(rolls, roll_index) * side.target_table_.size();
    int const i = roll == side.target_table_.size() ? roll - 1 : roll;
    combat_unit & target = side.combat_units_[side.target_table_[i]];
    if (!unit_still_viable(target)) {
        auto const it =
            find_nearest_if(side.combat_units_, i, &unit_still_viable);
        // If you're seeing this check fail, it means that the combat should
        // already have been ended, since 'side' contains no valid targets.
        check(it != side.combat_units_.end());
        if (it == side.combat_units_.end())
            return target;
        return *it;
    }

    return target;
}

battle_round_result battle_round(
    combat_units & side_1,
    combat_units & side_2,
    std::vector<double> & rolls,
    int & roll_index,
    std::vector<unit_damage> & damage,
    battle_round_kind_t kind)
{
    damage.clear();

    for (auto & attacker : side_1.combat_units_) {
        if (!unit_still_viable(attacker))
            continue;
        auto & defender = pick_target(attacker, side_2, rolls, roll_index);
        attacker.prev_target_ = &defender;
        attack(attacker, defender, rolls, roll_index, damage);
    }
    float const side_1_damage = std::transform_reduce(
        damage.begin(), damage.end(), 0.0f, std::plus{}, [](auto const & e) {
            return e.damage_;
        });

    int const damage_first_index = damage.size();
    for (auto & attacker : side_2.combat_units_) {
        if (!unit_still_viable(attacker))
            continue;
        auto & defender = pick_target(attacker, side_1, rolls, roll_index);
        attacker.prev_target_ = &defender;
        attack(attacker, defender, rolls, roll_index, damage);
    }
    float const side_2_damage = std::transform_reduce(
        damage.begin() + damage_first_index,
        damage.end(),
        0.0f,
        std::plus{},
        [](auto const & e) { return e.damage_; });

    if (kind != battle_round_kind_t::simulated) {
        for (auto const & d : damage) {
            damage_unit(d, rolls, roll_index);
        }
        for (auto & cu : side_1.combat_units_) {
            if (unit_disabled(cu))
                side_1.disable(cu);
            if (unit_destroyed(cu))
                side_1.destroy(cu);
        }
    }

    return {side_1_damage, side_2_damage};
}

battle_result battle(
    combat_units & side_1,
    combat_units & side_2,
    std::vector<fleet_t *> const & fleets_1,
    std::vector<fleet_t *> const & fleets_2,
    std::vector<double> & rolls,
    int roll_index,
    std::vector<unit_damage> & damage)
{
    while (true) {
        auto [side_1_damage, side_2_damage] =
            battle_round(side_1, side_2, rolls, roll_index, damage);
        if (!side_1.still_fighting_ || !side_2.still_fighting_) {
            return {
                side_1.still_fighting_ ? battle_result_t::controls_ao
                                       : battle_result_t::destroyed,
                side_2.still_fighting_ ? battle_result_t::controls_ao
                                       : battle_result_t::destroyed};
        }
        if (side_1_damage < 0.001 || side_2_damage < 0.001) {
            battle_result_t side_1_result = battle_result_t::retreated;
            battle_result_t side_2_result = battle_result_t::retreated;
            if (0.001 < side_1_damage && !can_escape_from(side_2, side_1)) {
                side_1_result = battle_result_t::controls_ao;
                side_2_result = battle_result_t::surrendered;
            }
            if (0.001 < side_2_damage && !can_escape_from(side_1, side_2)) {
                side_1_result = battle_result_t::surrendered;
                side_2_result = battle_result_t::controls_ao;
            }
            return {side_1_result, side_2_result};
        }

        float const side_1_damage_ratio = std::clamp(
            (side_1_damage - side_2_damage) / side_1_damage, -1.0f, 1.0f);
        float const side_2_damage_ratio = std::clamp(
            (side_2_damage - side_1_damage) / side_2_damage, -1.0f, 1.0f);

        bool const side_1_wants_out =
            std::ranges::any_of(fleets_1, [&](auto * f) {
                return side_1_damage_ratio < f->engagement_posture / 100.0f;
            });
        bool const side_2_wants_out =
            std::ranges::any_of(fleets_2, [&](auto * f) {
                return side_2_damage_ratio < f->engagement_posture / 100.0f;
            });

        if (side_1_wants_out && side_2_wants_out)
            return {battle_result_t::retreated, battle_result_t::retreated};

        if (side_1_wants_out) {
            // TODO: There may be an opportunity for interesting flavor events
            // here, if one or more of the fleets on a side abandon(s) the
            // remaining fleet(s), if the leving fleet(s) have the
            // acceleration to escape but the fleet(s) left behind do not.
            if (can_escape_from(side_1, side_2)) {
                return {battle_result_t::retreated, battle_result_t::controls_ao};
            } else {
                // A trapped fleet will fight like hell.
                for (auto & cu : side_1.combat_units_) {
                    cu.unit_->organization = (cu.unit_->organization + 100) / 2;
                }
            }
        } else if (side_2_wants_out) {
            if (can_escape_from(side_2, side_1)) {
                return {battle_result_t::controls_ao, battle_result_t::retreated};
            } else {
                for (auto & cu : side_2.combat_units_) {
                    cu.unit_->organization = (cu.unit_->organization + 100) / 2;
                }
            }
        }
    }
}

// TODO: Attacks should cause weapons and shield failure rolls, based on
// reliability ratings for those systems.

// TODO: Propulsion reliability rolls during combat too.

// TODO: Propulsion and shield reliability rolls should happen when moving
// through subspace.

// TODO: Take unit and fleet XP into account.

// TODO: Take unit org into account.

void encounter(
    game_state_t const & gs,
    std::vector<fleet_t *> const & fleets_1,
    std::vector<fleet_t *> const & fleets_2,
    std::vector<double> & rolls,
    int roll_index,
    std::vector<unit_damage> & damage)
{
    combat_units side_1(gs, fleets_1);
    combat_units side_2(gs, fleets_2);

    // TODO: handle all the posible cases of fleets wanting to engage or not,
    // etc.
    battle(side_1, side_2, fleets_1, fleets_2, rolls, roll_index, damage);
}
