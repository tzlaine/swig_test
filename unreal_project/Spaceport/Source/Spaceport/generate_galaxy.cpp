#include "constants.hpp"
#include "effects.hpp"
#include "generate_galaxy.hpp"
#include "game_data_t.hpp"
#include "rng.hpp"

#include <numbers>


namespace {
    // https://en.wikipedia.org/wiki/Stellar_classification

    // All units are solar (solar masses, solar luminosities, etc.).
    struct star_property_ranges_t
    {
        star_class_t class_;
        double frequency_;
        std::pair<double, double> temperature_;
        std::pair<double, double> mass_;
        std::pair<double, double> luminosity_;
    };

    // See: https://en.wikipedia.org/wiki/Stefan%E2%80%93Boltzmann_law
    // Luminance formula: L = 4piR^2σT^4
    // Simplified in terms of solar factors: R = T^-2L^0.5 = L^0.5 / T^2
    double solar_radius(double L, double T)
    {
        return std::sqrt(L) / (T * T);
    }

    constexpr star_property_ranges_t star_properties[] = {
        {star_class_t::invalid_star_class, 0.0, {}, {}, {}},
        {star_class_t::o, 0.0000003, {33000, 1000000}, {16, 1000000}, {30000, 1000000}},
        {star_class_t::b, 0.0012, {10000, 33000}, {2.1, 16}, {25, 30000}},
        {star_class_t::a, 0.0061, {7300, 10000}, {1.4, 2.1}, {5, 25}},
        {star_class_t::f, 0.03, {6000, 7300}, {1.04, 1.4}, {1.5, 5}},
        {star_class_t::g, 0.076, {5300, 6000}, {0.8, 1.04}, {0.6, 1.5}},
        {star_class_t::k, 0.12, {3900, 5300}, {0.45, 0.8}, {0.08, 0.6}},
        {star_class_t::m, 0.76, {2300, 3900}, {0.08, 0.45}, {0.01, 0.08}}
    };
}

