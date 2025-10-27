#include <game_data_t.hpp>
#include <json2pb.h>
#include <generate_galaxy.hpp>

#include <game_data_formatters.hpp>

#include <gtest/gtest.h>

#include <format>
#include <strstream>


using namespace adobe::literals;

// TODO: Move these to a testing utility header?
inline double eps = 0.000001; // tolerance for testing float closeness.

template<typename T>
void dump(T const & x)
{
    std::cout << std::format("{}", x) << "\n";
}

void truncate(float & x, int decimal_places)
{
    if (decimal_places < 0)
        return;
    float const factor = std::pow(10.0, decimal_places);
    x = std::trunc(x * factor) / factor;
}

void print_simple_hexes(std::ostream & os, game_state_t const & game_state)
{
    for (int j = 0; j < game_state.map_height; ++j) {
        for (int i = 0; i < game_state.map_width; ++i) {
            auto const & hex = game_state.hexes[j * game_state.map_width + i];
            switch (hex.province_id) {
            case prov_off_map: os << ":"; break;
            case prov_galactic_bulge: os << "X"; break;
            case prov_galactic_center: os << "C"; break;
            default: os << "N"; break;
            };
        }
        os << "\n";
    }
}

TEST(generation_tests, hex_shape_of_map)
{
    generation::detail::g_skip_system_generation_for_testing = true;

    {
        game_start_params params{.map_height=11};
        game_state_t game_state;
        generation::generate_galaxy(params, game_state);

        EXPECT_EQ(game_state.hexes.size(),
                  (size_t)(game_state.map_width * game_state.map_height));

        std::ostringstream oss;
        oss << "\n";
        print_simple_hexes(oss, game_state);

        std::string const expected = R"(
:::NNNNNNN:::
::NNNNNNNNN::
:NNNNNNNNNNN:
:NNNNNNNNNNN:
NNNNNXXXNNNNN
NNNNNXCXNNNNN
NNNNNNXNNNNNN
:NNNNNNNNNNN:
::NNNNNNNNN::
::NNNNNNNNN::
::::N:N:N::::
)";

        EXPECT_EQ(oss.str(), expected);
    }

    {
        game_start_params params{.map_height=15};
        game_state_t game_state;
        generation::generate_galaxy(params, game_state);

        EXPECT_EQ(game_state.hexes.size(),
                  (size_t)(game_state.map_width * game_state.map_height));

        std::ostringstream oss;
        oss << "\n";
        print_simple_hexes(oss, game_state);

        std::string const expected = R"(
:::::NNNNNNN:::::
:::NNNNNNNNNNN:::
::NNNNNNNNNNNNN::
:NNNNNNNNNNNNNNN:
:NNNNNNNNNNNNNNN:
NNNNNNNNNNNNNNNNN
NNNNNNNXXXNNNNNNN
NNNNNNNXCXNNNNNNN
NNNNNNNNXNNNNNNNN
NNNNNNNNNNNNNNNNN
:NNNNNNNNNNNNNNN:
::NNNNNNNNNNNNN::
::NNNNNNNNNNNNN::
::::NNNNNNNNN::::
::::::N:N:N::::::
)";

        EXPECT_EQ(oss.str(), expected);
    }

    generation::detail::g_skip_system_generation_for_testing = false;
}

