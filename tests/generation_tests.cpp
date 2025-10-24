#include <game_data_t.hpp>
#include <json2pb.h>
#include <generate_galaxy.hpp>

#include <game_data_formatters.hpp>

#include <gtest/gtest.h>

#include <format>
#include <strstream>


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

template<typename T>
void dump(T const & x)
{
    std::cout << std::format("{}", x) << "\n";
}

TEST(generation_tests, growth_factor_and_effects)
{
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

    dump(earth); // TODO

    {
        // Earth had better have the base habitability, and no effects.
        planet_t planet = earth;
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);
        EXPECT_EQ(result, base_pop_growth_factor);
        EXPECT_EQ(planet.effects.size(), 0u);
    }
    {
        planet_t planet = earth;
        // TODO: Change something.
        double const result =
            generation::detail::determine_growth_factor_and_effects(planet);

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
}
