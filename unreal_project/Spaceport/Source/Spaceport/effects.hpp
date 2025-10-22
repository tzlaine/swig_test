#pragma once

#include "game_data.hpp"


// TODO: In code that evaluates/applies these effects, some may need to
// recompute the effects of modifiying the affected target(s), producing
// knockon effects (e.g. changing o2_co2_suit. will need to create an effect
// that adjusts max_pop).

enum struct multiplicative : bool {
    no,
    yes
};

inline planet_effect_t onetime_growth_factor_effect(
    adobe::name_t name, adobe::name_t description, float amount,
    multiplicative mult = multiplicative::no)
{
    planet_effect_t retval{
        .name=name,
        .description=description,
        .target=planet_effect_target_t::growth_factor,
        .one_time_effect=amount,
        .monthly_effect=0,
        .months_of_effect=0,
        .months_remaining=0,
        .effects_are_permanent=true,
        .affects_cost=false,
        .multiplicative=(bool)mult
    };

    return retval;
}

inline planet_effect_t onetime_max_population_effect(
    adobe::name_t name, adobe::name_t description, float amount,
    multiplicative mult = multiplicative::no)
{
    planet_effect_t retval{
        .name=name,
        .description=description,
        .target=planet_effect_target_t::max_population,
        .one_time_effect=amount,
        .monthly_effect=0,
        .months_of_effect=0,
        .months_remaining=0,
        .effects_are_permanent=true,
        .affects_cost=false,
        .multiplicative=(bool)mult
    };

    return retval;
}
