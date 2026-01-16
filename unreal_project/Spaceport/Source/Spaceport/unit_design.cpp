#include "unit_design.hpp"

#include "constants.hpp"
#include "game_data_metadata.hpp"

#include <numeric>


namespace {
    float
    hull_novelty_distance(int curr_hull, int prev_hull, reason r, reasons * rs)
    {
        float retval = std::abs(float(prev_hull - curr_hull)) / prev_hull;
        if (retval < negligible_hull_size_design_difference)
            retval = 0.0f;
        else if (rs)
            rs->insert(std::move(r));
        return retval;
    }
}

namespace detail {
    enum struct design_grade_t : int { f, c, b, a };
    auto operator<=>(design_grade_t l, design_grade_t r)
    {
        return (int)l <=> (int)r;
    }

    design_grade_t design_grade(float design_quality)
    {
        if (0.9f < design_quality)
            return design_grade_t::a;
        if (0.8f < design_quality)
            return design_grade_t::b;
        if (0.7f < design_quality)
            return design_grade_t::c;
        return design_grade_t::f;
    }

    void generate_hit_table(unit_design_t & design, float design_quality)
    {
        design_grade_t const design_grade =
            detail::design_grade(design_quality);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);

        std::vector<hit_table_block> blocks;
        blocks.reserve(1024);
        blocks.insert(
            blocks.end(),
            {{hit_table_entry_t::hit_crew_space, crew_space},
             {hit_table_entry_t::hit_propulsion, design.propulsion_space}});
        if (design_grade == design_grade_t::f)
            blocks.push_back({hit_table_entry_t::hit_fuel, design.fuel});
        blocks.push_back(
            {hit_table_entry_t::hit_weapons, design.weapons_space});
        if (design_grade == design_grade_t::f) {
            blocks.insert(
                blocks.end(),
                {hit_table_entry_t::hit_rounds,
                 design.rounds + design.missiles});
        }
        blocks.insert(
            blocks.end(),
            {{hit_table_entry_t::hit_shields, design.shields_space},
             {hit_table_entry_t::hit_detection, design.detection_space},
             {hit_table_entry_t::hit_stealth, design.stealth_space},
             {hit_table_entry_t::hit_water, design.water},
             {hit_table_entry_t::hit_supplies, design.supplies},
             {hit_table_entry_t::hit_fighters, design.fighters},
             {hit_table_entry_t::hit_cargo, design.cargo}});
        if (design_grade != design_grade_t::f) {
            blocks.insert(
                blocks.end(),
                {{hit_table_entry_t::hit_fuel, design.fuel},
                 {hit_table_entry_t::hit_rounds,
                  design.rounds + design.missiles}});
        }

        int unused = call_lua_func("unit_unused_space", design);
        if (unused < 0)
            return;

        auto const first_cargo = [&] {
            return std::ranges::find(
                blocks, hit_table_entry_t::hit_cargo, &hit_table_block::entry_);
        };
        hit_table_block * initial_cargo = nullptr;
        if (auto it = first_cargo(); it != blocks.end())
            initial_cargo = &*it;

        auto const num_spacers = [&] {
            return unused + (initial_cargo ? initial_cargo->size_ : 0);
        };
        auto const insert_spacers = [&](auto it, int n) {
            if (design_grade == design_grade_t::a && 0 < unused) {
                int const unused_to_place = std::min(n, unused);
                if constexpr (requires { *it = (signed char)0; }) {
                    it = std::fill_n(
                        it,
                        unused_to_place,
                        (signed char)hit_table_entry_t::hit_unused);
                } else {
                    it = blocks.insert(
                        it,
                        hit_table_block{
                            hit_table_entry_t::hit_unused, unused_to_place});
                }
                n -= unused_to_place;
                unused -= unused_to_place;
            }
            if (initial_cargo && 0 < initial_cargo->size_) {
                int const cargo_to_place = std::min(n, initial_cargo->size_);
                if constexpr (requires { *it = (signed char)0; }) {
                    it = std::fill_n(
                        it,
                        cargo_to_place,
                        (signed char)hit_table_entry_t::hit_cargo);
                } else {
                    it = blocks.insert(
                        it,
                        hit_table_block{
                            hit_table_entry_t::hit_cargo, cargo_to_place});
                }
                n -= cargo_to_place;
                initial_cargo->size_ -= cargo_to_place;
            }
            if (0 < unused) {
                int const unused_to_place = std::min(n, unused);
                if constexpr (requires { *it = (signed char)0; }) {
                    it = std::fill_n(
                        it,
                        unused_to_place,
                        (signed char)hit_table_entry_t::hit_unused);
                } else {
                    it = blocks.insert(
                        it,
                        hit_table_block{
                            hit_table_entry_t::hit_unused, unused_to_place});
                }
                n -= unused_to_place;
                unused -= unused_to_place;
            }
            check(!n);
            return it;
        };

