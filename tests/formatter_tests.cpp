#include <game_data_formatters.hpp>

#include <gtest/gtest.h>

#include <strstream>


TEST(formatter_tests, user_and_dump)
{
    {
        auto const for_user = std::format("{:u}", planet_type_t::rocky);
        auto const for_debug = std::format("{}", planet_type_t::rocky);

        EXPECT_EQ(for_user, "rocky");
        EXPECT_EQ(for_debug, "rocky");
    }
    {
        auto const for_user = std::format("{:u}", planet_type_t::gas_giant);
        auto const for_debug = std::format("{}", planet_type_t::gas_giant);

        EXPECT_EQ(for_user, "gas giant");
        EXPECT_EQ(for_debug, "gas_giant");
    }
    {
        auto const for_user = std::format("{:u}", planet_type_t::ice_giant);
        auto const for_debug = std::format("{}", planet_type_t::ice_giant);

        EXPECT_EQ(for_user, "ice giant");
        EXPECT_EQ(for_debug, "ice_giant");
    }
}