TEST(generation_tests, growth_factor_and_effects)
{
    // rocky
    planet_t const earth{
        .planet_type=planet_type_t::rocky,
        .mass_kg=earth_mass_kg,
        .radius_km=earth_radius_km,
        .orbit_au=1,
        .orbital_period_y=1,
        .gravity_g=1,
        .axial_tilt_d=23,
        .day_h=24,
        .surface_temperature_k=earth_temperature_k,
        .magnetosphere_strength=1,
        .atmopsheric_pressure=1,
        .o2_co2_suitability=1,
        .ocean_coverage=earth_ocean_coverage,
        .growth_factor=base_pop_growth_factor,
        .atmosphere_type=atmosphere_type_t::oxidized_type_b,
        .water=100,
        .food=100,
        .energy=100,
        .metal=100,
        .fuel=100,
        .max_population=100
    };

    {
        // Earth had better have the base habitability, and no effects.
        planet_t planet = earth;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_EQ(result, base_pop_growth_factor);
        EXPECT_EQ(planet.effects.size(), 0u);
    }

    // gravity
    {
        planet_t planet = earth;
        planet.gravity_g = 0.05f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.2, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected{
            .name="very_low_grav"_name,
            .description="very_low_grav_desc"_name,
            .amount=-0.2,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.gravity_g = 0.5f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected{
            .name="low_grav"_name,
            .description="low_grav_desc"_name,
            .amount=-0.05,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.gravity_g = 1.2f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + 0.01, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        truncate(planet.effects[0].amount, 2);
        planet_effect_t const expected{
            .name="high_grav"_name,
            .description="high_grav_desc"_name,
            .amount=0.01,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.gravity_g = 1.5f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.2, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        truncate(planet.effects[0].amount, 1);
        planet_effect_t const expected{
            .name="very_high_grav"_name,
            .description="very_high_grav_desc"_name,
            .amount=-0.2,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }

    // tilt
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 1.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + 0.05, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected{
            .name="no_seasons"_name,
            .description="no_seasons_desc"_name,
            .amount=0.05,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 10.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + 0.025, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected{
            .name="mild_seasons"_name,
            .description="mild_seasons_desc"_name,
            .amount=0.025,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 20.0f;
        planet.orbital_period_y = 0.5f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + 0.0125, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected{
            .name="short_seasons"_name,
            .description="short_seasons_desc"_name,
            .amount=0.0125,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 20.0f;
        planet.orbital_period_y = 2.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.1, eps);
        EXPECT_EQ(planet.effects.size(), 2u);
        planet_effect_t const expected[] = {
            {
                .name="long_seasons"_name,
                .description="long_seasons_desc"_name,
                .amount=-0.1,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_seasons_infra_cost_effect"_name,
                .description="long_seasons_infra_cost_effect_desc"_name,
                .amount=agri_equip_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 20.0f;
        planet.orbital_period_y = 4.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.2, eps);
        EXPECT_EQ(planet.effects.size(), 3u);
        planet_effect_t const expected[] = {
            {
                .name="long_seasons"_name,
                .description="long_seasons_desc"_name,
                .amount=-0.2,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_seasons_infra_cost_effect"_name,
                .description="long_seasons_infra_cost_effect_desc"_name,
                .amount=agri_equip_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            },
            {
                .name="only_equatorial_band_habitable"_name,
                .description="only_equatorial_band_habitable_desc"_name,
                .amount=only_equatorial_region_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 40.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected{
            .name="intense_seasons"_name,
            .description="intense_seasons_desc"_name,
            .amount=-0.05,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 40.0f;
        planet.orbital_period_y = 0.5f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor  - 0.05 + 0.0125, eps);
        EXPECT_EQ(planet.effects.size(), 2u);
        planet_effect_t const expected[] = {
            {
                .name="intense_seasons"_name,
                .description="intense_seasons_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="short_seasons"_name,
                .description="short_seasons_desc"_name,
                .amount=0.0125,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 40.0f;
        planet.orbital_period_y = 1.3f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05 - 0.065, eps);
        EXPECT_EQ(planet.effects.size(), 3u);
        planet_effect_t const expected[] = {
            {
                .name="intense_seasons"_name,
                .description="intense_seasons_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_seasons"_name,
                .description="long_seasons_desc"_name,
                .amount=-0.065,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_intense_seasons_infra_cost_effect"_name,
                .description="long_intense_seasons_infra_cost_effect_desc"_name,
                .amount=1.5,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 40.0f;
        planet.orbital_period_y = 4.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05 - 0.2, eps);
        EXPECT_EQ(planet.effects.size(), 4u);
        planet_effect_t const expected[] = {
            {
                .name="intense_seasons"_name,
                .description="intense_seasons_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_seasons"_name,
                .description="long_seasons_desc"_name,
                .amount=-0.2,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_intense_seasons_infra_cost_effect"_name,
                .description="long_intense_seasons_infra_cost_effect_desc"_name,
                .amount=1.5,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            },
            {
                .name="only_equatorial_band_habitable"_name,
                .description="only_equatorial_band_habitable_desc"_name,
                .amount=only_equatorial_region_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
        EXPECT_EQ(planet.effects[3], expected[3]);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 50.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05, eps);
        EXPECT_EQ(planet.effects.size(), 2u);
        planet_effect_t const expected[] = {
            {
                .name="intense_seasons"_name,
                .description="intense_seasons_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="only_equatorial_band_habitable"_name,
                .description="only_equatorial_band_habitable_desc"_name,
                .amount=only_equatorial_region_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 50.0f;
        planet.orbital_period_y = 0.5f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor  - 0.05 + 0.0125, eps);
        EXPECT_EQ(planet.effects.size(), 3u);
        planet_effect_t const expected[] = {
            {
                .name="intense_seasons"_name,
                .description="intense_seasons_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="short_seasons"_name,
                .description="short_seasons_desc"_name,
                .amount=0.0125,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="only_equatorial_band_habitable"_name,
                .description="only_equatorial_band_habitable_desc"_name,
                .amount=only_equatorial_region_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 50.0f;
        planet.orbital_period_y = 1.3f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05 - 0.065, eps);
        EXPECT_EQ(planet.effects.size(), 4u);
        planet_effect_t const expected[] = {
            {
                .name="intense_seasons"_name,
                .description="intense_seasons_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_seasons"_name,
                .description="long_seasons_desc"_name,
                .amount=-0.065,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_intense_seasons_infra_cost_effect"_name,
                .description="long_intense_seasons_infra_cost_effect_desc"_name,
                .amount=1.5,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            },
            {
                .name="only_equatorial_band_habitable"_name,
                .description="only_equatorial_band_habitable_desc"_name,
                .amount=only_equatorial_region_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
        EXPECT_EQ(planet.effects[3], expected[3]);
    }
    {
        planet_t planet = earth;
        planet.axial_tilt_d = 50.0f;
        planet.orbital_period_y = 4.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05 - 0.2, eps);
        EXPECT_EQ(planet.effects.size(), 4u);
        planet_effect_t const expected[] = {
            {
                .name="intense_seasons"_name,
                .description="intense_seasons_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_seasons"_name,
                .description="long_seasons_desc"_name,
                .amount=-0.2,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_intense_seasons_infra_cost_effect"_name,
                .description="long_intense_seasons_infra_cost_effect_desc"_name,
                .amount=1.5,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            },
            {
                .name="only_equatorial_band_habitable"_name,
                .description="only_equatorial_band_habitable_desc"_name,
                .amount=only_equatorial_region_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
        EXPECT_EQ(planet.effects[3], expected[3]);
    }

    // day length
    {
        planet_t planet = earth;
        planet.day_h = 12.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor -0.2, eps);
        EXPECT_EQ(planet.effects.size(), 2u);
        planet_effect_t const expected[] = {
            {
                .name="short_days"_name,
                .description="short_days_desc"_name,
                .amount=-0.2,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="short_days_infra_cost_effect"_name,
                .description="short_days_infra_cost_effect_desc"_name,
                .amount=agri_equip_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
    }
    {
        planet_t planet = earth;
        planet.day_h = 29.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05, eps);
        EXPECT_EQ(planet.effects.size(), 2u);
        planet_effect_t const expected[] = {
            {
                .name="long_days"_name,
                .description="long_days_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_days_infra_cost_effect"_name,
                .description="long_days_infra_cost_effect_desc"_name,
                .amount=agri_equip_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
    }
    {
        planet_t planet = earth;
        planet.day_h = 36.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.1, eps);
        EXPECT_EQ(planet.effects.size(), 2u);
        planet_effect_t const expected[] = {
            {
                .name="long_days"_name,
                .description="long_days_desc"_name,
                .amount=-0.1,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="long_days_infra_cost_effect"_name,
                .description="long_days_infra_cost_effect_desc"_name,
                .amount=agri_equip_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
    }
    {
        planet_t planet = earth;
        planet.day_h = 53.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.15, eps);
        EXPECT_EQ(planet.effects.size(), 2u);
        planet_effect_t const expected[] = {
            {
                .name="very_long_days"_name,
                .description="very_long_days_desc"_name,
                .amount=-0.15,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="very_long_days_infra_cost_effect"_name,
                .description="very_long_days_infra_cost_effect_desc"_name,
                .amount=agri_equip_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
    }
    {
        planet_t planet = earth;
        planet.day_h = 99.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.2, eps);
        EXPECT_EQ(planet.effects.size(), 2u);
        planet_effect_t const expected[] = {
            {
                .name="very_long_days"_name,
                .description="very_long_days_desc"_name,
                .amount=-0.2,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="very_long_days_infra_cost_effect"_name,
                .description="very_long_days_infra_cost_effect_desc"_name,
                .amount=agri_equip_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .operation=effect_op_t::multiply
            }
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
    }

    // O2
    {
        planet_t planet = earth;
        planet.o2_co2_suitability = 0.25f;  // 0.75 together
        planet.atmopsheric_pressure = 3.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.045, 0.001);
        EXPECT_EQ(planet.effects.size(), 1u);
        truncate(planet.effects[0].amount, 3);
        planet_effect_t const expected{
            .name="poor_o2_co2_suitab"_name,
            .description="poor_o2_co2_suitab_desc"_name,
            .amount=-0.045,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.o2_co2_suitability = 0.45f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.12, 0.005);
        EXPECT_EQ(planet.effects.size(), 3u);
        truncate(planet.effects[0].amount, 2);
        truncate(planet.effects[1].amount, 2);
        truncate(planet.effects[2].amount, 2);
        planet_effect_t const expected[] = {
            {
                .name="very_poor_o2_co2_suitab"_name,
                .description="very_poor_o2_co2_suitab_desc"_name,
                .amount=-0.12,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="very_poor_o2_co2_suitab_habs_and_masks_infra_cost_effect"_name,
                .description="very_poor_o2_co2_suitab_habs_and_masks_infra_cost_effect_desc"_name,
                .amount=2,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="very_poor_o2_co2_suitab_habs_and_masks_pop_effect"_name,
                .description="very_poor_o2_co2_suitab_habs_and_masks_pop_effect_desc"_name,
                .amount=0.25,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.o2_co2_suitability = 0.35f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.14, 0.01);
        EXPECT_EQ(planet.effects.size(), 3u);
        truncate(planet.effects[0].amount, 2);
        truncate(planet.effects[1].amount, 2);
        truncate(planet.effects[2].amount, 2);
        planet_effect_t const expected[] = {
            {
                .name="marginal_o2_co2_suitab"_name,
                .description="marginal_o2_co2_suitab_desc"_name,
                .amount=-0.14,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="marginal_o2_co2_suitab_habs_and_masks_infra_cost_effect"_name,
                .description="marginal_o2_co2_suitab_habs_and_masks_infra_cost_effect_desc"_name,
                .amount=habs_and_masks_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="marginal_o2_co2_suitab_habs_and_masks_pop_effect"_name,
                .description="marginal_o2_co2_suitab_habs_and_masks_pop_effect_desc"_name,
                .amount=habs_and_masks_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.o2_co2_suitability = 0.25f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.2, 0.01);
        EXPECT_EQ(planet.effects.size(), 3u);
        truncate(planet.effects[0].amount, 2);
        truncate(planet.effects[1].amount, 2);
        truncate(planet.effects[2].amount, 2);
        planet_effect_t const expected[] = {
            {
                .name="insufficient_o2_co2_suitab"_name,
                .description="insufficient_o2_co2_suitab_desc"_name,
                .amount=-0.2,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="insufficient_o2_co2_suitab_habs_and_suits_infra_cost_effect"_name,
                .description="insufficient_o2_co2_suitab_habs_and_suits_infra_cost_effect_desc"_name,
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="insufficient_o2_co2_suitab_habs_and_suits_pop_effect"_name,
                .description="insufficient_o2_co2_suitab_habs_and_suits_pop_effect_desc"_name,
                .amount=habs_and_suits_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }

    // atmospheric pressure
    {
        planet_t planet = earth;
        planet.atmopsheric_pressure = 5.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + habs_and_suits_growth_modifier, 0.01);
        EXPECT_EQ(planet.effects.size(), 3u);
        truncate(planet.effects[0].amount, 2);
        truncate(planet.effects[1].amount, 2);
        truncate(planet.effects[2].amount, 2);
        planet_effect_t const expected[] = {
            {
                .name="high_press_n2_narcosis"_name,
                .description="high_press_n2_narcosis_desc"_name,
                .amount=habs_and_suits_growth_modifier,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="high_press_n2_narcosis_habs_and_suits_infra_cost_effect"_name,
                .description="high_press_n2_narcosis_habs_and_suits_infra_cost_effect_desc"_name,
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="high_press_n2_narcosis_habs_and_suits_pop_effect"_name,
                .description="high_press_n2_narcosis_habs_and_suits_pop_effect_desc"_name,
                .amount=habs_and_suits_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.atmopsheric_pressure = 8.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + 2 * habs_and_suits_growth_modifier, eps);
        EXPECT_EQ(planet.effects.size(), 4u);
        planet_effect_t const expected[] = {
            {
                .name="high_press_n2_narcosis"_name,
                .description="high_press_n2_narcosis_desc"_name,
                .amount=habs_and_suits_growth_modifier,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="very_high_press_o2_toxicity"_name,
                .description="very_high_press_o2_toxicity_desc"_name,
                .amount=habs_and_suits_growth_modifier,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="high_press_n2_narcosis_habs_and_suits_infra_cost_effect"_name,
                .description="high_press_n2_narcosis_habs_and_suits_infra_cost_effect_desc"_name,
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="high_press_n2_narcosis_habs_and_suits_pop_effect"_name,
                .description="high_press_n2_narcosis_habs_and_suits_pop_effect_desc"_name,
                .amount=habs_and_suits_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
        EXPECT_EQ(planet.effects[3], expected[3]);
    }

    // magnetosphere
    {
        planet_t planet = earth;
        planet.magnetosphere_strength = 0.25f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + habs_and_suits_growth_modifier, eps);
        EXPECT_EQ(planet.effects.size(), 3u);
        planet_effect_t const expected[] = {
            {
                .name="very_weak_magneto"_name,
                .description="very_weak_magneto_desc"_name,
                .amount=habs_and_suits_growth_modifier,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="very_weak_magneto_habs_and_suits_infra_cost_effect"_name,
                .description="very_weak_magneto_habs_and_suits_infra_cost_effect_desc"_name,
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="very_weak_magneto_habs_and_suits_pop_effect"_name,
                .description="very_weak_magneto_habs_and_suits_pop_effect_desc"_name,
                .amount=habs_and_suits_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.magnetosphere_strength = 0.5f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.5, eps);
        EXPECT_EQ(planet.effects.size(), 3u);
        planet_effect_t const expected[] = {
            {
                .name="weak_magneto"_name,
                .description="weak_magneto_desc"_name,
                .amount=-0.5,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="weak_magneto_habs_and_masks_infra_cost_effect"_name,
                .description="weak_magneto_habs_and_masks_infra_cost_effect_desc"_name,
                .amount=habs_and_masks_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="weak_magneto_habs_and_masks_pop_effect"_name,
                .description="weak_magneto_habs_and_masks_pop_effect_desc"_name,
                .amount=habs_and_masks_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.magnetosphere_strength = 0.75f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.25, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected = {
                .name="weak_magneto"_name,
                .description="weak_magneto_desc"_name,
                .amount=-0.25,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
        };
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.magnetosphere_strength = 1.6f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + 0.01, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        truncate(planet.effects[0].amount, 2);
        planet_effect_t const expected = {
            .name="strong_magneto"_name,
            .description="strong_magneto_desc"_name,
            .amount=0.01,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add
        };
        EXPECT_EQ(planet.effects[0], expected);
    }

    // temperature
    {
        planet_t planet = earth;
        planet.surface_temperature_k = earth_temperature_k - 50;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + habs_and_suits_growth_modifier, eps);
        EXPECT_EQ(planet.effects.size(), 3u);
        planet_effect_t const expected[] = {
            {
                .name="extremely_cold_avg_surface_temp"_name,
                .description="extremely_cold_avg_surface_temp_desc"_name,
                .amount=habs_and_suits_growth_modifier,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="extremely_cold_avg_surface_temp_habs_and_suits_infra_cost_effect"_name,
                .description="extremely_cold_avg_surface_temp_habs_and_suits_infra_cost_effect_desc"_name,
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="extremely_cold_avg_surface_temp_habs_and_suits_pop_effect"_name,
                .description="extremely_cold_avg_surface_temp_habs_and_suits_pop_effect_desc"_name,
                .amount=habs_and_suits_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.surface_temperature_k = earth_temperature_k - 31;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.6, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        truncate(planet.effects[0].amount, 2);
        planet_effect_t const expected = {
            .name="very_cold_avg_surface_temp"_name,
            .description="very_cold_avg_surface_temp_desc"_name,
            .amount=-0.6,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add
        };
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.surface_temperature_k = earth_temperature_k + 21;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.1, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        truncate(planet.effects[0].amount, 2);
        planet_effect_t const expected = {
            .name="hot_avg_surface_temp"_name,
            .description="hot_avg_surface_temp_desc"_name,
            .amount=-0.1,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add
        };
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.surface_temperature_k = earth_temperature_k + 31;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.6, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        truncate(planet.effects[0].amount, 2);
        planet_effect_t const expected = {
            .name="very_hot_avg_surface_temp"_name,
            .description="very_hot_avg_surface_temp_desc"_name,
            .amount=-0.6,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add
        };
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.surface_temperature_k = earth_temperature_k + 54;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + habs_and_suits_growth_modifier, eps);
        EXPECT_EQ(planet.effects.size(), 3u);
        planet_effect_t const expected[] = {
            {
                .name="extremely_hot_avg_surface_temp"_name,
                .description="extremely_hot_avg_surface_temp_desc"_name,
                .amount=habs_and_suits_growth_modifier,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="extremely_hot_avg_surface_temp_habs_and_suits_infra_cost_effect"_name,
                .description="extremely_hot_avg_surface_temp_habs_and_suits_infra_cost_effect_desc"_name,
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="extremely_hot_avg_surface_temp_habs_and_suits_pop_effect"_name,
                .description="extremely_hot_avg_surface_temp_habs_and_suits_pop_effect_desc"_name,
                .amount=habs_and_suits_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }
    {
        planet_t planet = earth;
        planet.surface_temperature_k = earth_temperature_k + 60;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + growth_uninhabitable, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        truncate(planet.effects[0].amount, 2);
        planet_effect_t const expected = {
            .name="uninhabitably_hot_avg_surface_temp"_name,
            .description="uninhabitably_hot_avg_surface_temp_desc"_name,
            .amount=growth_uninhabitable,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add
        };
        EXPECT_EQ(planet.effects[0], expected);
    }

    // Requiring hab+suits wipes out other effects, except those based on
    // gravity.
    {
        planet_t planet = earth;
        planet.o2_co2_suitability = 0.45f;     // Comes first.
        planet.magnetosphere_strength = 0.25f; // Requires habs+suits.
        planet.gravity_g = 0.50f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05 + habs_and_suits_growth_modifier, 0.005);
        EXPECT_EQ(planet.effects.size(), 4u);
        truncate(planet.effects[0].amount, 2);
        truncate(planet.effects[1].amount, 2);
        truncate(planet.effects[2].amount, 2);
        truncate(planet.effects[3].amount, 2);
        planet_effect_t const expected[] = {
            {
                .name="very_weak_magneto"_name,
                .description="very_weak_magneto_desc"_name,
                .amount=habs_and_suits_growth_modifier,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="low_grav"_name,
                .description="low_grav_desc"_name,
                .amount=-0.05,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="very_weak_magneto_habs_and_suits_infra_cost_effect"_name,
                .description="very_weak_magneto_habs_and_suits_infra_cost_effect_desc"_name,
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=(unsigned int)planet_effect_mod_t::cost,
                .operation=effect_op_t::multiply
            },
            {
                .name="very_weak_magneto_habs_and_suits_pop_effect"_name,
                .description="very_weak_magneto_habs_and_suits_pop_effect_desc"_name,
                .amount=habs_and_suits_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
        EXPECT_EQ(planet.effects[3], expected[3]);
    }

    // gas giant
    planet_t const a_gas_giant{
        .planet_type=planet_type_t::gas_giant,
        .mass_kg=earth_mass_kg * 1000,
        .radius_km=earth_radius_km * 10,
        .orbit_au=15,
        .orbital_period_y=30,
        .gravity_g=1000,
        .axial_tilt_d=10,
        .day_h=36,
        .surface_temperature_k=earth_temperature_k / 2,
        .magnetosphere_strength=5,
        .atmopsheric_pressure=atmos_millions,
        .o2_co2_suitability=n_a,
        .ocean_coverage=n_a,
        .growth_factor=base_pop_growth_factor,
        .atmosphere_type=atmosphere_type_t::gas_giant_atmosphere,
        .water=0,
        .food=0,
        .energy=3,
        .metal=3,
        .fuel=3,
        .max_population=0
    };
    {
        planet_t planet = a_gas_giant;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + growth_uninhabitable, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected{
            .name="uninhab_non_rocky_planet"_name,
            .description="uninhab_non_rocky_planet_desc"_name,
            .amount=-1000,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }

    // ice giant
    planet_t const an_ice_giant{
        .planet_type=planet_type_t::ice_giant,
        .mass_kg=earth_mass_kg * 8 * 8 * 8,
        .radius_km=earth_radius_km * 8,
        .orbit_au=25,
        .orbital_period_y=90,
        .gravity_g=8 * 8 * 8,
        .axial_tilt_d=10,
        .day_h=12,
        .surface_temperature_k=earth_temperature_k / 10,
        .magnetosphere_strength=2,
        .atmopsheric_pressure=atmos_thousands,
        .o2_co2_suitability=n_a,
        .ocean_coverage=n_a,
        .growth_factor=base_pop_growth_factor,
        .atmosphere_type=atmosphere_type_t::ice_giant_atmosphere,
        .water=0,
        .food=0,
        .energy=3,
        .metal=3,
        .fuel=3,
        .max_population=0
    };
    {
        planet_t planet = an_ice_giant;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor + growth_uninhabitable, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet_effect_t const expected{
            .name="uninhab_non_rocky_planet"_name,
            .description="uninhab_non_rocky_planet_desc"_name,
            .amount=-1000,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
}

TEST(generation_tests, generate_star)
{
    auto const & props = generation::detail::star_properties;

    double accum = 0.0;
    {
        star_t const star = generation::detail::generate_star(0.0);
        EXPECT_EQ(star.star_class, star_class_t::o);
    }
    {
        star_t const star = generation::detail::generate_star(0.0000002);
        EXPECT_EQ(star.star_class, star_class_t::o);
    }
    {
        star_t const star = generation::detail::generate_star(0.0012);
        EXPECT_EQ(star.star_class, star_class_t::b);
    }
    {
        star_t const star = generation::detail::generate_star(0.0061);
        EXPECT_EQ(star.star_class, star_class_t::a);
    }
    {
        star_t const star = generation::detail::generate_star(0.03);
        EXPECT_EQ(star.star_class, star_class_t::f);
    }
    {
        star_t const star = generation::detail::generate_star(0.076);
        EXPECT_EQ(star.star_class, star_class_t::g);
    }
    {
        star_t const star = generation::detail::generate_star(0.12);
        EXPECT_EQ(star.star_class, star_class_t::k);
    }
    {
        star_t const star = generation::detail::generate_star(0.76);
        EXPECT_EQ(star.star_class, star_class_t::m);
    }
    {
        star_t const star = generation::detail::generate_star(1.0);
        EXPECT_EQ(star.star_class, star_class_t::g);
    }

    {
        int const n = 100000;
        std::vector<star_t> stars(n);
        std::ranges::generate(stars, [] {
            return generation::detail::generate_star();
        });

        int counts[(int)star_class_t::m + 1] = {0};
        for (auto const & s : stars) {
            int const index = (int)s.star_class;
            ++counts[index];
            EXPECT_LT(props[index].temperature_.first, s.temperature_k);
            EXPECT_LT(s.temperature_k, props[index].temperature_.second);
            EXPECT_LT(props[index].mass_.first, s.solar_masses);
            EXPECT_LT(s.solar_masses, props[index].mass_.second);
            EXPECT_LT(props[index].luminosity_.first, s.solar_luminosities);
            EXPECT_LT(s.solar_luminosities, props[index].luminosity_.second);
        }

        EXPECT_EQ(counts[0], 0);
        EXPECT_NEAR(1.0 * counts[1] / n, props[1].frequency_, 0.01);
        EXPECT_NEAR(1.0 * counts[2] / n, props[2].frequency_, 0.01);
        EXPECT_NEAR(1.0 * counts[3] / n, props[3].frequency_, 0.01);
        EXPECT_NEAR(1.0 * counts[4] / n, props[4].frequency_, 0.01);

        // Rolls that don't match anything in the table end up here, so this
        // will always be an overcount.
        EXPECT_NEAR(1.0 * counts[5] / n, props[5].frequency_, 0.2);

        EXPECT_NEAR(1.0 * counts[6] / n, props[6].frequency_, 0.01);
        EXPECT_NEAR(1.0 * counts[7] / n, props[7].frequency_, 0.01);
    }
}

// Nearly everything in this function is a random roll, so this is mostly for
// eyeball debugging (getting a rough sense of the way planets are
// distributed, etc.)
TEST(generation_tests, generate_system)
{
    hex_coord_t const hc{0, 0};
    int const map_height = 11;
    auto const pos = hex_position(hc, map_height);
    int const system_id = 0;

    generation::detail::scratch_space::planets_type all_planets;

    std::vector<double> radii;
    std::vector<double> masses;
    auto get_intermediate_values = [&](
        system_t & s, int i, std::vector<double> const & r,
        std::vector<double> const & m, auto ps) {
        radii = std::move(r);
        masses = std::move(m);
        return false;
    };

    {
        system_t system;
        while (system.star.solar_luminosities < 0.8 || 1.2 < system.star.solar_luminosities) {
            system.first_planet = all_planets.size();
            generation::detail::generate_system_impl(
                system, all_planets, hc, pos, system_id, get_intermediate_values);
            system.last_planet = all_planets.size();
        }

        dump(system);

        std::cout << "\n";

        EXPECT_NE(system.first_planet, system.last_planet);
        std::cout << std::format("{} planets\n", system.last_planet - system.first_planet);

        std::cout << "\n";

        for (auto r : radii) {
            std::cout << std::format("{:.3g} AU ", r);
        }
        std::cout << "\n";

        std::cout << "\n";

        for (auto m : masses) {
            std::cout << std::format("{:.3g} kg ", m);
        }
        std::cout << "\n";
    }
}

TEST(generation_tests, generate_planet)
{
    // This function mostly does random rolls, so the tests below are just
    // sanity checks.

    system_t const system = {
        .name=adobe::name_t(""),
        .coord=hex_coord_t{0, 0},
        .star=star_t{
            .star_class=star_class_t::g,
            .temperature_k=5650.598319840416,
            .solar_masses=1.0063179632913541,
            .solar_luminosities=0.9237603942316259,
            .solar_radii=1.0028672334746918 },
        .world_pos_x=0.8916085168659775,
        .world_pos_y=18.538425888758375,
        .first_planet=152,
        .last_planet=158
    };

    std::pair<float, double> const planet_orbits_masses[7] = {
        {0.221f, 1.93e+21},
        {0.329f, 2.11e+23},
        {0.688f, 1.51e+24},
        {0.993f, 4.01e+22},
        {3.66f, 1.83e+26},
        {8.7f, 1.26e+26},
        {20.3f, 1.94e+27}
    };

    for (int i = 0; i < 7 * 2; ++i) {
        auto const orbit_mass = planet_orbits_masses[i % 7];
        planet_t planet = {
            .system_id = 0,
            .mass_kg = orbit_mass.second,
            .orbit_au = orbit_mass.first
        };

        bool const habitable =
            generation::detail::generate_planet(planet, system);

        EXPECT_GT(planet.planet_type, planet_type_t::invalid_planet_type);
        EXPECT_LE(planet.planet_type, planet_type_t::ice_giant);

        // These should be unchanged.
        EXPECT_EQ(planet.mass_kg, orbit_mass.second);
        EXPECT_EQ(planet.orbit_au, orbit_mass.first);

        EXPECT_GT(planet.radius_km, 0.0);
        EXPECT_GT(planet.orbital_period_y, 0.0);
        EXPECT_GT(planet.gravity_g, 0.0);
        EXPECT_GE(planet.axial_tilt_d, 0.0);
        EXPECT_LE(planet.axial_tilt_d, 90.0);
        EXPECT_GT(planet.day_h, 0.0);
        EXPECT_GT(planet.surface_temperature_k, 0.0);
        EXPECT_LT(planet.surface_temperature_k, 2000.0);
        EXPECT_GE(planet.magnetosphere_strength, 0.0);
        if (planet.atmopsheric_pressure != atmos_thousands &&
            planet.atmopsheric_pressure != atmos_millions) {
            EXPECT_GE(planet.atmopsheric_pressure, 0.0);
        }
        EXPECT_GE(planet.o2_co2_suitability, 0.0);
        EXPECT_LE(planet.o2_co2_suitability, 1.0);
        if (planet.ocean_coverage != n_a) {
            EXPECT_GE(planet.ocean_coverage, 0.0);
            EXPECT_LE(planet.ocean_coverage, 1.0);
        }
        EXPECT_GT(planet.atmosphere_type,
                  atmosphere_type_t::invalid_atmosphere_type);
        EXPECT_LE(planet.atmosphere_type,
                  atmosphere_type_t::ice_giant_atmosphere);
        EXPECT_GE(planet.water, 0);
        EXPECT_LE(planet.water, 100);
        EXPECT_GE(planet.food, 0);
        EXPECT_LE(planet.food, 100);
        EXPECT_GE(planet.energy, 0);
        EXPECT_LE(planet.energy, 100);
        EXPECT_GE(planet.metal, 0);
        EXPECT_LE(planet.metal, 100);
        EXPECT_GE(planet.fuel, 0);
        EXPECT_LE(planet.fuel, 100);
        EXPECT_EQ(planet.population, 0.0);
        EXPECT_EQ(planet.infrastructure, 0.0);
        EXPECT_EQ(planet.owner, -1);
        EXPECT_EQ(planet.original_owner, -1);
    }

    {
        planet_t planet = {
            .system_id = 0,
            .mass_kg = 6e24,
            .orbit_au = 1
        };

        // forced planet type for test
        planet.planet_type = planet_type_t::rocky;
        bool const habitable =
            generation::detail::generate_planet(planet, system);
        // Make sure the testing-only forced-type logic works.
        EXPECT_EQ(planet.planet_type, planet_type_t::rocky);

        // These should be unchanged.
        EXPECT_EQ(planet.mass_kg, 6e24);
        EXPECT_EQ(planet.orbit_au, 1);

        EXPECT_GT(planet.radius_km, 0.0);
        EXPECT_GT(planet.orbital_period_y, 0.0);
        EXPECT_GT(planet.gravity_g, 0.0);
        EXPECT_GE(planet.axial_tilt_d, 0.0);
        EXPECT_LE(planet.axial_tilt_d, 90.0);
        EXPECT_GT(planet.day_h, 0.0);
        EXPECT_NEAR(planet.surface_temperature_k, earth_temperature_k, 10);
        EXPECT_GE(planet.magnetosphere_strength, 0.0);
        EXPECT_LT(planet.magnetosphere_strength, 5.0);
        EXPECT_GE(planet.atmopsheric_pressure, 0.0);
        EXPECT_GE(planet.o2_co2_suitability, 0.0);
        EXPECT_LE(planet.o2_co2_suitability, 1.0);
        EXPECT_GE(planet.ocean_coverage, 0.0);
        EXPECT_LE(planet.ocean_coverage, 1.0);
        EXPECT_GT(planet.atmosphere_type,
                  atmosphere_type_t::invalid_atmosphere_type);
        EXPECT_LE(planet.atmosphere_type, atmosphere_type_t::high_temperature);
        EXPECT_GE(planet.water, 0);
        EXPECT_LE(planet.water, 100);
        EXPECT_GE(planet.food, 0);
        EXPECT_LE(planet.food, 100);
        EXPECT_GE(planet.energy, 0);
        EXPECT_LE(planet.energy, 100);
        EXPECT_GE(planet.metal, 0);
        EXPECT_LE(planet.metal, 100);
        EXPECT_GE(planet.fuel, 0);
        EXPECT_LE(planet.fuel, 100);
        EXPECT_EQ(planet.population, 0.0);
        EXPECT_EQ(planet.infrastructure, 0.0);
        EXPECT_EQ(planet.owner, -1);
        EXPECT_EQ(planet.original_owner, -1);
    }

    {
        planet_t planet = {
            .system_id = 0,
            .mass_kg = 5.683e26,
            .orbit_au = 9.5f
        };

        // forced planet type for test
        planet.planet_type = planet_type_t::gas_giant;
        bool const habitable =
            generation::detail::generate_planet(planet, system);
        // Make sure the testing-only forced-type logic works.
        EXPECT_EQ(planet.planet_type, planet_type_t::gas_giant);

        // These should be unchanged.
        EXPECT_EQ(planet.mass_kg, 5.683e26);
        EXPECT_EQ(planet.orbit_au, 9.5f);

        EXPECT_GT(planet.radius_km, 0.0);
        EXPECT_GT(planet.orbital_period_y, 0.0);
        EXPECT_GT(planet.gravity_g, 0.0);
        EXPECT_GE(planet.axial_tilt_d, 0.0);
        EXPECT_LE(planet.axial_tilt_d, 90.0);
        EXPECT_GT(planet.day_h, 0.0);
        EXPECT_GT(planet.surface_temperature_k, 0.0);
        EXPECT_LT(planet.surface_temperature_k, 2000.0);
        EXPECT_GE(planet.magnetosphere_strength, 0.0);
        EXPECT_EQ(planet.atmopsheric_pressure, atmos_millions);
        EXPECT_GE(planet.o2_co2_suitability, 0.0);
        EXPECT_LE(planet.o2_co2_suitability, 1.0);
        EXPECT_EQ(planet.ocean_coverage, n_a);
        EXPECT_EQ(planet.atmosphere_type,
                  atmosphere_type_t::gas_giant_atmosphere);
        EXPECT_GE(planet.water, 0);
        EXPECT_LE(planet.water, 10);
        EXPECT_EQ(planet.food, 0);
        EXPECT_GE(planet.energy, 0);
        EXPECT_LE(planet.energy, 10);
        EXPECT_GE(planet.metal, 0);
        EXPECT_LE(planet.metal, 10);
        EXPECT_GE(planet.fuel, 0);
        EXPECT_LE(planet.fuel, 10);
        EXPECT_EQ(planet.population, 0.0);
        EXPECT_EQ(planet.infrastructure, 0.0);
        EXPECT_EQ(planet.owner, -1);
        EXPECT_EQ(planet.original_owner, -1);
    }

    {
        planet_t planet = {
            .system_id = 0,
            .mass_kg = 8.681e25,
            .orbit_au = 19.2f
        };

        // forced planet type for test
        planet.planet_type = planet_type_t::ice_giant;
        bool const habitable =
            generation::detail::generate_planet(planet, system);
        // Make sure the testing-only forced-type logic works.
        EXPECT_EQ(planet.planet_type, planet_type_t::ice_giant);

        // These should be unchanged.
        EXPECT_EQ(planet.mass_kg, 8.681e25);
        EXPECT_EQ(planet.orbit_au, 19.2f);

        EXPECT_GT(planet.radius_km, 0.0);
        EXPECT_GT(planet.orbital_period_y, 0.0);
        EXPECT_GT(planet.gravity_g, 0.0);
        EXPECT_GE(planet.axial_tilt_d, 0.0);
        EXPECT_LE(planet.axial_tilt_d, 90.0);
        EXPECT_GT(planet.day_h, 0.0);
        EXPECT_GT(planet.surface_temperature_k, 0.0);
        EXPECT_LT(planet.surface_temperature_k, 2000.0);
        EXPECT_GE(planet.magnetosphere_strength, 0.0);
        EXPECT_EQ(planet.atmopsheric_pressure, atmos_thousands);
        EXPECT_GE(planet.o2_co2_suitability, 0.0);
        EXPECT_LE(planet.o2_co2_suitability, 1.0);
        EXPECT_EQ(planet.ocean_coverage, n_a);
        EXPECT_EQ(planet.atmosphere_type,
                  atmosphere_type_t::ice_giant_atmosphere);
        EXPECT_GE(planet.water, 0);
        EXPECT_LE(planet.water, 10);
        EXPECT_EQ(planet.food, 0);
        EXPECT_GE(planet.energy, 0);
        EXPECT_LE(planet.energy, 10);
        EXPECT_GE(planet.metal, 0);
        EXPECT_LE(planet.metal, 10);
        EXPECT_GE(planet.fuel, 0);
        EXPECT_LE(planet.fuel, 10);
        EXPECT_EQ(planet.population, 0.0);
        EXPECT_EQ(planet.infrastructure, 0.0);
        EXPECT_EQ(planet.owner, -1);
        EXPECT_EQ(planet.original_owner, -1);
    }
}

TEST(generation_tests, generate_hex)
{
    game_start_params const params = {};
    int const first_hex_index = 3;
    int const last_hex_index = 10;
    int const habitable_systems_per_hex = 5;
    game_state_t game_state;
    auto const [map_radius, bulge_radius, center_hex, center_hex_pos] =
        generation::detail::galaxy_shape(params, game_state);

    generation::detail::scratch_space
        scratch(last_hex_index - first_hex_index);
    for (int i = first_hex_index; i < last_hex_index; ++i) {
        hex_t hex;
        generation::detail::generate_hex(
            hex, i, game_state, params, map_radius, bulge_radius, center_hex,
            center_hex_pos, habitable_systems_per_hex, scratch);

        EXPECT_LE((size_t)habitable_systems_per_hex,
                  hex.last_system - hex.first_system);
    }
    std::cout << std::format(
        "Generated {} habitable systems, {} systems total.\n",
        (last_hex_index - first_hex_index) * habitable_systems_per_hex,
        game_state.systems.size());
}

TEST(generation_tests, generate_galaxy)
{
    game_start_params params;
    game_state_t game_state;
    generation::generate_galaxy(params, game_state);
}
