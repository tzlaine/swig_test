#include "check.hpp"
#include "constants.hpp"
#include "generate_galaxy.hpp"
#include "model_util.hpp"
#include "rng.hpp"
#include "task_system.hpp"

#include <numbers>

#define INSTRUMENT_REROLLS 0
#if INSTRUMENT_REROLLS
#include <game_data_formatters.hpp>
#endif


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
    planet_t & planet, system_t const & system, bool homeworld_reroll)
{
    using namespace adobe::literals;

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

    // TODO: The probability of a rocky planet past the first gas/ice giant
    // should be nearly zero.

    // TODO: High-temp rocky planets with a very low atmospheric should not be
    // a thing.

    // Arbitrary line between the gas giants and the ice giants, taken from
    // the boundary between the Saturn and Uranus orbits, and scaled by solar
    // masses of this system's star..
    double const ice_distance_au =
        ice_giant_distance_factor * system.star.solar_masses;

    double density = 0.0; // in kg/km^3
    if (homeworld_reroll) {
        density = random_number(rocky_density_dist);
        if (earth_mass_kg < planet.mass_kg)
            planet.mass_kg *= 0.9;
    } else
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

    if (!homeworld_reroll) {
        // Kepler's third law: T^2 = (4pi^2/GM)a^3
        double const a = planet.orbit_au * km_per_au * m_per_km;
        double const a3 = a * a * a;
        double const M = system.star.solar_masses * sun_mass_kg;
        planet.orbital_period_y =
            std::sqrt(4 * std::numbers::pi * std::numbers::pi / (G_ * M) * a3) /
            secs_per_year;
    }

    if (homeworld_reroll) {
        std::normal_distribution<double> dist(1.0, 0.025);
        planet.o2_co2_suitability = random_number(dist);
        planet.atmospheric_pressure = random_number(dist);
        planet.surface_temperature_k -=
            (planet.surface_temperature_k - earth_temperature_k) / 2;
        if (planet.magnetosphere_strength < 1.0) {
            planet.magnetosphere_strength -=
                (planet.magnetosphere_strength - 1) / 2;
        }
    } else {
        planet.surface_temperature_k =
            system.star.temperature_k *
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
                planet.atmospheric_pressure =
                    high_temp_atmosphere_pressure_factor *
                    random_number(atmos_dist) * planet.mass_kg / earth_mass_kg;
            } else if (planet.magnetosphere_strength < 0.01) {
                double const no_mag_roll = random_unit_double();
                if (no_mag_roll <
                    prob_no_magnetosphere_rocky_planet_is_reduced) {
                    planet.atmosphere_type = atmosphere_type_t::reduced_type_a;
                    planet.atmospheric_pressure =
                        reduced_rocky_planet_pressure_factor *
                        random_number(atmos_dist) * planet.mass_kg /
                        earth_mass_kg;
                } else {
                    planet.atmosphere_type =
                        atmosphere_type_t::carbon_rich_type_c;
                    planet.atmospheric_pressure =
                        nonreduced_rocky_planet_pressure_factor *
                        random_number(atmos_dist) * planet.mass_kg /
                        earth_mass_kg;
                }
            } else {
                planet.atmosphere_type = atmosphere_type_t::oxidized_type_b;
                double const scale = o2_dist.max() - o2_dist.min();
                planet.o2_co2_suitability =
                    1.0 - random_number(o2_dist) / scale;
                planet.atmospheric_pressure =
                    random_number(atmos_dist) * planet.mass_kg / earth_mass_kg;
            }
        } else {
            planet.magnetosphere_strength =
                random_number(giant_magnetosphere_dist);
            if (planet.planet_type == planet_type_t::gas_giant) {
                planet.atmosphere_type =
                    atmosphere_type_t::gas_giant_atmosphere;
                planet.atmospheric_pressure = atmos_millions;
            } else {
                planet.atmosphere_type =
                    atmosphere_type_t::ice_giant_atmosphere;
                planet.atmospheric_pressure = atmos_thousands;
            }
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

    if (homeworld_reroll)
        planet.effects.clear();

    double const growth_factor =
        call_lua_func("determine_growth_factor_and_effects", planet);

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

    // TODO: Give each hex a bias (+ive or -ive) for fuel_minerals and metal, since
    // presence of heavy elements is stellar-neighborhood-dependent.

    if (planet.planet_type == planet_type_t::rocky) {
        double const scale =
            planet.magnetosphere_strength ? planet.magnetosphere_strength : 1.0;
        planet.metal = clamp_res(scale * random_number(resource_dist));
        if (homeworld_reroll)
            planet.metal = clamp_res(planet.metal + max_resource_value * 0.25);
    } else {
        planet.metal = clamp_res(moon_factor * random_number(resource_dist));
    }

    if (planet.planet_type == planet_type_t::rocky) {
        planet.fuel_minerals = clamp_res(random_number(resource_dist));
        if (homeworld_reroll)
            planet.fuel_minerals = clamp_res(planet.fuel_minerals + max_resource_value * 0.25);
    } else {
        planet.fuel_minerals = clamp_res(moon_factor * random_number(resource_dist));
    }

    planet.orbital_pos_r = float(random_unit_double() * 2 * std::numbers::pi);

    planet.infrastructure_cost_factor = 1.0f;

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
    check(habitable_systems < params.systems_per_hex);

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
                hex_index,
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
                hex_index,
                pos,
                system_index)) {
            ++first_uninhabitable_index;
        }
    }
}

