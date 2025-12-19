#include <game_data.hpp>
#include <lua.hpp>

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

TEST(effects_tests, TODO)
{
    // TODO
}
