#include "unit_design.hpp"

#include "constants.hpp"
#include "game_data_metadata.hpp"
#include "rng.hpp"

#include <numeric>


float hull_size_novelty(
    unit_design_t const & design, nation_t const & nation, reasons * reasons_)
{
    float const max_hull_size = call_lua_func("max_hull_space", nation);
    float nearness_to_max_hull_size =
        std::abs(max_hull_size - design.hull) / max_hull_size;
    if (nearness_to_max_hull_size < close_to_max_hull_size)
        nearness_to_max_hull_size = 0.0f;
    else if (reasons_)
        reasons_->insert({"near_hull_size_limit"});

    float distance_from_nearest_hull_size = 1.0;
    auto completed_designs =
        nation.unit_designs | std::views::filter([](auto const & e) {
            return e.design_months_remaining <= 0.0f;
        });
    auto const it = std::ranges::min_element(
        completed_designs, [h = design.hull](auto const & a, auto const & b) {
            return std::abs(a.hull - h) < std::abs(b.hull - h);
        });
    if (it != completed_designs.end()) {
        distance_from_nearest_hull_size =
            std::abs(float(it->hull - design.hull)) / it->hull;
        if (distance_from_nearest_hull_size <
            negligible_hull_size_design_difference)
            distance_from_nearest_hull_size = 0.0f;
        else if (reasons_)
            reasons_->insert({"inexperience_with_hull_size"});
    }

    return nearness_to_max_hull_size + distance_from_nearest_hull_size;
}