std::vector<candidate_planet>
generation::detail::find_starting_locations(game_state_t & gs, int n)
{
    auto const score = [](planet_t const & planet) {
        return call_lua_func("starting_planet_score", planet);
    };
    auto retval = scored_planets(gs, score);
    only_top_planets(retval, n);

    for (auto & candidate : retval) {
        system_t const & system = gs.systems[candidate.planet_->system_id];
#if INSTRUMENT_REROLLS
        std::cout << std::format(
            "planet initially: max_pop={} growth_factor={} metal={} fuel_minerals={} "
            "score={}\n{}\n\n",
            candidate.planet_->max_population,
            candidate.planet_->growth_factor,
            candidate.planet_->metal,
            candidate.planet_->fuel_minerals,
            candidate.score_,
            *candidate.planet_);
#endif
        int max_iterations = 100000;
        while (starting_pop_plus_minus <
                   std::abs(
                       candidate.planet_->max_population - mean_starting_pop) ||
               candidate.score_ < 0) {
            if (!--max_iterations)
                break;
            generate_planet(*candidate.planet_, system, true);
            candidate.score_ = score(*candidate.planet_);
#if INSTRUMENT_REROLLS
            std::cout << std::format(
                "planet reroll: max_pop={} growth_factor={} metal={} fuel_minerals={} score={}\n{}\n\n",
                candidate.planet_->max_population,
                candidate.planet_->growth_factor,
                candidate.planet_->metal,
                candidate.planet_->fuel_minerals,
                candidate.score_,
                *candidate.planet_);
#endif
        }
    }

    return retval;
}

void generation::generate_galaxy(
    game_start_params_t const & params,
    game_state_t & game_state,
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

    task_system ts(4);

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
                       &hexes_generated] {
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

    int total_planets = 0;
    for (auto & hex_scratch_ : scratch.hexes_) {
        for (auto & system_scratch_ : hex_scratch_.systems_) {
            total_planets += (int)system_scratch_.planets_.size();
        }
    }
    game_state.planets.resize(total_planets);

    auto it = game_state.planets.begin();
    int hex_id = 0;
    int system_id = 0;
    for (auto & hex_scratch_ : scratch.hexes_) {
        auto const province_id = game_state.hexes[hex_id].province_id;
        for (auto & system_scratch_ : hex_scratch_.systems_) {
            game_state.systems[system_id].first_planet =
                std::distance(game_state.planets.begin(), it);
            for (auto & planet : system_scratch_.planets_) {
                planet.system_id = system_id;
            }
            it = std::ranges::move(system_scratch_.planets_, it).out;
            game_state.systems[system_id].last_planet =
                std::distance(game_state.planets.begin(), it);
            ++system_id;
        }
        ++hex_id;

        // NB: This advances the system_id, even if hex_scratch_.systens_
        // is empty.  Some are -- the ones in the core and the corners of
        // the map.
        system_id = hex_id * params.systems_per_hex;
    }

    if (fully_complete)
        *fully_complete = true;
}

void generation::generate_nations(
    game_start_params_t const & params, game_state_t & gs)
{
    int const n =
        (int)params.player_id_to_nation_id.size() + params.ai_opponents;

    std::vector<candidate_planet> const homeworlds =
        generation::detail::find_starting_locations(gs, n);

    for (int i = 0; i < n; ++i) {
        call_lua_func(
            "create_starting_nation",
            gs,
            i,
            *homeworlds[i].planet_,
            homeworlds[i].planet_id_);
        std::ranges::sort(gs.nations[i].hexes_seen);
        std::ranges::sort(gs.nations[i].systems_present_in);
        std::ranges::sort(gs.nations[i].systems_visited);
        std::ranges::sort(gs.nations[i].planets_surveyed);
    }
}
