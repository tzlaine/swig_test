#include <game_data.hpp>
#include <generate_galaxy.hpp>
#include <lua.hpp>
#include <task_system.hpp>

#include <gtest/gtest.h>


TEST(effects_tests, lua_bindings)
{
    {
        sol::optional<int> opt = lua().script("return invalid_hex_coord.x");
        EXPECT_TRUE(opt);
        EXPECT_EQ(*opt, hex_coord_t{}.x);
    }
    {
        sol::optional<hex_coord_t> opt =
            lua().script("return invalid_hex_coord");
        EXPECT_TRUE(opt);
        EXPECT_EQ(*opt, hex_coord_t{});
    }

    {
        sol::optional<int> opt =
            lua().script("return invalid_nation_and_object.nation_id");
        EXPECT_TRUE(opt);
        EXPECT_EQ(*opt, nation_and_object_id_t{}.nation_id);
    }
    {
        sol::optional<nation_and_object_id_t> opt =
            lua().script("return invalid_nation_and_object");
        EXPECT_TRUE(opt);
        EXPECT_EQ(*opt, nation_and_object_id_t{});
    }

    {
        sol::optional<planet_t> opt = lua().script("return planet_t.new()");
        EXPECT_TRUE(opt);
        EXPECT_EQ(*opt, planet_t{});
    }

    {
        sol::optional<planet_t> opt = lua().script("return planet_t.new()");
        EXPECT_TRUE(opt);
        EXPECT_EQ(*opt, planet_t{});
    }

    {
        sol::optional<atmosphere_type_t> opt =
            lua().script("return atmosphere_type_t.high_temperature");
        EXPECT_TRUE(opt);
        EXPECT_EQ(*opt, atmosphere_type_t::high_temperature);
    }

    {
        bool b = lua().script(
            "return atmosphere_type_t.high_temperature < "
            "atmosphere_type_t.reduced_type_a");
        EXPECT_FALSE(b);
    }

    {
        bool b = lua().script(
            "return atmosphere_type_t.high_temperature < "
            "atmosphere_type_t.gas_giant_atmosphere");
        EXPECT_TRUE(b);
    }

    {
        bool b = lua().script(
            "return atmosphere_type_t.high_temperature ~= "
            "atmosphere_type_t.gas_giant_atmosphere");
        EXPECT_TRUE(b);
    }
}

struct planet_effect_2
{
    adobe::name_t name;
    adobe::name_t reason;
    float value = -1.0f;
};

void apply_planet_effect(planet_t & planet, planet_effect_2 const & pe)
{
    sol::table effect = lua()["planet_effects"][pe.name.c_str()];
    sol::function apply = effect["apply"];
    sol::object expr = effect["expr"];
    if (expr)
        apply(effect, planet);
    else
        apply(effect, planet, pe.value);
}

TEST(effects_tests, planet_effects)
{
    script_file("effects.lua");
    sol::table effects = lua()["planet_effects"];

    planet_t const earth{
        .planet_type = planet_type_t::rocky,
        .mass_kg = earth_mass_kg,
        .radius_km = earth_radius_km,
        .orbit_au = 1,
        .orbital_period_y = 1,
        .gravity_g = 1,
        .axial_tilt_d = 23,
        .day_h = 24,
        .surface_temperature_k = (float)earth_temperature_k,
        .magnetosphere_strength = 1,
        .atmopsheric_pressure = 1,
        .o2_co2_suitability = 1,
        .ocean_coverage = (float)earth_ocean_coverage,
        .growth_factor = base_pop_growth_factor,
        .atmosphere_type = atmosphere_type_t::oxidized_type_b,
        .water = 100,
        .food = 100,
        .energy = 100,
        .metal = 100,
        .fuel = 100,
        .population = 100,
        .infrastructure = 10,
        .infrastructure_cost_factor = 1,
        .max_population = 100};

    using namespace adobe::literals;

    float const eps = 0.001;

    {
        planet_t earth_copy = earth;
        apply_planet_effect(
            earth_copy, planet_effect_2{"no_seasons"_name, ""_name});
        EXPECT_LT(
            std::abs(earth_copy.growth_factor - (earth.growth_factor + 0.05)),
            eps);
    }

    {
        planet_t earth_copy = earth;
        apply_planet_effect(
            earth_copy, planet_effect_2{"long_seasons"_name, ""_name});
        EXPECT_LT(
            std::abs(earth_copy.growth_factor - (earth.growth_factor - 0.05)),
            eps);
    }

    {
        planet_t earth_copy = earth;
        apply_planet_effect(
            earth_copy, planet_effect_2{"infra_cost"_name, ""_name, 2.0f});
        EXPECT_LT(
            earth_copy.infrastructure_cost_factor /
                    earth.infrastructure_cost_factor -
                2.0f,
            eps);
    }
}
