#pragma once

#include "game_data.hpp"


// TODO: In code that evaluates/applies these effects, some may need to
// recompute the effects of modifiying the affected target(s), producing
// knockon effects (e.g. changing o2_co2_suit. will need to create an effect
// that adjusts max_pop).

inline planet_effect_t onetime_growth_factor_effect(
    adobe::name_t name, adobe::name_t description, float amount,
    effect_op_t op = effect_op_t::add)
{
    planet_effect_t retval{
        .name=name,
        .description=description,
        .amount=amount,
        .months_of_effect=0,
        .months_remaining=0,
        .target=planet_effect_target_t::growth_factor,
        .target_modifiers=0,
        .operation=op
    };

    return retval;
}

inline planet_effect_t onetime_max_population_effect(
    adobe::name_t name, adobe::name_t description, float amount,
    effect_op_t op = effect_op_t::add)
{
    planet_effect_t retval{
        .name=name,
        .description=description,
        .amount=amount,
        .months_of_effect=0,
        .months_remaining=0,
        .target=planet_effect_target_t::max_population,
        .target_modifiers=0,
        .operation=op
    };

    return retval;
}
