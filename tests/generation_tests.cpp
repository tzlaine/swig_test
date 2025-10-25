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
    {
        // TODO
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

    // Requiring hab+suits after requiring hab+masks removes the hab+masks
    // requirement.
    {
        planet_t planet = earth;
        planet.o2_co2_suitability = 0.45f;     // Requires habs+masks; comes first
        planet.magnetosphere_strength = 0.25f; // Requires habs+suits
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.12 + habs_and_suits_growth_modifier, 0.005);
        EXPECT_EQ(planet.effects.size(), 4u);
        truncate(planet.effects[0].amount, 2);
        truncate(planet.effects[1].amount, 2);
        truncate(planet.effects[2].amount, 2);
        truncate(planet.effects[3].amount, 2);
        planet_effect_t const expected[] = {
            {
                .name="very_poor_o2_co2_suitab"_name,
                .description="very_poor_o2_co2_suitab_desc"_name,
                .amount=-0.12,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
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