float generation::detail::determine_growth_factor_and_effects(planet_t & planet)
{
    using namespace adobe::literals;
    using namespace std::literals;

    float retval = base_pop_growth_factor;

    auto const record = [&](adobe::name_t effect_name,
                            adobe::name_t effect_desc, float amount) {
        retval += amount;
        planet.effects.push_back(
            onetime_growth_factor_effect(effect_name, effect_desc, amount));
    };

    auto const only_equatorial_band_habitable = [&] {
        planet.effects.push_back(onetime_max_population_effect(
                                     "only_equatorial_band_habitable"_name,
                                     "only_equatorial_band_habitable_desc"_name,
                                     only_equatorial_region_habitable_factor,
                                     effect_op_t::multiply));
    };

    if (planet.planet_type != planet_type_t::rocky) {
        record("uninhab_non_rocky_planet"_name,
               "uninhab_non_rocky_planet_desc"_name, growth_uninhabitable);
        return retval;
    }

    // gravity
    if (planet.gravity_g < 0.1f) {
        record("very_low_grav"_name, "very_low_grav_desc"_name, -0.2);
    } else if (planet.gravity_g < 0.9f) {
        record("low_grav"_name, "low_grav_desc"_name,
               -0.1f * (1 - planet.gravity_g));
    } else if (planet.gravity_g < 1.1f) {
        // no effect
    } else if (planet.gravity_g < 1.3f) {
        record("high_grav"_name, "high_grav_desc"_name,
               0.1 * (planet.gravity_g - 1.1f));
    } else {
        record("very_high_grav"_name, "very_high_grav_desc"_name,
               -(planet.gravity_g - 1.3f));
    }

    // tilt
    if (planet.axial_tilt_d < 5.0f) {
        record("no_seasons"_name, "no_seasons_desc"_name, 0.05);
    } else if (/*5 < */planet.axial_tilt_d < 15.0f) {
        record("mild_seasons"_name, "mild_seasons_desc"_name, 0.025);
    } else if (/*15 < */planet.axial_tilt_d < 30.0f) {
        // normal seasons
        if (planet.orbital_period_y < 0.8) {
            record("short_seasons"_name, "short_seasons_desc"_name,
                   0.025 * (1 - planet.orbital_period_y));
        } else if (1.2 < planet.orbital_period_y) {
            record("long_seasons"_name, "long_seasons_desc"_name,
                   -0.05 * planet.orbital_period_y);
            planet.effects.push_back(planet_effect_t{
                    .name="long_seasons_infra_cost_effect"_name,
                    .description="long_seasons_infra_cost_effect_desc"_name,
                    .amount=1.25,
                    .target=planet_effect_target_t::infrastructure,
                    .operation=effect_op_t::multiply
                });
            if (3.0 < planet.orbital_period_y)
                only_equatorial_band_habitable();
        }
    } else if (/*30 < */planet.axial_tilt_d < 45.0f) {
        record("intense_seasons"_name, "intense_seasons_desc"_name, -0.05);
        if (planet.orbital_period_y < 0.8) {
            record("short_seasons"_name,
                   "short_seasons_desc"_name,
                   0.025 * (1 - planet.orbital_period_y));
        } else if (1.2 < planet.orbital_period_y) {
            record("long_seasons"_name,
                   "long_seasons_desc"_name,
                   -0.05 * planet.orbital_period_y);
            planet.effects.push_back(planet_effect_t{
                    .name="long_intense_seasons_infra_cost_effect"_name,
                    .description="long_intense_seasons_infra_cost_effect_desc"_name,
                    .amount=1.5,
                    .target=planet_effect_target_t::infrastructure,
                    .operation=effect_op_t::multiply
                });
            if (1.5 < planet.orbital_period_y)
                only_equatorial_band_habitable();
        }
    } else if (/*45 < */planet.axial_tilt_d < 60.0f) {
        record("intense_seasons"_name, "intense_seasons_desc"_name, -0.05);
        if (planet.orbital_period_y < 0.8) {
            record("short_seasons"_name,
                   "short_seasons_desc"_name,
                   0.025 * (1 - planet.orbital_period_y));
        } else if (1.2 < planet.orbital_period_y) {
            record("long_seasons"_name,
                   "long_seasons_desc"_name,
                   -0.05 * planet.orbital_period_y);
            planet.effects.push_back(planet_effect_t{
                    .name="long_intense_seasons_infra_cost_effect"_name,
                    .description="long_intense_seasons_infra_cost_effect_desc"_name,
                    .amount=1.5,
                    .target=planet_effect_target_t::infrastructure,
                    .operation=effect_op_t::multiply
                });
        }
        only_equatorial_band_habitable();
    } else if (/*60 < */planet.axial_tilt_d < 75.0f) {
        if (planet.orbital_period_y < 0.8) {
            record("short_seasons"_name, "short_seasons_desc"_name,
                   0.025 * (1 - planet.orbital_period_y));
        } else if (1.2 < planet.orbital_period_y) {
            record("long_seasons"_name, "long_seasons_desc"_name,
                   -0.05 * planet.orbital_period_y);
            planet.effects.push_back(planet_effect_t{
                    .name="long_seasons_infra_cost_effect"_name,
                    .description="long_seasons_infra_cost_effect_desc"_name,
                    .amount=1.25,
                    .target=planet_effect_target_t::infrastructure,
                    .operation=effect_op_t::multiply
                });
        }
        only_equatorial_band_habitable();
    } else if (/*75 < */planet.axial_tilt_d < 85.0f) {
        record("mild_seasons"_name, "mild_seasons_desc"_name, 0.025);
        only_equatorial_band_habitable();
    } else {
        record("no_seasons"_name, "no_seasons_desc"_name, 0.05);
        only_equatorial_band_habitable();
    }

    // TODO: Very long or very short days should have a negative effect on
    // infrastructure cost, due to the need for so much special care for
    // crops.  This only applies to non-habitat/suit situations obviously.

    // day length
    if (planet.day_h < 24.0f * 0.9f) {
        record("short_days"_name, "short_days_desc"_name,
               -24.0 / planet.day_h * 0.1);
    } else if (planet.day_h < 24.0f * 1.1f) {
        // no effect
    } else if (planet.day_h < 48.0f) {
        record("long_days"_name, "long_days_desc"_name,
               -std::min((planet.day_h - 24.0) * 0.01, 0.1));
    } else {
        record("very_long_days"_name, "very_long_days_desc"_name,
               -0.1 - std::min((planet.day_h - 48.0) * 0.01, 0.1));
    }

    // TODO: The O2/CO2 mix of a planet should either be something you can
    // adjust at the game start, or should perhaps require one of the
    // earliest techs.

    // TODO: The use of hab+suit should obviate many other modifiers that
    // would not apply to life entirely indoors.

    // These can come from multiple sources; don't replicate them.
    int habs_and_masks_already_required = 0;
    int habs_and_suits_already_required = 0;
    std::string name_scratch;
    std::string desc_scratch;
    auto const habs_and_masks_required = [&](std::string_view name_prefix) {
        if (habs_and_masks_already_required ||
            habs_and_suits_already_required) {
            return;
        }
        name_scratch = name_prefix;
        name_scratch += "_habs_and_masks_pop_effect";
        desc_scratch = name_scratch;
        desc_scratch += "_desc";
        planet.effects.push_back(
            onetime_max_population_effect(
                adobe::name_t(name_scratch.c_str()),
                adobe::name_t(desc_scratch.c_str()),
                habs_and_masks_habitable_factor, effect_op_t::multiply));
        name_scratch = name_prefix;
        name_scratch += "_habs_and_masks_infra_cost_effect";
        desc_scratch = name_scratch;
        desc_scratch += "_desc";
        planet.effects.push_back(planet_effect_t{
                .name=adobe::name_t(name_scratch.c_str()),
                .description=adobe::name_t(desc_scratch.c_str()),
                .amount=habs_and_masks_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=flags(planet_effect_mod_t::cost),
                .operation=effect_op_t::multiply
            });
        ++habs_and_masks_already_required;
    };
    auto const habs_and_suits_required = [&](std::string_view name_prefix) {
        if (habs_and_suits_already_required)
            return;
        name_scratch = name_prefix;
        name_scratch += "_habs_and_suits_pop_effect";
        desc_scratch = name_scratch;
        desc_scratch += "_desc";
        planet.effects.push_back(
            onetime_max_population_effect(
                adobe::name_t(name_scratch.c_str()),
                adobe::name_t(desc_scratch.c_str()),
                habs_and_suits_habitable_factor, effect_op_t::multiply));
        name_scratch = name_prefix;
        name_scratch += "_habs_and_suits_infra_cost_effect";
        desc_scratch = name_scratch;
        desc_scratch += "_desc";
        planet.effects.push_back(planet_effect_t{
                .name=adobe::name_t(name_scratch.c_str()),
                .description=adobe::name_t(desc_scratch.c_str()),
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=flags(planet_effect_mod_t::cost),
                .operation=effect_op_t::multiply
            });
        ++habs_and_suits_already_required;
    };

    // O2 (making simplifying assumption of ignoring the CO2 part of this
    // property)
    constexpr double harmless_o2_threshold =
        harmless_low_o2_percentage / earth_o2_percentage;
    double const effective_o2 =
        planet.o2_co2_suitability * planet.atmopsheric_pressure;
    if (harmless_o2_threshold < effective_o2) {
        // no effect
    } else if (effective_o2_percentage_la_paz_bolivia / earth_o2_percentage <
               effective_o2) {
        record("poor_o2_co2_suitab"_name,
               "poor_o2_co2_suitab_desc"_name,
               -(harmless_o2_threshold - effective_o2) * 0.25);
    } else if (effective_o2_percentage_aconcagua / earth_o2_percentage <
               effective_o2) {
        record("very_poor_o2_co2_suitab"_name,
               "very_poor_o2_co2_suitab_desc"_name,
               -(harmless_o2_threshold - effective_o2) * 0.25);
        habs_and_masks_required("very_poor_o2_co2_suitab"sv);
    } else if (effective_o2_percentage_mt_everest_peak / earth_o2_percentage <
               effective_o2) {
        record("marginal_o2_co2_suitab"_name,
               "marginal_o2_co2_suitab_desc"_name,
               -(harmless_o2_threshold - effective_o2) * 0.25);
        habs_and_masks_required("marginal_o2_co2_suitab"sv);
    } else {
        record("insufficient_o2_co2_suitab"_name,
               "insufficient_o2_co2_suitab_desc"_name,
               habs_and_suits_growth_modifier);
        habs_and_suits_required("insufficient_o2_co2_suitab"sv);
    }

    // atmospheric pressure (< 1 cases handled with o2_co2_suitability
    // above)
    if (4.0f < planet.atmopsheric_pressure) {
        record("high_press_n2_narcosis"_name,
               "high_press_n2_narcosis_desc"_name,
               habs_and_suits_growth_modifier);
        habs_and_suits_required("high_press_n2_narcosis"sv);
    }
    if (7.0f < effective_o2) {
        record("very_high_press_o2_toxicity"_name,
               "very_high_press_o2_toxicity_desc"_name,
               habs_and_suits_growth_modifier);
        habs_and_suits_required("very_high_press_o2_toxicity"sv);
    }

    // magnetosphere
    if (planet.magnetosphere_strength < 0.33) {
        record("very_weak_magneto"_name,
               "very_weak_magneto_desc"_name,
               habs_and_suits_growth_modifier);
        habs_and_suits_required("very_weak_magneto"sv);
    } else if (/*0.33 < */planet.magnetosphere_strength < 0.9) {
        record("weak_magneto"_name,
               "weak_magneto_desc"_name,
               -(1 - planet.magnetosphere_strength));
        if (planet.magnetosphere_strength < 0.67)
            habs_and_masks_required("weak_magneto"sv);
    } else if (/*0.9 < */planet.magnetosphere_strength < 1.1) {
        // no effect
    } else if (1.1 < planet.magnetosphere_strength) {
        record("strong_magneto"_name,
               "strong_magneto_desc"_name,
               (planet.magnetosphere_strength - 1.1) * 0.02);
    }

    // temperature
    if (planet.surface_temperature_k < earth_temperature_k - 44) {
        record("extremely_cold_avg_surface_temp"_name,
               "extremely_cold_avg_surface_temp_desc"_name,
               habs_and_suits_growth_modifier);
        habs_and_suits_required("extremely_cold_avg_surface_temp"sv);
    } else if (/*earth - 44 < */planet.surface_temperature_k < earth_temperature_k - 22) {
        record("very_cold_avg_surface_temp"_name,
               "very_cold_avg_surface_temp_desc"_name,
               -(earth_temperature_k - 11 - planet.surface_temperature_k) * 0.03);
    } else if (/*earth - 22 < */planet.surface_temperature_k < earth_temperature_k - 11) {
        record("cold_avg_surface_temp"_name,
               "cold_avg_surface_temp_desc"_name,
               -(earth_temperature_k - 11 - planet.surface_temperature_k) * 0.01);
    } else if (/*earth - 11 < */planet.surface_temperature_k < earth_temperature_k + 11) {
        // no effect
    } else if (/*earth + 11 < */planet.surface_temperature_k < earth_temperature_k + 22) {
        record("hot_avg_surface_temp"_name,
               "hot_avg_surface_temp_desc"_name,
               -(planet.surface_temperature_k - (earth_temperature_k + 11)) * 0.01);
    } else if (/*earth + 22 < */planet.surface_temperature_k < earth_temperature_k + 33) {
        record("very_hot_avg_surface_temp"_name,
               "very_hot_avg_surface_temp_desc"_name,
               -(planet.surface_temperature_k - (earth_temperature_k + 11)) * 0.03);
    } else if (/*earth + 33 < */planet.surface_temperature_k < earth_temperature_k + 55) {
        record("extremely_hot_avg_surface_temp"_name,
               "extremely_hot_avg_surface_temp_desc"_name,
               habs_and_suits_growth_modifier);
        habs_and_suits_required("extremely_hot_avg_surface_temp"sv);
    } else {
        record("uninhabitably_hot_avg_surface_temp"_name,
               "uninhabitably_hot_avg_surface_temp_desc"_name, growth_uninhabitable);
    }

    if (habs_and_masks_already_required &&
        habs_and_suits_already_required) {
        std::erase_if(planet.effects, [](auto const & effect) {
            return !std::ranges::search(
                effect.name, "_habs_and_masks"sv).empty();
        });
    }

    resort_effects(planet);

    return retval;
}