        auto const first_fuel = [&] {
            return std::ranges::find(
                blocks, hit_table_entry_t::hit_fuel, &hit_table_block::entry_);
        };
        auto const first_ammo = [&] {
            return std::ranges::find(
                blocks,
                hit_table_entry_t::hit_rounds,
                &hit_table_block::entry_);
        };

        // Except in the worst designs, put empty spaces around fuel and ammo
        // to prevent chain reactions.
        if (design_grade == design_grade_t::f) {
            // nothing to do here
        } else if (num_spacers() == 0) {
            // nothing to do here
        } else if (num_spacers() == 1) {
            // TODO: Use explosion radius constants.
            insert_spacers(first_fuel(), 1);
        } else if (num_spacers() == 2) {
            insert_spacers(first_fuel(), 1);
            insert_spacers(first_ammo(), 1);
        } else {
            insert_spacers(first_fuel(), 1);
            insert_spacers(first_ammo(), 2);
        }

        // Move all remaining cargo into this block, so that it does not get
        // placed before-and-if it is used as spacing.
        hit_table_block remaining_cargo;
        if (initial_cargo && initial_cargo->size_) {
            remaining_cargo = *initial_cargo;
            initial_cargo->size_ = 0;
            initial_cargo = &remaining_cargo;
        }

        std::vector<signed char> & table = design.hit_table;
        table.resize(design.hull, 0);
        auto it = table.begin();
        for (auto & block : blocks) {
            if (block.entry_ == hit_table_entry_t::hit_fuel &&
                design_grade_t::c < design_grade && 0 < num_spacers() &&
                1 < block.size_) {
                int const spacings = std::min(block.size_ - 1, num_spacers());
                int const segment_size = block.size_ / (spacings + 1);
                for (int i = 0; i < spacings; ++i) {
                    block.size_ -= segment_size;
                    it = std::fill_n(
                        it, segment_size, (signed char)block.entry_);
                    it = insert_spacers(it, 1);
                }
                it = std::fill_n(it, block.size_, (signed char)block.entry_);
            } else if (block.entry_ == hit_table_entry_t::hit_rounds) {
                int rounds = design.rounds;
                int missiles = design.missiles;
                if (design_grade_t::c < design_grade && 1 < num_spacers() &&
                    1 < block.size_) {
                    int const spacings =
                        std::min(block.size_ - 1, num_spacers() / 2);
                    if (spacings) {
                        int const segment_size = block.size_ / (spacings + 1);
                        for (int i = 0; i < spacings; ++i) {
                            int const rounds_to_place =
                                std::min(rounds, segment_size);
                            rounds -= rounds_to_place;
                            int const missiles_to_place = std::min(
                                missiles, segment_size - rounds_to_place);
                            missiles -= missiles_to_place;
                            it = std::fill_n(
                                it,
                                rounds_to_place,
                                (signed char)hit_table_entry_t::hit_rounds);
                            it = std::fill_n(
                                it,
                                missiles_to_place,
                                (signed char)hit_table_entry_t::hit_missiles);
                            it = insert_spacers(it, 2);
                        }
                    }
                }
                it = std::fill_n(
                    it, rounds, (signed char)hit_table_entry_t::hit_rounds);
                it = std::fill_n(
                    it, missiles, (signed char)hit_table_entry_t::hit_missiles);
            } else {
                it = std::fill_n(it, block.size_, (signed char)block.entry_);
            }
        }
        it =
            std::fill_n(it, unused, (signed char)hit_table_entry_t::hit_unused);
        if (initial_cargo) {
            // Wait to place cargo until we know how much is left over.  Some
            // of it may have been used as spacing.
            it = std::fill_n(
                it,
                remaining_cargo.size_,
                (signed char)hit_table_entry_t::hit_cargo);
        }
        check(it == table.end());
    }

    void handle_too_little_space(unit_design_t & design)
    {
        int unused_space = call_lua_func("unit_unused_space", design);
        if (unused_space < 0) {
            int const cargo_unused_min = std::min(design.cargo, -unused_space);
            design.cargo -= cargo_unused_min;
            unused_space += cargo_unused_min;
        }
        while (unused_space < 0) {
            if (unused_space < 0 && 0 < design.fighters) {
                --design.fighters;
                ++unused_space;
            }
            if (unused_space < 0 && 0 < design.missiles) {
                --design.missiles;
                ++unused_space;
            }
            if (unused_space < 0 && 0 < design.rounds) {
                --design.rounds;
                ++unused_space;
            }
            if (unused_space < 0 && 0 < design.supplies) {
                --design.supplies;
                ++unused_space;
                if (design.supplies == 0) {
                    design.failed_design = true;
                    return;
                }
            }
            if (unused_space < 0 && 0 < design.water) {
                --design.water;
                ++unused_space;
                if (design.water == 0) {
                    design.failed_design = true;
                    return;
                }
            }
            if (unused_space < 0 && 0 < design.fuel) {
                --design.fuel;
                ++unused_space;
                if (design.fuel == 0) {
                    design.failed_design = true;
                    return;
                }
            }
        }
    }
}

