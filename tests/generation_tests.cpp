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

double truncate(double x, int decimal_places)
{
    if (decimal_places < 0)
        return x;
    double const factor = std::pow(10.0, decimal_places);
    return std::trunc(x * factor) / factor;
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
        planet.effects[0].amount = truncate(planet.effects[0].amount, 2);
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
        planet.effects[0].amount = truncate(planet.effects[0].amount, 1);
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
        // TODO
    }

    // day length
    {
        planet_t planet = earth;
        planet.day_h = 12.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor -0.2, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        //planet.effects[0].amount = truncate(planet.effects[0].amount, 1);
        planet_effect_t const expected{
            .name="short_days"_name,
            .description="short_days_desc"_name,
            .amount=-0.2,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.day_h = 29.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.05, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet.effects[0].amount = truncate(planet.effects[0].amount, 2);
        planet_effect_t const expected{
            .name="long_days"_name,
            .description="long_days_desc"_name,
            .amount=-0.05,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.day_h = 36.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.1, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet.effects[0].amount = truncate(planet.effects[0].amount, 1);
        planet_effect_t const expected{
            .name="long_days"_name,
            .description="long_days_desc"_name,
            .amount=-0.1,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.day_h = 53.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.15, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet.effects[0].amount = truncate(planet.effects[0].amount, 2);
        planet_effect_t const expected{
            .name="very_long_days"_name,
            .description="very_long_days_desc"_name,
            .amount=-0.15,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
    }
    {
        planet_t planet = earth;
        planet.day_h = 99.0f;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_NEAR(result, base_pop_growth_factor - 0.2, eps);
        EXPECT_EQ(planet.effects.size(), 1u);
        planet.effects[0].amount = truncate(planet.effects[0].amount, 1);
        planet_effect_t const expected{
            .name="very_long_days"_name,
            .description="very_long_days_desc"_name,
            .amount=-0.2,
            .target=planet_effect_target_t::growth_factor,
            .operation=effect_op_t::add};
        EXPECT_EQ(planet.effects[0], expected);
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
        planet.effects[0].amount = truncate(planet.effects[0].amount, 3);
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
        planet.effects[0].amount = truncate(planet.effects[0].amount, 2);
        planet.effects[1].amount = truncate(planet.effects[1].amount, 2);
        planet.effects[2].amount = truncate(planet.effects[2].amount, 2);
        planet_effect_t const expected[3] = {
            {
                .name="very_poor_o2_co2_suitab"_name,
                .description="very_poor_o2_co2_suitab_desc"_name,
                .amount=-0.12,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="very_poor_o2_co2_suitab_habs_and_masks_pop_effect"_name,
                .description="very_poor_o2_co2_suitab_habs_and_masks_pop_effect_desc"_name,
                .amount=0.25,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
            {
                .name="very_poor_o2_co2_suitab_habs_and_masks_infra_cost_effect"_name,
                .description="very_poor_o2_co2_suitab_habs_and_masks_infra_cost_effect_desc"_name,
                .amount=2,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=2u,
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
        planet.effects[0].amount = truncate(planet.effects[0].amount, 2);
        planet.effects[1].amount = truncate(planet.effects[1].amount, 2);
        planet.effects[2].amount = truncate(planet.effects[2].amount, 2);
        planet_effect_t const expected[3] = {
            {
                .name="marginal_o2_co2_suitab"_name,
                .description="marginal_o2_co2_suitab_desc"_name,
                .amount=-0.14,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="marginal_o2_co2_suitab_habs_and_masks_pop_effect"_name,
                .description="marginal_o2_co2_suitab_habs_and_masks_pop_effect_desc"_name,
                .amount=habs_and_masks_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
            {
                .name="marginal_o2_co2_suitab_habs_and_masks_infra_cost_effect"_name,
                .description="marginal_o2_co2_suitab_habs_and_masks_infra_cost_effect_desc"_name,
                .amount=habs_and_masks_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=2u,
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
        planet.effects[0].amount = truncate(planet.effects[0].amount, 2);
        planet.effects[1].amount = truncate(planet.effects[1].amount, 2);
        planet.effects[2].amount = truncate(planet.effects[2].amount, 2);
        planet_effect_t const expected[3] = {
            {
                .name="insufficient_o2_co2_suitab"_name,
                .description="insufficient_o2_co2_suitab_desc"_name,
                .amount=-0.2,
                .target=planet_effect_target_t::growth_factor,
                .operation=effect_op_t::add
            },
            {
                .name="insufficient_o2_co2_suitab_habs_and_suits_pop_effect"_name,
                .description="insufficient_o2_co2_suitab_habs_and_suits_pop_effect_desc"_name,
                .amount=habs_and_suits_habitable_factor,
                .target=planet_effect_target_t::max_population,
                .operation=effect_op_t::multiply
            },
            {
                .name="insufficient_o2_co2_suitab_habs_and_suits_infra_cost_effect"_name,
                .description="insufficient_o2_co2_suitab_habs_and_suits_infra_cost_effect_desc"_name,
                .amount=habs_and_suits_infra_cost_factor,
                .target=planet_effect_target_t::infrastructure,
                .target_modifiers=2u,
                .operation=effect_op_t::multiply
            },
        };
        EXPECT_EQ(planet.effects[0], expected[0]);
        EXPECT_EQ(planet.effects[1], expected[1]);
        EXPECT_EQ(planet.effects[2], expected[2]);
    }

    // atmospheric pressure
    {
        // TODO
    }

    // magnetosphere
    {
        // TODO
    }

    // temperature
    {
        // TODO
    }

    // Duplicate of habs_and_* effects
    {
        // TODO
    }

    if (0) {
        planet_t planet = earth;
        // TODO: Change something.
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);

        dump(planet);
        // TODO
        std::cout << "result=" << result << "\n"
                  << "# effect=" << planet.effects.size() << "\n";
        for (auto const & e : planet.effects) {
            dump(e);
            std::cout << "\n";
        }

        // TODO: Check value of result.
        // TODO: Check effects.
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