// See: https://en.wikipedia.org/wiki/Sun

star_t generation::detail::generate_star()
{
    star_t retval;

    // TODO: See if generating 100s or 1000s of rolls at once is faster.
    double roll = random_unit_double();
    auto it = std::ranges::find_if(
        star_properties,
        [&](auto const & elem) {
            bool const retval = roll < elem.frequency_;
            roll -= elem.frequency_;
            return retval;
        });
    if (it == std::ranges::begin(star_properties) ||
        it == std::ranges::end(star_properties)) {
        it = std::ranges::begin(star_properties) + 5;
    }
    retval.star_class = it->class_;

    auto const & ranges = *it;
    retval.temperature_k = std::lerp(ranges.temperature_.first,
                                     ranges.temperature_.second,
                                     random_unit_double());
    retval.solar_masses = std::lerp(ranges.mass_.first,
                                    ranges.mass_.second,
                                    random_unit_double());
    retval.solar_luminosities = std::lerp(ranges.luminosity_.first,
                                          ranges.luminosity_.second,
                                          random_unit_double());

    retval.solar_radii = solar_radius(
        retval.solar_luminosities,
        retval.temperature_k / sun_temperature_k);

    return retval;
}

// For reference:
// Mercury 5427 kg/km^3 3.3e23 kg
// Venus 5243 kg/km^3 4.87e24 kg
// Earth 5514 kg/km^3 5.97e24 kg
// Mars 3934 kg/km^3 6.42e23 kg
// Jupiter 1326 kg/km^3 1.90e27 kg
// Saturn 687 kg/km^3 5.68e26 kg
// Uranus 1270 kg/km^3 1.02e26 kg
// Neptune 1638 kg/km^3 8.68e25 kg