float hull_size_novelty(
    unit_design_t const & design, nation_t const & nation, reasons * reasons_)
{
    float distance_from_nearest_hull_size = 0.0f;

    auto completed_designs =
        nation.unit_designs | std::views::filter([](auto const & e) {
            return e.design_months_remaining <= 0.0f;
        });
    auto const nearest_it = std::ranges::min_element(
        completed_designs, [h = design.hull](auto const & a, auto const & b) {
            return std::abs(a.hull - h) < std::abs(b.hull - h);
        });
    check(nearest_it != completed_designs.end());
    distance_from_nearest_hull_size = hull_novelty_distance(
        design.hull,
        nearest_it->hull,
        {"inexperience_with_hull_size"},
        reasons_);

    float nearness_to_max_hull_size = 0.0f;

    float const max_hull_size = call_lua_func("max_hull_space", nation);
    float const distance_from_max_hull_size =
        (max_hull_size - design.hull) / max_hull_size;
    if (distance_from_max_hull_size < close_to_max_hull_size) {
        auto const nearest_to_max_it = std::ranges::min_element(
            completed_designs,
            [h = max_hull_size](auto const & a, auto const & b) {
                return std::abs(a.hull - h) < std::abs(b.hull - h);
            });
        check(nearest_to_max_it != completed_designs.end());
        nearness_to_max_hull_size = hull_novelty_distance(
            max_hull_size,
            nearest_to_max_it->hull,
            {"near_max_hull_size"},
            reasons_);
    }

    return nearness_to_max_hull_size + distance_from_nearest_hull_size;
}

float design_novelty(
    unit_design_t const & design, nation_t const & nation, reasons * reasons_)
{
    float retval = 0.0f;
    retval += hull_size_novelty(design, nation, reasons_);
    retval += equipment_novelty(
        design,
        nation,
        detail::metadata<unit_design_t>::propulsion(),
        reasons_);
    retval += equipment_novelty(
        design, nation, detail::metadata<unit_design_t>::weapons(), reasons_);
    retval += equipment_novelty(
        design, nation, detail::metadata<unit_design_t>::shields(), reasons_);
    retval += equipment_novelty(
        design, nation, detail::metadata<unit_design_t>::detection(), reasons_);
    retval += equipment_novelty(
        design, nation, detail::metadata<unit_design_t>::stealth(), reasons_);
    return retval;
}

float design_months(
    unit_design_t const & design, nation_t const & nation, reasons * reasons_)
{
    return base_unit_design_months +
           unit_design_months_per_novelty_point *
               design_novelty(design, nation, reasons_);
}

