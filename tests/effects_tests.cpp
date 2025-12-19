#include <game_data.hpp>
#include <lua.hpp>

#include <gtest/gtest.h>


// TODO: Put this in its own test file?
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
}

TEST(effects_tests, TODO) {}