bool generation::detail::generate_planet(planet_t & planet, system_t const & system)
{
    // According to Google, planets < 0.3 Jupiter masses are rocky.
    constexpr double rockiness_mass_threshold = 5.7e26;

    // TODO: Move these out of this function?
    std::gamma_distribution<double> day_dist(2, 1.8);
    std::normal_distribution<double> rocky_magnetosphere_dist(1.0, 0.35);
    std::normal_distribution<double> giant_magnetosphere_dist(4.0, 1.0);
    std::gamma_distribution<double> o2_dist(1.5, 1);
    std::normal_distribution<double> tilt_dist(0.0, 30.0);
    std::normal_distribution<double> rocky_density_dist(5250.0, 400.0);
    std::normal_distribution<double> gas_giant_density_dist(1000.0, 150.0);
    std::normal_distribution<double> ice_giant_density_dist(1400.0, 150.0);
    std::normal_distribution<double> atmos_dist(1.0, 0.2);
    std::normal_distribution<double> ocean_coverage_dist(0.7, 0.05);
    std::uniform_int_distribution<int> resource_dist(
        min_resource_value, max_resource_value);

    // Arbitrary line between the gas giants and the ice giants, taken
    // from the boundary between the Saturn and Uranus orbits, and scaled
    // by solar masses of this system's star..
    double const ice_distance_au = 15.0 * system.star.solar_masses;

    double density = 0.0; // in kg/km^3
    if (planet.mass_kg < rockiness_mass_threshold) {
        density = random_number(rocky_density_dist);
        planet.planet_type = planet_type_t::rocky;
    } else if (planet.orbit_au < ice_distance_au) {
        density = random_number(gas_giant_density_dist);
        planet.planet_type = planet_type_t::gas_giant;
    } else {
        density = random_number(ice_giant_density_dist);
        planet.planet_type = planet_type_t::ice_giant;
    }
    double const volume = planet.mass_kg / density;

    planet.radius_km = std::cbrt(0.75 * volume / std::numbers::pi);

    // 1000 to convert to meters
    double const surface_gravity =
        G * planet.mass_kg / (planet.radius_km * 1000.0);
    planet.gravity_g = surface_gravity / earth_gravity;

    planet.axial_tilt_d =
        std::min(std::abs(random_number(tilt_dist)), 90.0);

    // 15 is made up.
    planet.day_h = 15.0 * random_number(day_dist);

    planet.surface_temperature_k =
        system.star.temperature_k *
        std::sqrt(system.star.solar_radii * sun_radius_km /
                  (2 * planet.orbit_au * au_to_km));

    planet.o2_co2_suitability = 0.0;
    if (planet.planet_type == planet_type_t::rocky) {
        if (random_unit_double() < prob_rocky_planet_has_magnetosphere) {
            planet.magnetosphere_strength =
                std::min(0.0, random_number(rocky_magnetosphere_dist));
        } else {
            planet.magnetosphere_strength = 0.0;
        }
        // 50 is made up.
        if (earth_temperature_k + 50 < planet.surface_temperature_k) {
            planet.atmosphere_type = atmosphere_type_t::high_temperature;
            // 5 is made up.
            planet.atmopsheric_pressure = 5 * random_number(atmos_dist) *
                planet.mass_kg / earth_mass_kg;
        } else if (planet.magnetosphere_strength < 0.01) {
            double const no_mag_roll = random_unit_double();
            if (no_mag_roll < prob_no_magnetosphere_rocky_planet_is_reduced) {
                planet.atmosphere_type =
                    atmosphere_type_t::reduced_type_a;
                // 0.05 is made up.
                planet.atmopsheric_pressure =
                    0.05 * random_number(atmos_dist) *
                    planet.mass_kg / earth_mass_kg;
            } else {
                planet.atmosphere_type =
                    atmosphere_type_t::carbon_rich_type_c;
                // 0.2 is made up.
                planet.atmopsheric_pressure =
                    0.2 * random_number(atmos_dist) *
                    planet.mass_kg / earth_mass_kg;
            }
        } else {
            planet.atmosphere_type = atmosphere_type_t::oxidized_type_b;
            double const scale = o2_dist.max() - o2_dist.min();
            planet.o2_co2_suitability =
                1.0 - random_number(o2_dist) / scale;
            planet.atmopsheric_pressure = random_number(atmos_dist) *
                planet.mass_kg / earth_mass_kg;
        }
    } else {
        planet.magnetosphere_strength =
            random_number(giant_magnetosphere_dist);
        if (planet.planet_type == planet_type_t::gas_giant) {
            planet.atmosphere_type =
                atmosphere_type_t::gas_giant_atmosphere;
            planet.atmopsheric_pressure = atmos_millions;
        } else {
            planet.atmosphere_type =
                atmosphere_type_t::ice_giant_atmosphere;
            planet.atmopsheric_pressure = atmos_thousands;
        }
    }

    // TODO: rings

    if (planet.planet_type == planet_type_t::rocky) {
        planet.ocean_coverage = random_number(ocean_coverage_dist);
        double const earth_pop_scale =
            (planet.radius_km * planet.radius_km * planet.radius_km) *
            planet.ocean_coverage /
            (earth_radius_km * earth_radius_km * earth_radius_km) /
            earth_ocean_coverage;
        planet.max_population =
            std::round(max_earth_pops * earth_pop_scale);
    } else {
        planet.ocean_coverage = n_a;
        planet.max_population = 0;
    }

    double const growth_factor =
        determine_growth_factor_and_effects(planet);
    planet.growth_factor = growth_factor;

    auto clamp_res = [](int x) {
        return std::clamp(min_resource_value, x, min_resource_value);
    };

    // This scale represents moons that a gas/ice giant has, which may
    // have accessible surface resouces.
    double const moon_factor = gas_giant_moon_resource_factor;

    if (planet.planet_type == planet_type_t::rocky) {
        if (planet.atmosphere_type == atmosphere_type_t::oxidized_type_b &&
            growth_factor_considered_habitable < growth_factor) {
            planet.water = max_resource_value;
        } else {
            planet.water = clamp_res(random_number(resource_dist));
        }
    } else {
        planet.water = clamp_res(
            moon_factor * random_number(resource_dist));
    }

    if (planet.planet_type == planet_type_t::rocky &&
        planet.atmosphere_type == atmosphere_type_t::oxidized_type_b &&
        growth_factor_considered_habitable < growth_factor) {
        planet.food = clamp_res(random_number(resource_dist));
    } else {
        planet.food = 0;
    }

    planet.energy = 0;
    if (planet.planet_type == planet_type_t::rocky) {
        planet.energy = clamp_res(
            energy_from_solar + energy_from_wind +
            random_number(resource_dist));
    } else if (planet.planet_type == planet_type_t::gas_giant) {
        planet.energy = clamp_res(
            2 * random_number(resource_dist));
    } else {
        planet.energy = n_a;
    }

    if (planet.planet_type == planet_type_t::rocky) {
        double const scale = planet.magnetosphere_strength ?
            planet.magnetosphere_strength : 1.0;
        planet.metal = clamp_res(
            scale * random_number(resource_dist));
    } else {
        planet.metal = clamp_res(
            moon_factor * random_number(resource_dist));
    }

    if (planet.planet_type == planet_type_t::rocky) {
        planet.fuel = clamp_res(random_number(resource_dist));
    } else {
        planet.fuel = clamp_res(
            moon_factor * random_number(resource_dist));
    }

    planet.population = 0;
    planet.infrastructure = 0;
    planet.owner = -1;
    planet.original_owner = -1;

    return growth_factor_considered_habitable < growth_factor;
}

