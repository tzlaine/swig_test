#pragma once

#include "game_data.hpp"
#include "lua.hpp"
#include "reason.hpp"
#include "rng.hpp"

#include <ranges>


namespace detail {
    struct hit_table_block
    {
        hit_table_entry_t entry_ = hit_table_entry_t::invalid_hit_table_entry;
        int size_ = 0;
    };
    void generate_hit_table(unit_design_t & design, float design_quality);

    template<int N>
    void reconstruct_mean_rolls(
        unit_design_t const & design, std::array<double, N> & rolls)
    {
        // reconstruct mean roll (in [0.0, 1.0]) for each equipment
        {
            int months = 0;
            std::ranges::fill(rolls, 0.0);
            auto table_it = design.hit_table.begin();
            while (table_it != design.hit_table.end()) {
                table_it = std::ranges::transform(
                               std::ranges::subrange(
                                   table_it, table_it + rolls.size()),
                               rolls,
                               rolls.begin(),
                               [](int curr, double roll) {
                                   return curr / 100.0 + roll;
                               })
                               .in1;
                ++months;
            }
            std::ranges::transform(rolls, rolls.begin(), [months](double roll) {
                return roll / months;
            });
        }
    }

    template<int N>
    auto space_and_effectiveness_from_rolls(std::array<double, N> & rolls)
    {
        // A bad roll affects both space and effectiveness.  How much of each
        // depends on another roll....
        auto retval = rolls;
        for (int i = 0, last = (int)rolls.size(); i < last; ++i) {
            double const space_effectiveness_split_roll = random_unit_double();
            rolls[i] = 1.0 - (1.0 - rolls[i]) * space_effectiveness_split_roll;
            retval[i] = 1.0 - (1.0 - retval[i]) *
                                  (1.0 - space_effectiveness_split_roll);
        }
        return retval;
    }

    void handle_too_little_space(unit_design_t & design);
}

float hull_size_novelty(
    unit_design_t const & design,
    nation_t const & nation,
    reasons * reasons_ = nullptr);

template<typename Meta>
float equipment_novelty(
    unit_design_t const & design,
    nation_t const & nation,
    Meta const & aspect,
    reasons * reasons_ = nullptr)
{
    int const prev_designs = std::ranges::count_if(
        nation.unit_designs | std::views::filter([](auto const & e) {
            return e.design_months_remaining <= 0.0f;
        }),
        [level = design.*aspect.ptr_, p = aspect.ptr_](auto const & e) {
            return e.*p == level;
        });
    std::vector<float> iteration_design_novelty =
        lua().get<std::vector<float>>("iteration_design_novelty");
    if (prev_designs < iteration_design_novelty.size()) {
        if (reasons_) {
            reasons_->insert(
                {std::format("inexperience_with_{0}_level", aspect.name_),
                 prev_designs});
        }
        return iteration_design_novelty[prev_designs];
    }
    return 0.0f;
}

float design_novelty(
    unit_design_t const & design,
    nation_t const & nation,
    reasons * reasons_ = nullptr);

float design_months(
    unit_design_t const & design,
    nation_t const & nation,
    reasons * reasons_ = nullptr);

float design_monthly_cost(
    unit_design_t const & design,
    nation_t const & nation,
    reasons * reasons_ = nullptr);

void roll_design_month(unit_design_t & design, nation_t const & nation);
