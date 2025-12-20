#include "constants.hpp"
#include "effects.hpp"
#include "generate_galaxy.hpp"
#include "rng.hpp"
#include "task_system.hpp"

#include <numbers>


// TODO: Need to add a way to reset the Lua state, when running in the editor.
// Otherwise, since all Lua states are initialized at static-init time,
// multiple editor runs will all use the original version of the Lua scripts.

// TODO: Get rid of the remaining magic numbers in this file (put them in a
// Lua file).

// MAINTENANCE NOTE: Any calls to the lambda habs_and_suits_required (but not
// the masks one) should come before any other effects, since it clears out
// all previous effects.  Any effects that affect colonists living in
// habs+suits should come *after* all calls to habs_and_suits_required.
float generation::detail::determine_growth_factor_and_effects(planet_t & planet)
{
    using namespace adobe::literals;

    auto const effect = [&](auto name) {
        planet.effects.push_back(planet_effect_t{name});
    };

    planet.growth_factor = base_pop_growth_factor;

    if (planet.planet_type != planet_type_t::rocky) {
        effect("uninhab_non_rocky_planet"_name);
        planet.growth_factor += growth_uninhabitable;
        return planet.growth_factor;
    }

    // tilt
    if (planet.axial_tilt_d < 5.0f) {
        effect("no_seasons"_name);
    } else if (/*5 < */ planet.axial_tilt_d < 15.0f) {
        effect("mild_seasons"_name);
    } else if (/*15 < */ planet.axial_tilt_d < 30.0f) {
        // normal seasons
        if (planet.orbital_period_y < 0.8) {
            effect("short_seasons"_name);
        } else if (1.2 < planet.orbital_period_y) {
            effect("long_seasons"_name);
            planet.effects.push_back(planet_effect_t{
                .name = "infra_cost"_name,
                .reason = "long_seasons"_name,
                .value = agri_equip_infra_cost_factor});
            if (3.0 < planet.orbital_period_y)
                effect("only_equatorial_band_habitable"_name);
        }
    } else if (/*30 < */ planet.axial_tilt_d < 45.0f) {
        effect("intense_seasons"_name);
        if (planet.orbital_period_y < 0.8) {
            effect("short_seasons"_name);
        } else if (1.2 < planet.orbital_period_y) {
            effect("long_seasons"_name);
            planet.effects.push_back(planet_effect_t{
                .name = "infra_cost"_name,
                .reason = "long_intense_seasons"_name,
                .value = 1.5});
            if (1.5 < planet.orbital_period_y)
                effect("only_equatorial_band_habitable"_name);
        }
    } else if (/*45 < */ planet.axial_tilt_d < 60.0f) {
        effect("intense_seasons"_name);
        if (planet.orbital_period_y < 0.8) {
            effect("short_seasons"_name);
        } else if (1.2 < planet.orbital_period_y) {
            effect("long_seasons"_name);
            planet.effects.push_back(planet_effect_t{
                .name = "infra_cost"_name,
                .reason = "long_intense_seasons"_name,
                .value = 1.5});
        }
        effect("only_equatorial_band_habitable"_name);
    } else if (/*60 < */ planet.axial_tilt_d < 75.0f) {
        if (planet.orbital_period_y < 0.8) {
            effect("short_seasons"_name);
        } else if (1.2 < planet.orbital_period_y) {
            effect("long_seasons"_name);
            planet.effects.push_back(planet_effect_t{
                .name = "infra_cost"_name,
                .reason = "long_seasons"_name,
                .value = agri_equip_infra_cost_factor});
        }
        effect("only_equatorial_band_habitable"_name);
    } else if (/*75 < */ planet.axial_tilt_d < 85.0f) {
        effect("mild_seasons"_name);
        effect("only_equatorial_band_habitable"_name);
    } else {
        effect("no_seasons"_name);
        effect("only_equatorial_band_habitable"_name);
    }

    // day length
    if (planet.day_h < 24.0f * 0.9f) {
        effect("short_days"_name);
        planet.effects.push_back(planet_effect_t{
            .name = "infra_cost"_name,
            .reason = "short_days"_name,
            .value = agri_equip_infra_cost_factor});
    } else if (planet.day_h < 24.0f * 1.1f) {
        // no effect
    } else if (planet.day_h < 48.0f) {
        effect("long_days"_name);
        planet.effects.push_back(planet_effect_t{
            .name = "infra_cost"_name,
            .reason = "long_days"_name,
            .value = agri_equip_infra_cost_factor});
    } else {
        effect("very_long_days"_name);
        planet.effects.push_back(planet_effect_t{
            .name = "infra_cost"_name,
            .reason = "very_long_days"_name,
            .value = agri_equip_infra_cost_factor});
    }
    // TODO: Require habs+suits if the day is long enough?

    // TODO: Should the day length effects apply to planets that are very
    // highly tilted, like >= 75 deg?

    // TODO: The O2/CO2 mix of a planet should either be something you can
    // adjust at the game start, or should perhaps require one of the
    // earliest techs.

    // These can come from multiple sources; don't replicate them.
    int habs_and_suits_already_required = 0;
    std::string name_scratch;
    std::string desc_scratch;
    auto const habs_and_masks_required = [&](adobe::name_t reason) {
        planet.effects.push_back(planet_effect_t{
            .name = "habs_and_masks_required"_name, .reason = reason});
        planet.effects.push_back(planet_effect_t{
            .name = "infra_cost"_name,
            .reason = reason,
            .value = habs_and_masks_infra_cost_factor});
    };
    auto const habs_and_suits_required = [&](adobe::name_t reason) {
        if (habs_and_suits_already_required)
            return;

        // Dump all previous effects.  Living in a hab+suit obviates almost
        // all other effects.
        planet.effects.clear();

        planet.effects.push_back(planet_effect_t{
            .name = "habs_and_suits_required"_name, .reason = reason});
        planet.effects.push_back(planet_effect_t{
            .name = "infra_cost"_name,
            .reason = reason,
            .value = habs_and_suits_infra_cost_factor});

        ++habs_and_suits_already_required;
    };

    // O2 (making simplifying assumption of ignoring the CO2 part of this
    // property)
    double const harmless_o2_threshold =
        harmless_low_o2_percentage / earth_o2_percentage;
    double const effective_o2 =
        planet.o2_co2_suitability * planet.atmopsheric_pressure;
    if (harmless_o2_threshold < effective_o2) {
        // no effect
    } else if (
        effective_o2_percentage_la_paz_bolivia / earth_o2_percentage <
        effective_o2) {
        effect("poor_o2_co2_suitab"_name);
    } else if (
        effective_o2_percentage_aconcagua / earth_o2_percentage <
        effective_o2) {
        effect("very_poor_o2_co2_suitab"_name);
        habs_and_masks_required("very_poor_o2_co2_suitab"_name);
    } else if (
        effective_o2_percentage_mt_everest_peak / earth_o2_percentage <
        effective_o2) {
        effect("marginal_o2_co2_suitab"_name);
        habs_and_masks_required("marginal_o2_co2_suitab"_name);
    } else {
        habs_and_suits_required("insufficient_o2_co2_suitab"_name);
        effect("insufficient_o2_co2_suitab"_name);
    }

    // atmospheric pressure (< 1 cases handled with o2_co2_suitability
    // above)
    if (4.0f < planet.atmopsheric_pressure) {
        habs_and_suits_required("high_press_n2_narcosis"_name);
        effect("high_press_n2_narcosis"_name);
    }
    if (7.0f < effective_o2) {
        habs_and_suits_required("very_high_press_o2_toxicity"_name);
        effect("very_high_press_o2_toxicity"_name);
    }

    // magnetosphere
    if (planet.magnetosphere_strength < 0.33) {
        habs_and_suits_required("very_weak_magneto"_name);
        effect("very_weak_magneto"_name);
    } else if (/*0.33 < */ planet.magnetosphere_strength < 0.9) {
        effect("weak_magneto"_name);
        if (planet.magnetosphere_strength < 0.67)
            habs_and_masks_required("weak_magneto"_name);
    } else if (/*0.9 < */ planet.magnetosphere_strength < 1.1) {
        // no effect
    } else if (1.1 < planet.magnetosphere_strength) {
        effect("strong_magneto"_name);
    }

    // temperature
    if (planet.surface_temperature_k < min_habitable_nonsuit_temp_k) {
        habs_and_suits_required("extremely_cold_avg_surface_temp"_name);
        effect("extremely_cold_avg_surface_temp"_name);
    } else if (
        /*earth - 44 < */ planet.surface_temperature_k <
        earth_temperature_k - 22) {
        effect("very_cold_avg_surface_temp"_name);
    } else if (
        /*earth - 22 < */ planet.surface_temperature_k <
        earth_temperature_k - 11) {
        effect("cold_avg_surface_temp"_name);
    } else if (
        /*earth - 11 < */ planet.surface_temperature_k <
        earth_temperature_k + 11) {
        // no effect
    } else if (
        /*earth + 11 < */ planet.surface_temperature_k <
        earth_temperature_k + 22) {
        effect("hot_avg_surface_temp"_name);
    } else if (
        /*earth + 22 < */ planet.surface_temperature_k <
        earth_temperature_k + 33) {
        effect("very_hot_avg_surface_temp"_name);
    } else if (
        /*earth + 33 < */ planet.surface_temperature_k < max_habitable_temp_k) {
        habs_and_suits_required("extremely_hot_avg_surface_temp"_name);
        effect("extremely_hot_avg_surface_temp"_name);
    } else {
        effect("uninhabitably_hot_avg_surface_temp"_name);
    }

    // gravity
    if (planet.gravity_g < 0.1f)
        effect("very_low_grav"_name);
    else if (planet.gravity_g < 0.9f)
        effect("low_grav"_name);
    else if (planet.gravity_g < 1.1f)
        ; // no effect
    else if (planet.gravity_g < 1.3f)
        effect("high_grav"_name);
    else
        effect("very_high_grav"_name);

    for (auto const & e : planet.effects) {
        apply_planet_effect(planet, e);
    }

    return planet.growth_factor;
}