bool generation::detail::generate_system(
    system_t & system, std::vector<planet_t> & planets,
    hex_coord_t hc, point_2d hex_world_pos, int system_id)
{
    // system.name = TODO;

    system.coord = hc;
    system.star = generate_star();

    system.first_planet = planets.size();

    // TODO: See if generating 100s or 1000s of rolls at once is faster.
    double x_roll = random_unit_double();
    double y_roll = hex_height * random_unit_double();

    /* The (x,y) rolled above is mapped onto a rectangle that represents
       all the locations within the hex.  Since a hex does not exactly
       cover a rectangle, the left-side corners are snipped off and put on
       the right to form the rectangle, as shown below.
           
       |-----|
       |   \ | <- missing lower left segment of hex
       |    \|
       |    /|
       |   / | <- missing upper left segment of hex
       |-----|
    */

    if (0.5 < x_roll) {
        if (hex_height / 2.0 < y_roll)
            y_roll -= hex_height / 2.0;
        else
            y_roll += hex_height / 2.0;
        x_roll -= 1.0;
    }

    // Now the corners on the left are at x=-0.5 to x=0.0; correct.
    x_roll += 0.5;

    // Change coord to be relative to hex center. TODO?
    //x_roll -= hex_width / 2.0;
    //y_roll -= hex_height / 2.0;

    system.world_pos_x = hex_world_pos.x + x_roll;
    system.world_pos_y = hex_world_pos.y + y_roll;

    // 35 comes from Neptune's orbit at about 30AU, and adding a little.
    // The * solar_masses is there because the size of the accretion disk
    // that formed this system is vaguely proportional to the mass of the
    // star.
    double const protostar_disk_radius_au =
        35.0 * system.star.solar_masses;
    std::gamma_distribution<double> gamma_dist(1.0, 1.5);

    std::vector<double> radii; // in AU
    radii.push_back(0.25 * system.star.solar_masses);
    while (radii.back() < protostar_disk_radius_au) {
        double next_pos_au =
            radii.back() * (1.0 + random_number(gamma_dist));
        radii.push_back(next_pos_au);
    }
    radii.back() = std::min(radii.back(), protostar_disk_radius_au);

    // Create area/masses for each planet.
    // Also, replace radii with midpoint between near/far.
    std::vector<double> masses(radii.size());
    double sum_of_masses = 0.0;
    std::transform(radii.begin(), radii.end() - 1, radii.begin() + 1,
                   masses.begin(), [&](double & near, double far) {
                       // Area of the accretion disk that was involved in
                       // forming this planet.
                       double const retval =
                           std::numbers::pi * (far * far - near * near);
                       sum_of_masses += retval;
                       near = (near + far) / 2.0;
                       return retval;
                   });
    radii.pop_back();

    // Replace masses with fractions of sum of mass of all planets, then
    // scale that to kg based on the Solar sytem.
    std::transform(
        masses.begin(), masses.end(), masses.begin(), [&](double mass) {
            return mass / sum_of_masses *
                mass_of_solar_system_planets_kg * system.star.solar_masses;
        });

    planets.resize(planets.size() + masses.size());
    system.last_planet = planets.size();

    std::ranges::subrange const system_planets(
        planets.end() -  masses.size(), planets.end());
    bool has_habitable_planet = false;
    auto radii_it = radii.begin();
    auto masses_it = masses.begin();
    for (auto & planet : system_planets) {
        planet.system_id = system_id;
        planet.orbit_au = *radii_it++;
        planet.mass_kg = *masses_it++;

        double const orbital_radius_m = 1000.0 * planet.orbit_au * au_to_km;
        double const orbital_period_s = std::sqrt(
            4 * std::numbers::pi * std::numbers::pi *
            orbital_radius_m * orbital_radius_m * orbital_radius_m /
            G * (system.star.solar_masses * sun_mass_kg + planet.mass_kg));
        planet.orbital_period_y = orbital_period_s / years_to_seconds;

        if (generate_planet(planet, system))
            has_habitable_planet = true;
    }

    return has_habitable_planet;
}

