#pragma once

#include "game_data.hpp"
#include "lua.hpp"
#include "reason.hpp"

#include <ranges>


namespace detail {
    struct hit_table_block
    {
        hit_table_entry_t entry_;
        int size_;
    };
    void generate_hit_table(unit_design_t & design, float design_quality);
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

float design_months(
    unit_design_t const & design,
    nation_t const & nation,
    reasons * reasons_ = nullptr);

float design_monthly_cost(
    unit_design_t const & design,
    nation_t const & nation,
    reasons * reasons_ = nullptr);

void roll_design_month(unit_design_t & design, nation_t const & nation);