float design_monthly_cost(
    unit_design_t const & design, nation_t const & nation, reasons * reasons_)
{
    cost_t const unit_cost = call_lua_func("unit_cost", design);
    return base_unit_design_monthly_cost_factor * unit_cost.money_cost +
           unit_design_monthly_cost_factor_per_novelty_point *
               design_novelty(design, nation, reasons_);
}

// If the design is not finished, updates design_months_remaining, and adds
// the raw roll (in [0, 100]) to design.hit_table.  Otherwise, finializes teh
// design, assigns all the final values, and fills out the proper
// design.hit_table.
void roll_design_month(unit_design_t & design, nation_t const & nation)
{
    // The roll for each equipment tech is normally dsitributed, and centered
    // within the gap between {tech}_tradition (in [0, 100]) and max tradition
    // (100).
    std::normal_distribution<double> unit_normal_dist(0.5, 0.1666667);
    auto const unit_normal = [&] {
        return std::clamp(random_number(unit_normal_dist), 0.0, 1.0);
    };

    // [0.0, 100.0]
    std::array<double, 6> rolls = {
        {std::lerp(nation.construction_tradition, 100.0, unit_normal()),
         std::lerp(nation.propulsion_tradition, 100.0, unit_normal()),
         std::lerp(nation.weapons_tradition, 100.0, unit_normal()),
         std::lerp(nation.shields_tradition, 100.0, unit_normal()),
         std::lerp(nation.stealth_tradition, 100.0, unit_normal()),
         std::lerp(nation.detection_tradition, 100.0, unit_normal())}};

    double const mean_roll =
        std::accumulate(rolls.begin(), rolls.end(), 0.0) / double(rolls.size());
    design.design_months_remaining -= mean_roll / 100.0;

    // Before the design is fully rolled, hit_table is not in use.  Use it to
    // store the rolls above; each value is [0, 100].
    design.hit_table.insert(design.hit_table.end(), rolls.begin(), rolls.end());

    if (0.0f < design.design_months_remaining)
        return;

    detail::reconstruct_mean_rolls(design, rolls);
    auto const effectiveness_rolls =
        detail::space_and_effectiveness_from_rolls(rolls);

    design.propulsion_space =
        design.propulsion * 10 *
        std::min(1.0 / rolls[1], max_unit_design_space_factor);
    design.weapons_space =
        design.weapons * 10 *
        std::min(1.0 / rolls[2], max_unit_design_space_factor);
    design.shields_space =
        design.shields * 10 *
        std::min(1.0 / rolls[3], max_unit_design_space_factor);
    design.detection_space =
        design.detection * 10 *
        std::min(1.0 / rolls[4], max_unit_design_space_factor);
    design.stealth_space =
        design.stealth * 10 *
        std::min(1.0 / rolls[5], max_unit_design_space_factor);

    detail::handle_too_little_space(design);

    design.effective_propulsion =
        design.propulsion *
        std::max(effectiveness_rolls[1], min_unit_design_effectiveness_factor);
    design.effective_weapons =
        design.weapons *
        std::max(effectiveness_rolls[2], min_unit_design_effectiveness_factor);
    design.effective_shields =
        design.shields *
        std::max(effectiveness_rolls[3], min_unit_design_effectiveness_factor);
    design.effective_detection =
        design.detection *
        std::max(effectiveness_rolls[4], min_unit_design_effectiveness_factor);
    design.effective_stealth =
        design.stealth *
        std::max(effectiveness_rolls[5], min_unit_design_effectiveness_factor);

    design.propulsion_reliability = rolls[1];
    design.weapons_reliability = rolls[2];
    design.shields_reliability = rolls[3];
    design.detection_reliability = rolls[4];
    design.stealth_reliability = rolls[5];

    design.known_propulsion_failures = 0;
    design.known_weapons_failures = 0;
    design.known_shields_failures = 0;
    design.known_detection_failures = 0;
    design.known_stealth_failures = 0;

    design.design_months_remaining = 0.0f;

    detail::generate_hit_table(design, mean_roll / 100.0);
}