float design_novelty(
    unit_design_t const & design, nation_t const & nation, reasons * reasons_)
{
    float retval = 0.0f;
    retval += hull_size_novelty(design, nation, reasons_);
    detail::metadata<unit_design_t>::foreach_member([&](auto const & meta) {
        retval += equipment_novelty(design, nation, meta, reasons_);
    });
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

namespace detail {
    void generate_hit_table(unit_design_t & design, float design_quality)
    {
        std::vector<hit_table_block> blocks;
        blocks.reserve(1024);
        blocks = {
            {hit_table_entry_t::hit_propulsion, design.propulsion_space},
            {hit_table_entry_t::hit_weapons, design.weapons_space},
            {hit_table_entry_t::hit_shields, design.shields_space},
            {hit_table_entry_t::hit_detection, design.detection_space},
            {hit_table_entry_t::hit_stealth, design.stealth_space},
            {hit_table_entry_t::hit_water, design.water},
            {hit_table_entry_t::hit_supplies, design.supplies},
            {hit_table_entry_t::hit_fighters, design.fighters},
            {hit_table_entry_t::hit_cargo, design.cargo},
            {hit_table_entry_t::hit_fuel, design.fuel},
            {hit_table_entry_t::hit_rounds, design.rounds},
            {hit_table_entry_t::hit_missiles, design.missiles}};

        int unused = call_lua_func("unit_unused_space", design);
        for (auto const & block : blocks) {
            unused -= block.size_;
        }
        if (unused < 0)
            return;

        auto const first_fuel = [&] {
            return std::ranges::find(
                blocks, hit_table_entry_t::hit_fuel, &hit_table_block::entry_);
        };
        auto const first_ammo = [&] {
            return std::ranges::find_if(
                blocks,
                [](auto const & e) {
                    return e == hit_table_entry_t::hit_rounds ||
                           e == hit_table_entry_t::hit_missiles;
                },
                &hit_table_block::entry_);
        };

        // Put empty spaces around fuel and ammo to prevent chain reactions.
        if (unused == 0) {
            // nothing to do here
        } else if (unused == 1) {
            blocks.insert(
                first_fuel(),
                hit_table_block{hit_table_entry_t::hit_unused, 1});
            unused = 0;
        } else if (unused == 2) {
            blocks.insert(
                first_fuel(),
                hit_table_block{hit_table_entry_t::hit_unused, 1});
            blocks.insert(
                first_ammo(),
                hit_table_block{hit_table_entry_t::hit_unused, 1});
            unused = 0;
        } else {
            blocks.insert(
                first_fuel(),
                hit_table_block{hit_table_entry_t::hit_unused, 1});
            blocks.insert(
                first_ammo(),
                hit_table_block{hit_table_entry_t::hit_unused, 2});
            unused -= 3;
        }

        std::vector<signed char> & table = design.hit_table;
        table.resize(design.hull);
        auto it = table.begin();
        for (auto & block : blocks) {
            if (0 < unused && 1 < design.fuel &&
                block.entry_ == hit_table_entry_t::hit_fuel) {
                int const segments = std::min(block.size_, unused);
                int const segment_size = block.size_ / segments;
                for (int i = 0; i < segments; ++i) {
                    block.size_ -= segment_size;
                    it = std::fill_n(
                        it, segment_size, (signed char)block.entry_);
                    *it++ = (signed char)hit_table_entry_t::hit_unused;
                }
                it = std::fill_n(it, block.size_, (signed char)block.entry_);
            } else if (
                1 < unused && (1 < design.rounds &&
                               block.entry_ == hit_table_entry_t::hit_rounds) ||
                (1 < design.missiles &&
                 block.entry_ == hit_table_entry_t::hit_missiles)) {
                bool const rounds_and_missiles =
                    !!design.rounds && !!design.missiles;
                int const segments = std::min(
                    block.size_, rounds_and_missiles ? unused / 4 : unused / 2);
                int const segment_size = block.size_ / segments;
                for (int i = 0; i < segments; ++i) {
                    block.size_ -= segment_size;
                    it = std::fill_n(
                        it, segment_size, (signed char)block.entry_);
                    *it++ = (signed char)hit_table_entry_t::hit_unused;
                    *it++ = (signed char)hit_table_entry_t::hit_unused;
                }
                it = std::fill_n(it, block.size_, (signed char)block.entry_);
            } else {
                it = std::fill_n(
                    it, block.size_, (signed char)block.entry_);
            }
        }
    }
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

    design.hit_table.insert(design.hit_table.end(), rolls.begin(), rolls.end());

    if (0.0f < design.design_months_remaining)
        return;

    // reconstruct mean roll (in [0.0, 1.0]) for each equipment
    {
        int months = 0;
        auto table_it = design.hit_table.begin();
        while (table_it != design.hit_table.end()) {
            table_it =
                std::ranges::transform(
                    std::ranges::subrange(table_it, table_it + rolls.size()),
                    rolls,
                    rolls.begin(),
                    [](int curr, double roll) { return roll + curr; })
                    .in1;
            ++months;
        }
        std::ranges::transform(rolls, rolls.begin(), [months](double roll) {
            return roll / months;
        });
    }

    // A bad roll affects both space and effectiveness.  How much of each
    // depends on another roll....
    decltype(rolls) effectiveness_rolls = rolls;
    for (int i = 0, last = (int)rolls.size(); i < last; ++i) {
        double const space_effectiveness_split_roll = random_unit_double();
        rolls[i] = 1.0 - (1.0 - rolls[i]) * space_effectiveness_split_roll;
        effectiveness_rolls[i] =
            1.0 - (1.0 - effectiveness_rolls[i]) *
                      (1.0 - space_effectiveness_split_roll);
    }

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

    int unused_space = call_lua_func("unit_unused_space", design);
    if (unused_space < 0) {
        int const cargo_unused_min = std::min(design.cargo, -unused_space);
        design.cargo -= cargo_unused_min;
        unused_space += cargo_unused_min;
    }
    while (unused_space < 0) {
        if (unused_space < 0) {
            --design.fighters;
            ++unused_space;
        }
        if (unused_space < 0) {
            --design.missiles;
            ++unused_space;
        }
        if (unused_space < 0) {
            --design.rounds;
            ++unused_space;
        }
        if (unused_space < 0) {
            --design.supplies;
            ++unused_space;
            if (design.supplies == 0)
                break;
        }
        if (unused_space < 0) {
            --design.water;
            ++unused_space;
            if (design.water == 0)
                break;
        }
        if (unused_space < 0) {
            --design.fuel;
            ++unused_space;
            if (design.fuel == 0)
                break;
        }
    }

    if (unused_space < 0)
        design.failed_design = true;

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
