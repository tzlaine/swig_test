#pragma once

#include "check.hpp"
#include "constants.hpp"
#include "game_data.hpp"

#include <algorithm>
#include <numeric>
#include <vector>


struct combat_unit
{
    unit_t * unit_ = nullptr;
    combat_unit * prev_target_ = nullptr;
    unit_design_t const * design_ = nullptr;
    fleet_t * fleet_ = nullptr;
    nation_t const * nation_ = nullptr;
    float acceleration_ = 0.0f;
    int fleet_unit_index_ = -1;
    int combat_unit_index_ = -1;
    bool capital_ = false;
    bool destroyed_ = false;
    bool disabled_ = false;
};

inline bool valid_target(combat_unit const & cu)
{
    return !cu.destroyed_ && !cu.disabled_;
}

struct unit_damage
{
    combat_unit * combat_unit_ = nullptr;
    float damage_ = 0.0f;
    bool missile_damage_ = false;
};

inline double next_roll(std::vector<double> const & rolls, int & i)
{
    check(i < (int)rolls.size());
    ++i;
    if (i == (int)rolls.size())
        i = 0;
    return rolls[i];
}

float pd_defense_probability(
    combat_unit const & attacker, combat_unit const & defender);

float pd_hit_probability(
    combat_unit const & attacker, combat_unit const & defender);

float missile_hit_probability(
    combat_unit const & attacker, combat_unit const & defender);

float combat_acceleration(combat_unit const & cu);

// Evaluates elements (excluding begin(r) + pos), returning the one at
// position I for which pred(*(begin(r) + I)) is true, such that abs(pos - I)
// is minimized.  For N = abs(pos - i), returns i + N if pred(*(begin(r) + pos
// + N)) and pred(*(begin(r) + pos - N)) are both true.  Returns end(r) if no
// element meets the predicate.
template<std::ranges::random_access_range R, typename Pred>
std::ranges::borrowed_iterator_t<R>
find_nearest_if(R && r, int pos, Pred && pred)
{
    auto const size = std::ranges::size(r);
    check(0 <= pos && pos < size);
    auto const first = std::ranges::begin(r);
    for (int i = 1; i < size; ++i) {
        if (pos + i < size && pred(r[pos + i]))
            return first + (pos + i);
        if (0 <= pos - i && pred(r[pos - i]))
            return first + (pos - i);
    }
    return std::ranges::end(r);
}

// TODO: Add a pre-generated array of unit doubles, and use these instead of
// doing each roll individually.  This will make the code race free, and
// probably make it faster independently of that.  The array should be pretty
// big (maybe 8k values?), and there should be a seed that determines where to
// enter the array, and each subsequent read from the array should use modular
// indexing.

enum struct ignore_explosions_t { no, yes };

void apply_hit(
    combat_unit & cu,
    int hit_location,
    ignore_explosions_t ignore_explosions = ignore_explosions_t::no);

void load_cargo(
    std::vector<signed char> & cargo, std::vector<signed char> & hit_table);

void damage_unit(
    unit_damage damage, std::vector<double> & rolls, int & roll_index);

bool unit_disabled(combat_unit const & cu);
bool unit_destroyed(combat_unit const & cu);

void attack(
    combat_unit & attacker,
    combat_unit & defender,
    std::vector<double> & rolls,
    int & roll_index,
    std::vector<unit_damage> & damage);

struct combat_units
{
    combat_units() = delete;

    template<typename R>
    combat_units(game_state_t const & gs, R & fleets)
    {
        int n = 0;
        double mean_acceleration = 0.0;

        for (auto & f : fleets) {
            fleet_t * fleet_ptr = nullptr;
            if constexpr (std::is_pointer_v<
                              std::remove_reference_t<decltype(f)>>) {
                fleet_ptr = f;
            } else {
                fleet_ptr = &f;
            }
            fleet_t & fleet = *fleet_ptr;
            nation_t const & nation = gs.nations[fleet.id.nation_id];
            for (int i = 0, last = (int)fleet.units.size(); i < last; ++i) {
                unit_t & unit = fleet.units[i];
                unit_design_t const & design =
                    nation.unit_designs[unit.id.object_id];
                combat_units_.push_back({
                    .unit_ = &unit,
                    .design_ = &design,
                    .fleet_ = &fleet,
                    .nation_ = &nation,
                    .fleet_unit_index_ = i});
                combat_unit & just_added = combat_units_.back();
                float const acceleration = combat_acceleration(just_added);
                just_added.acceleration_ = acceleration;
                just_added.disabled_ = unit_disabled(just_added);
                ++n;
                mean_acceleration += acceleration;
            }
        }

        mean_acceleration /= n;

        for (auto & unit : combat_units_) {
            if (unit.acceleration_ < mean_acceleration)
                unit.capital_ = true;
        }

        auto const screens_it = std::partition(
            combat_units_.begin(), combat_units_.end(), [](auto const & e) {
                return e.capital_;
            });
        auto const num_capitals = screens_it - combat_units_.begin();
        auto const num_screens = combat_units_.end() - screens_it;
        target_table_.resize(
            capital_combat_selection_factor * num_capitals +
            screen_combat_selection_factor * num_screens);
        auto it = target_table_.begin();
        for (int i = 0, last = capital_combat_selection_factor; i < last; ++i) {
            std::iota(it, it + num_capitals, 0);
            it += num_capitals;
        }
        for (int i = 0, last = screen_combat_selection_factor; i < last; ++i) {
            std::iota(it, it + num_screens, num_capitals);
            it += num_screens;
        }

        int i = 0;
        for (auto & unit : combat_units_) {
            unit.combat_unit_index_ = i++;
        }
    }

    void disable(combat_unit & cu) { cu.disabled_ = true; }

    void destroy(combat_unit & cu)
    {
        cu.destroyed_ = true;
        std::erase(target_table_, cu.combat_unit_index_);
    }

    std::vector<combat_unit> combat_units_;
    std::vector<int> target_table_;
};

combat_unit & pick_target(
    combat_unit & attacker,
    combat_units & units,
    std::vector<double> & rolls,
    int & roll_index);

void battle_round(
    combat_units & side_1,
    combat_units & side_2,
    std::vector<double> & rolls,
    int & roll_index,
    std::vector<unit_damage> & damage);

void battle(
    combat_units & side_1,
    combat_units & side_2,
    std::vector<double> & rolls,
    int roll_index,
    std::vector<unit_damage> & damage);

// TODO: Also pass the model.  If not the model, pass something that can
// record the new orders after the engagement, ship lossses, etc.
void encounter(
    game_state_t const & gs,
    std::vector<fleet_t *> const & fleets_1,
    std::vector<fleet_t *> const & fleets_2,
    std::vector<double> & rolls,
    int roll_index,
    std::vector<unit_damage> & damage);
