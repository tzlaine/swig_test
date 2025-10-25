#include <game_data_formatters.hpp>

#include <gtest/gtest.h>

#include <strstream>


TEST(flags_tests, operations)
{
    {
        EXPECT_TRUE(flags(planet_effect_mod_t::cost));
        EXPECT_TRUE(flags(planet_effect_mod_t::monthly));
        EXPECT_FALSE(flags(planet_effect_mod_t::invalid_planet_effect_mod));
    }

    {
        EXPECT_EQ((unsigned int)(flags(planet_effect_mod_t::cost) |
            planet_effect_mod_t::monthly),
            3u);
        EXPECT_EQ(flags(planet_effect_mod_t::cost) |
            planet_effect_mod_t::monthly,
            flags(planet_effect_mod_t::cost) |
            flags(planet_effect_mod_t::monthly));

        flags initial_1(planet_effect_mod_t::cost);
        flags initial_2(planet_effect_mod_t::cost);

        initial_1 |= planet_effect_mod_t::monthly;
        initial_2 |= flags(planet_effect_mod_t::monthly);

        EXPECT_EQ((unsigned int)initial_1, 3u);
        EXPECT_EQ((unsigned int)initial_2, 3u);
        EXPECT_EQ(initial_1, initial_2);
    }

    {
        EXPECT_EQ((unsigned int)(flags(planet_effect_mod_t::cost) &
            planet_effect_mod_t::monthly),
            0u);
        EXPECT_EQ(flags(planet_effect_mod_t::cost) &
            planet_effect_mod_t::monthly,
            flags<planet_effect_mod_t>());

        flags const all = all_flags<planet_effect_mod_t>();
        flags initial_1 = all;
        flags initial_2 = all;

        initial_1 &= planet_effect_mod_t::monthly;
        initial_2 &= flags(planet_effect_mod_t::monthly);

        EXPECT_EQ((unsigned int)initial_1, 1u);
        EXPECT_EQ((unsigned int)initial_2, 1u);
        EXPECT_EQ(initial_1, initial_2);
    }

    {
        flags const all = all_flags<planet_effect_mod_t>();

        EXPECT_EQ(~all, flags(planet_effect_mod_t::invalid_planet_effect_mod));
        EXPECT_EQ(~flags(planet_effect_mod_t::monthly), flags(planet_effect_mod_t::cost));
        EXPECT_EQ(~flags(planet_effect_mod_t::cost), flags(planet_effect_mod_t::monthly));
        EXPECT_EQ(~flags<planet_effect_mod_t>(), all);
    }

    // generated
    {
        EXPECT_EQ((unsigned int)all_flags<planet_effect_mod_t>(), 3u);
        EXPECT_EQ(
            all_flags<planet_effect_mod_t>(),
            flags(planet_effect_mod_t::monthly) | planet_effect_mod_t::cost);
        EXPECT_EQ(
            planet_effect_mod_t::monthly | planet_effect_mod_t::cost,
            flags(planet_effect_mod_t::monthly) | planet_effect_mod_t::cost);
        EXPECT_EQ(~planet_effect_mod_t::invalid_planet_effect_mod,
                  all_flags<planet_effect_mod_t>());
        EXPECT_EQ((unsigned int)~planet_effect_mod_t::monthly, 2u);
        EXPECT_EQ((unsigned int)~planet_effect_mod_t::cost, 1u);
    }
}

TEST(flags_tests, printing)
{
    EXPECT_EQ(std::format("{}", all_flags<planet_effect_mod_t>()),
              "cost|monthly");
    EXPECT_EQ(std::format("{}", flags(planet_effect_mod_t::cost)), "cost");
    EXPECT_EQ(std::format("{}", flags(planet_effect_mod_t::monthly)),
              "monthly");
}