void generation::generate_galaxy(game_start_params const & params,
                                 game_state_t & game_state)
{
    // in world units
    double const map_radius = params.map_height / 2.0 * hex_height;
    // + hex_height/2 allows room for center hex
    double const bulge_radius = map_radius / 10.0 + hex_height / 2.0;

    // in hexes
    game_state.map_height = params.map_height;
    game_state.map_width = params.map_height / (hex_width / hex_height);
    if (game_state.map_width % 2 == 0)
        ++game_state.map_width;
    game_state.hexes.resize(game_state.map_width * game_state.map_height);

    auto const hex_position_ =
        [map_height = game_state.map_height](hex_coord_t hc) {
            return ::hex_position(hc, map_height);
        };

   hex_coord_t const center_hex{
       game_state.map_width / 2, game_state.map_height / 2};
   point_2d const center_hex_pos = hex_position_(center_hex);

   std::normal_distribution<double> habitable_systems_dist(
       params.habtitable_systems_per_hex_mean,
       params.habtitable_systems_per_hex_std_dev);

   // TODO: The hexes can be generated in parallel; so can the individual
   // stars within the hex.

   int hex_index = -1;
   for (auto & hex : game_state.hexes) {
       ++hex_index;
       hex_coord_t const hc{
           hex_index % game_state.map_width, hex_index / game_state.map_width};
       hex.coord = hc;

       auto const pos = hex_position_(hc);
       if (within(center_hex_pos, pos, bulge_radius)) {
           if (hc == center_hex)
               hex.province_id = prov_galactic_center;
           else
               hex.province_id = prov_galactic_bulge;
           continue;
       } else if (!within(center_hex_pos, pos, map_radius)) {
           hex.province_id = prov_off_map;
           continue;
       }

       hex.province_id = prov_none;
       hex.first_system = game_state.systems.size();

#if defined(BUILD_FOR_TEST)
       if (detail::g_skip_system_generation_for_testing)
           continue;
#endif

       auto const habitable_systems =
           int(std::round(random_number(habitable_systems_dist)));
       int habitable_systems_so_far = 0;
       while ((habitable_systems &&
               habitable_systems_so_far < habitable_systems) ||
              (!habitable_systems &&
               game_state.systems.size() - hex.first_system < 20u)) {
           game_state.systems.push_back(system_t());
           if (detail::generate_system(
                   game_state.systems.back(), game_state.planets,
                   hc, pos, hex.first_system + habitable_systems_so_far)) {
               ++habitable_systems_so_far;
           }
       }

       hex.last_system = game_state.systems.size();
   }
}
