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
    float initial_organization_ = 0.0f;
    float curr_organization_ = 0.0f;
    int fleet_unit_index_ = -1;
    int combat_unit_index_ = -1;
    bool capital_ = false;
    bool destroyed_ = false;
    bool disabled_ = false;
};

inline bool unit_still_viable(combat_unit const & cu)
{
    return !cu.destroyed_ && !cu.disabled_;
}

enum struct reliability_t {
    invalid_reliability,
    propulsion,
    weapons,
    shields,
    detection,
    stealth
};

struct unit_damage
{
    combat_unit * combat_unit_ = nullptr;
    float damage_ = 0.0f;
    bool missile_damage_ = false;
    reliability_t reliability_damage_ = reliability_t::invalid_reliability;
};

inline double next_roll(std::vector<double> const & rolls, int & i)
{
    check(i < (int)rolls.size());
    ++i;
    if (i == (int)rolls.size())
        i = 0;
    return rolls[i];
}

float effective_value(float base_value, combat_unit const & cu);

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

template<reliability_t Kind, auto Ptr>
std::optional<unit_damage> roll_reliability_impl(
    combat_unit & cu, std::vector<double> & rolls, int & roll_index)
{
    if (next_roll(rolls, roll_index) <
        std::pow(
            effective_value(cu.design_->*Ptr, cu) / 100.0f,
            1 / reliability_exponent)) {
        return unit_damage{.combat_unit_ = &cu, .reliability_damage_ = Kind};
    }
    return {};
}

template<reliability_t Kind>
std::optional<unit_damage> roll_reliability(
    combat_unit & cu, std::vector<double> & rolls, int & roll_index)
{
    switch (Kind) {
    case reliability_t::propulsion:
        return roll_reliability_impl<
            Kind,
            &unit_design_t::propulsion_reliability>(cu, rolls, roll_index);
    case reliability_t::weapons:
        return roll_reliability_impl<Kind, &unit_design_t::weapons_reliability>(
            cu, rolls, roll_index);
    case reliability_t::shields:
        return roll_reliability_impl<Kind, &unit_design_t::shields_reliability>(
            cu, rolls, roll_index);
    case reliability_t::detection:
        return roll_reliability_impl<
            Kind,
            &unit_design_t::detection_reliability>(cu, rolls, roll_index);
    case reliability_t::stealth:
        return roll_reliability_impl<Kind, &unit_design_t::stealth_reliability>(
            cu, rolls, roll_index);
    }
    return {};
}

bool attack(
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
                combat_units_.push_back(
                    {.unit_ = &unit,
                     .design_ = &design,
                     .fleet_ = &fleet,
                     .nation_ = &nation,
                     .initial_organization_ = unit.organization / 100.0f,
                     .curr_organization_ = unit.organization / 100.0f,
                     .fleet_unit_index_ = i});
                combat_unit & just_added = combat_units_.back();
                float const acceleration = combat_acceleration(just_added);
                just_added.acceleration_ = acceleration;
                just_added.disabled_ = unit_disabled(just_added);

                ++n;
                mean_acceleration += acceleration;

                if (min_acceleration_ == 0.0f)
                    min_acceleration_ = acceleration;
                min_acceleration_ = std::min(acceleration, min_acceleration_);
                if (max_acceleration_ == 0.0f)
                    max_acceleration_ = acceleration;
                max_acceleration_ = std::max(acceleration, max_acceleration_);
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

        still_fighting_ = (int)combat_units_.size();
    }

    void disable(combat_unit & cu)
    {
        if (unit_still_viable(cu))
            --still_fighting_;
        cu.disabled_ = true;
    }

    void destroy(combat_unit & cu)
    {
        if (unit_still_viable(cu))
            --still_fighting_;
        cu.destroyed_ = true;
        std::erase(target_table_, cu.combat_unit_index_);
    }

    std::vector<combat_unit> combat_units_;
    std::vector<int> target_table_;
    float min_acceleration_ = 0.0f;
    float max_acceleration_ = 0.0f;
    int still_fighting_ = 0;
};

inline bool
can_escape_from(combat_units const & side_1, combat_units const & side_2)
{
    return side_2.max_acceleration_ - 0.1f < side_1.min_acceleration_;
}

combat_unit & pick_target(
    combat_unit & attacker,
    combat_units & units,
    std::vector<double> & rolls,
    int & roll_index);

struct battle_round_result
{
    float side_1_damage_ = 0.0f;
    float side_2_damage_ = 0.0f;
};

enum struct battle_round_kind_t { simulated, real };

battle_round_result battle_round(
    combat_units & side_1,
    combat_units & side_2,
    std::vector<double> & rolls,
    int & roll_index,
    std::vector<unit_damage> & damage,
    battle_round_kind_t kind = battle_round_kind_t::real);

enum struct battle_result_t { controls_ao, destroyed, surrendered, retreated };

struct battle_result
{
    battle_result_t side_1_result_ = battle_result_t::retreated;
    battle_result_t side_2_result_ = battle_result_t::retreated;
};

battle_result battle(
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