// See: https://en.wikipedia.org/wiki/Sun

// TODO: See if generating 100s or 1000s of rolls at once is faster.
star_t
generation::detail::generate_star(double roll /* = random_unit_double()*/)
{
    star_t retval;

    auto it = std::ranges::find_if(star_properties, [&](auto const & elem) {
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
    retval.temperature_k = std::lerp(
        ranges.temperature_.first,
        ranges.temperature_.second,
        random_unit_double());
    retval.solar_masses = std::lerp(
        ranges.mass_.first, ranges.mass_.second, random_unit_double());
    retval.solar_luminosities = std::lerp(
        ranges.luminosity_.first,
        ranges.luminosity_.second,
        random_unit_double());

    retval.solar_radii = solar_radius(
        retval.solar_luminosities, retval.temperature_k / sun_temperature_k);

    return retval;
}

// For reference:
// Mercury 5427 kg/m^3 3.3e23 kg
// Venus 5243 kg/m^3 4.87e24 kg
// Earth 5514 kg/m^3 5.97e24 kg
// Mars 3934 kg/m^3 6.42e23 kg
// Jupiter 1326 kg/m^3 1.90e27 kg
// Saturn 687 kg/m^3 5.68e26 kg
// Uranus 1270 kg/m^3 1.02e26 kg
// Neptune 1638 kg/m^3 8.68e25 kg

bool generation::detail::generate_planet(
    planet_t & planet, system_t const & system)
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
    double const ice_distance_au =
        ice_giant_distance_factor * system.star.solar_masses;

    double density = 0.0; // in kg/km^3
#if defined(BUILD_FOR_TEST)
    // Alow the test to optionally force a planet type.
    if (planet.planet_type == planet_type_t::rocky)
        density = random_number(rocky_density_dist);
    else if (planet.planet_type == planet_type_t::gas_giant)
        density = random_number(gas_giant_density_dist);
    else if (planet.planet_type == planet_type_t::ice_giant)
        density = random_number(ice_giant_density_dist);
    if (density) {
    } else
#endif
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
    double const m3_per_km3 = 1e9;
    double const volume = planet.mass_kg / (density * m3_per_km3);

    planet.radius_km = std::cbrt(0.75 * volume / std::numbers::pi);

    double const surface_gravity =
        G_ * planet.mass_kg /
        (planet.radius_km * m_per_km * planet.radius_km * m_per_km);
    planet.gravity_g = surface_gravity / earth_gravity;

    planet.axial_tilt_d = std::min(std::abs(random_number(tilt_dist)), 90.0);

    planet.day_h = day_length_factor * random_number(day_dist);

    // Kepler's third law: T^2 = (4pi^2/GM)a^3
    double const a = planet.orbit_au * km_per_au * m_per_km;
    double const a3 = a * a * a;
    double const M = system.star.solar_masses * sun_mass_kg;
    planet.orbital_period_y =
        std::sqrt(4 * std::numbers::pi * std::numbers::pi / (G_ * M) * a3) /
        secs_per_year;

    planet.surface_temperature_k = system.star.temperature_k *
                                   std::sqrt(
                                       system.star.solar_radii * sun_radius_km /
                                       (2 * planet.orbit_au * km_per_au));

    planet.o2_co2_suitability = 0.0;
    if (planet.planet_type == planet_type_t::rocky) {
        if (random_unit_double() < prob_rocky_planet_has_magnetosphere) {
            planet.magnetosphere_strength =
                std::max(0.0, random_number(rocky_magnetosphere_dist));
        } else {
            planet.magnetosphere_strength = 0.0;
        }
        if (high_temp_k < planet.surface_temperature_k) {
            planet.atmosphere_type = atmosphere_type_t::high_temperature;
            planet.atmopsheric_pressure = high_temp_atmosphere_pressure_factor *
                                          random_number(atmos_dist) *
                                          planet.mass_kg / earth_mass_kg;
        } else if (planet.magnetosphere_strength < 0.01) {
            double const no_mag_roll = random_unit_double();
            if (no_mag_roll < prob_no_magnetosphere_rocky_planet_is_reduced) {
                planet.atmosphere_type = atmosphere_type_t::reduced_type_a;
                planet.atmopsheric_pressure =
                    reduced_rocky_planet_pressure_factor *
                    random_number(atmos_dist) * planet.mass_kg / earth_mass_kg;
            } else {
                planet.atmosphere_type = atmosphere_type_t::carbon_rich_type_c;
                planet.atmopsheric_pressure =
                    nonreduced_rocky_planet_pressure_factor *
                    random_number(atmos_dist) * planet.mass_kg / earth_mass_kg;
            }
        } else {
            planet.atmosphere_type = atmosphere_type_t::oxidized_type_b;
            double const scale = o2_dist.max() - o2_dist.min();
            planet.o2_co2_suitability = 1.0 - random_number(o2_dist) / scale;
            planet.atmopsheric_pressure =
                random_number(atmos_dist) * planet.mass_kg / earth_mass_kg;
        }
    } else {
        planet.magnetosphere_strength = random_number(giant_magnetosphere_dist);
        if (planet.planet_type == planet_type_t::gas_giant) {
            planet.atmosphere_type = atmosphere_type_t::gas_giant_atmosphere;
            planet.atmopsheric_pressure = atmos_millions;
        } else {
            planet.atmosphere_type = atmosphere_type_t::ice_giant_atmosphere;
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
        planet.max_population = std::round(max_earth_pops * earth_pop_scale);
    } else {
        planet.ocean_coverage = n_a;
        planet.max_population = 0;
    }

    double const growth_factor = determine_growth_factor_and_effects(planet);
    planet.growth_factor = growth_factor;

    auto clamp_res = [](int x) {
        return std::clamp(x, min_resource_value, max_resource_value);
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
        planet.water = clamp_res(moon_factor * random_number(resource_dist));
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
    } else {
        planet.energy = clamp_res(moon_factor * random_number(resource_dist));
    }

    // TODO: Give each hex a bias (+ive or -ive) for fuel and metal, since
    // presence of heavy elements is stellar-neighborhood-dependent.

    if (planet.planet_type == planet_type_t::rocky) {
        double const scale =
            planet.magnetosphere_strength ? planet.magnetosphere_strength : 1.0;
        planet.metal = clamp_res(scale * random_number(resource_dist));
    } else {
        planet.metal = clamp_res(moon_factor * random_number(resource_dist));
    }

    if (planet.planet_type == planet_type_t::rocky) {
        planet.fuel = clamp_res(random_number(resource_dist));
    } else {
        planet.fuel = clamp_res(moon_factor * random_number(resource_dist));
    }

    planet.population = 0;
    planet.infrastructure = 0;
    planet.owner = -1;
    planet.original_owner = -1;

    planet.orbital_pos_r = float(random_unit_double() * 2 * std::numbers::pi);

    return growth_factor_considered_habitable < growth_factor;
}

void generation::detail::generate_hex(
    hex_t & hex,
    int hex_index,
    game_state_t & game_state,
    game_start_params_t const & params,
    double map_radius,
    double bulge_radius,
    hex_coord_t center_hex,
    point_2d center_hex_pos,
    int habitable_systems,
    hex_scratch & scratch)
{
    assert(habitable_systems < params.systems_per_hex);

    hex_coord_t const hc = from_index(hex_index, game_state.map_width);

    hex.coord = hc;

    hex.first_system = hex_index * params.systems_per_hex;
    hex.last_system = hex.first_system + params.systems_per_hex;

    auto const pos = hex_position(hc, game_state.map_height);
    if (within(center_hex_pos, pos, bulge_radius)) {
        if (hc == center_hex)
            hex.province_id = prov_galactic_center;
        else
            hex.province_id = prov_galactic_bulge;
        return;
    } else if (!within(center_hex_pos, pos, map_radius)) {
        hex.province_id = prov_off_map;
        return;
    }

    hex.province_id = prov_none;

#if defined(BUILD_FOR_TEST)
    if (detail::g_skip_system_generation_for_testing)
        return;
#endif

    // TODO: With a fairly large number of systems in a hex, also consider
    // giving each of them a vertical coordinate.

    scratch.systems_.resize(params.systems_per_hex);

    int first_uninhabitable_index = 0;
    for (int i = 0; i < params.systems_per_hex; ++i) {
        auto & planets = scratch.systems_[i];
        int const system_index = hex.first_system + i;
        if (detail::generate_system(
                game_state.systems[system_index],
                planets,
                hc,
                pos,
                system_index)) {
            auto first_uninhabitable_it =
                scratch.systems_.begin() + first_uninhabitable_index;
            if (first_uninhabitable_it != scratch.systems_.end())
                std::swap(*first_uninhabitable_it, scratch.systems_[i]);
            ++first_uninhabitable_index;
        }
    }

    // If we happened not to generate enough habitable systems in the first
    // systems_per_hex generated systems, keep going as long as necessary.
    while (first_uninhabitable_index < habitable_systems) {
        auto & planets = scratch.systems_[first_uninhabitable_index];
        int const system_index = hex.first_system + first_uninhabitable_index;
        if (detail::generate_system(
                game_state.systems[system_index],
                planets,
                hc,
                pos,
                system_index)) {
            ++first_uninhabitable_index;
        }
    }
}

void generation::generate_galaxy(
    game_start_params_t const & params,
    game_state_t & game_state,
    task_system * ts_ptr,
    concurrent_queue<int> * percent_complete,
    std::atomic_bool * fully_complete)
{
    auto [map_radius, bulge_radius, center_hex, center_hex_pos] =
        detail::galaxy_shape(params, game_state);

    std::normal_distribution<double> habitable_systems_dist(
        params.habitable_systems_per_hex_mean,
        ::detail::plus_minus_to_sigma(
            params.habitable_systems_per_hex_plus_minus));

    detail::scratch_space scratch(game_state.hexes.size());
    game_state.systems.resize(game_state.hexes.size() * params.systems_per_hex);

    std::atomic_int hexes_generated = 0;

    std::unique_ptr<task_system> local_ts_ptr;
    if (!ts_ptr)
        local_ts_ptr.reset(new task_system(4));
    task_system & ts = ts_ptr ? *ts_ptr : *local_ts_ptr;

    int const update_percentage = 5;
    int const five_percent = game_state.hexes.size() * update_percentage / 100;

    int hex_index = 0;
    for (auto & hex : game_state.hexes) {
        auto const habitable_systems =
            int(std::round(random_number(habitable_systems_dist)));
        auto & hex_scratch_ = scratch.hexes_[hex_index];
        ts.async_exec([=,
                       &hex,
                       &game_state,
                       &params,
                       &hex_scratch_,
                       &hexes_generated,
                       &ts] {
            int const finished = hexes_generated.load();
            if (percent_complete && (finished + 1) % five_percent == 0)
                percent_complete->push(update_percentage);
            detail::generate_hex(
                hex,
                hex_index,
                game_state,
                params,
                map_radius,
                bulge_radius,
                center_hex,
                center_hex_pos,
                habitable_systems,
                hex_scratch_);
            ++hexes_generated;
        });
        ++hex_index;
    }

    while (hexes_generated.load() < game_state.hexes.size()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (percent_complete)
        percent_complete->done();

    hex_index = 0;
    int system_id = 0;
    for (auto & hex : game_state.hexes) {
        auto & hex_scratch_ = scratch.hexes_[hex_index];
        auto system_it = game_state.systems.begin() + system_id;
        for (auto & system_scratch_ : hex_scratch_.systems_) {
            auto const prev_size = game_state.planets.size();
            game_state.planets.resize(
                prev_size + system_scratch_.planets_.size());
            for (auto & planet : system_scratch_.planets_) {
                planet.system_id = system_id;
            }
            std::ranges::move(
                system_scratch_.planets_,
                game_state.planets.begin() + prev_size);
            system_it->first_planet = prev_size;
            system_it->last_planet = game_state.planets.size();
            assert(
                game_state.planets[system_it->first_planet].system_id ==
                system_id);
            assert(
                game_state.planets[system_it->last_planet - 1].system_id ==
                system_id);
            ++system_it;
            ++system_id;
        }

        ++hex_index;
    }

    if (fully_complete)
        *fully_complete = true;
}
