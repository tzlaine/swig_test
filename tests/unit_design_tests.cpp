#include <unit_design.hpp>
#include <game_data_metadata.hpp>

#include <gtest/gtest.h>


using namespace adobe::literals;

template<typename T>
struct scoped_reset
{
    scoped_reset(T & x) : prev_(x), curr_(x) {}
    ~scoped_reset() { curr_ = prev_; }

private:
    T prev_;
    T & curr_;
};

inline float eps = 0.000001f; // tolerance for testing float closeness.

TEST(unit_design_tests, hull_novelty)
{
    unit_design_t design_100 = {
        .hull = 100,
        .armor = 10,
        .propulsion = 2,
        .weapons = 2,
        .shields = 2,
        .detection = 2,
        .stealth = 2,
        .fuel = 2,
        .water = 2,
        .supplies = 2,
        .rounds = 2,
        .missiles = 2,
        .fighters = 2,
        .cargo = 2,
        .propulsion_space = 20,
        .weapons_space = 20,
        .shields_space = 20,
        .detection_space = 20,
        .stealth_space = 20,
        .effective_propulsion = 2,
        .effective_weapons = 2,
        .effective_shields = 2,
        .effective_detection = 2,
        .effective_stealth = 2,
        .propulsion_reliability = 100,
        .weapons_reliability = 100,
        .shields_reliability = 100,
        .detection_reliability = 100,
        .stealth_reliability = 100,
        .known_propulsion_failures = 0,
        .known_weapons_failures = 0,
        .known_shields_failures = 0,
        .known_detection_failures = 0,
        .known_stealth_failures = 0,
        .design_months_remaining = 0};

    unit_design_t design_200 = design_100;
    design_200.hull = 200;

    nation_t nation = {
        .unit_designs = {},
        .construction_tech = 2,
        .propulsion_tech = 2,
        .weapons_tech = 2,
        .shields_tech = 2,
        .stealth_tech = 2,
        .detection_tech = 2,
        .automation_tech = 2,
        .construction_tradition = 30,
        .propulsion_tradition = 30,
        .weapons_tradition = 30,
        .shields_tradition = 30,
        .stealth_tradition = 30,
        .detection_tradition = 30};

    {
        // reusing a previous hull design is not novel
        scoped_reset _1(design_100);
        scoped_reset _2(design_200);
        scoped_reset _3(nation);
        nation.unit_designs = {design_100};
        reasons rs;

        unit_design_t new_design = {
            .hull = 100,
            .armor = 10,
            .propulsion = 2,
            .weapons = 2,
            .shields = 2,
            .detection = 2,
            .stealth = 2,
            .fuel = 2,
            .water = 2,
            .supplies = 2,
            .rounds = 2,
            .missiles = 2,
            .fighters = 2,
            .cargo = 2};

        float const result = hull_size_novelty(new_design, nation, &rs);
        EXPECT_NEAR(result, 0.0f, eps);
        EXPECT_EQ(rs.size(), 0u);
    }

    {
        // reusing a previous max-size hull design is not novel
        scoped_reset _1(design_100);
        scoped_reset _2(design_200);
        scoped_reset _3(nation);
        nation.unit_designs = {design_200};
        reasons rs;

        unit_design_t new_design = {
            .hull = 200,
            .armor = 10,
            .propulsion = 2,
            .weapons = 2,
            .shields = 2,
            .detection = 2,
            .stealth = 2,
            .fuel = 2,
            .water = 2,
            .supplies = 2,
            .rounds = 2,
            .missiles = 2,
            .fighters = 2,
            .cargo = 2};

        float const result = hull_size_novelty(new_design, nation, &rs);
        EXPECT_NEAR(result, 0.0f, eps);
        EXPECT_EQ(rs.size(), 0u);
    }

    {
        scoped_reset _1(design_100);
        scoped_reset _2(design_200);
        scoped_reset _3(nation);
        nation.unit_designs = {design_100};
        reasons rs;

        unit_design_t new_design = {
            .hull = 200,
            .armor = 10,
            .propulsion = 2,
            .weapons = 2,
            .shields = 2,
            .detection = 2,
            .stealth = 2,
            .fuel = 2,
            .water = 2,
            .supplies = 2,
            .rounds = 2,
            .missiles = 2,
            .fighters = 2,
            .cargo = 2};

        float const result = hull_size_novelty(new_design, nation, &rs);
        EXPECT_NEAR(result, 2.0f, eps);
        EXPECT_EQ(rs.size(), 2u);
        EXPECT_EQ(rs[0].description_format(), "inexperience_with_hull_size");
        EXPECT_EQ(rs[1].description_format(), "near_max_hull_size");
    }

    {
        // in-progress designs are ignored
        scoped_reset _1(design_100);
        scoped_reset _2(design_200);
        scoped_reset _3(nation);
        design_100.design_months_remaining = 10;
        nation.unit_designs = {design_200};
        reasons rs;

        unit_design_t new_design = {
            .hull = 100,
            .armor = 10,
            .propulsion = 2,
            .weapons = 2,
            .shields = 2,
            .detection = 2,
            .stealth = 2,
            .fuel = 2,
            .water = 2,
            .supplies = 2,
            .rounds = 2,
            .missiles = 2,
            .fighters = 2,
            .cargo = 2};

        float const result = hull_size_novelty(new_design, nation, &rs);
        EXPECT_NEAR(result, 0.5f, eps);
        EXPECT_EQ(rs.size(), 1u);
        EXPECT_EQ(rs[0].description_format(), "inexperience_with_hull_size");
    }
}

TEST(unit_design_tests, equipment_novelty)
{
    unit_design_t design_100 = {
        .hull = 100,
        .armor = 10,
        .propulsion = 1,
        .weapons = 1,
        .shields = 1,
        .detection = 1,
        .stealth = 1,
        .fuel = 1,
        .water = 1,
        .supplies = 1,
        .rounds = 1,
        .missiles = 1,
        .fighters = 1,
        .cargo = 1,
        .propulsion_space = 10,
        .weapons_space = 10,
        .shields_space = 10,
        .detection_space = 10,
        .stealth_space = 10,
        .effective_propulsion = 1,
        .effective_weapons = 1,
        .effective_shields = 1,
        .effective_detection = 1,
        .effective_stealth = 1,
        .propulsion_reliability = 100,
        .weapons_reliability = 100,
        .shields_reliability = 100,
        .detection_reliability = 100,
        .stealth_reliability = 100,
        .known_propulsion_failures = 0,
        .known_weapons_failures = 0,
        .known_shields_failures = 0,
        .known_detection_failures = 0,
        .known_stealth_failures = 0,
        .design_months_remaining = 0};

    nation_t nation = {
        .unit_designs = {},
        .construction_tech = 2,
        .propulsion_tech = 2,
        .weapons_tech = 2,
        .shields_tech = 2,
        .stealth_tech = 2,
        .detection_tech = 2,
        .automation_tech = 2,
        .construction_tradition = 30,
        .propulsion_tradition = 30,
        .weapons_tradition = 30,
        .shields_tradition = 30,
        .stealth_tradition = 30,
        .detection_tradition = 30};

    {
        scoped_reset _1(design_100);
        scoped_reset _2(nation);
        std::vector<float> iteration_design_novelty =
            lua().get<std::vector<float>>("iteration_design_novelty");
        for (float novelty_factor : iteration_design_novelty) {
            reasons rs;
            unit_design_t new_design = design_100;
            new_design.propulsion = 1;

            float const result = equipment_novelty(
                new_design,
                nation,
                detail::metadata<unit_design_t>::propulsion(),
                &rs);
            EXPECT_NEAR(result, novelty_factor, eps);
            EXPECT_EQ(rs.size(), 1u);
            EXPECT_EQ(
                rs[0].description_format(),
                "inexperience_with_propulsion_level");

            nation.unit_designs.push_back(design_100);
        }
        {
            reasons rs;
            unit_design_t new_design = design_100;
            new_design.propulsion = 1;

            float const result = equipment_novelty(
                new_design,
                nation,
                detail::metadata<unit_design_t>::propulsion(),
                &rs);
            EXPECT_NEAR(result, 0.0f, eps);
            EXPECT_EQ(rs.size(), 0u);
        }
    }
    {
        // other tech ratings do not affect this one
        scoped_reset _1(design_100);
        scoped_reset _2(nation);
        std::vector<float> iteration_design_novelty =
            lua().get<std::vector<float>>("iteration_design_novelty");
        for (float novelty_factor : iteration_design_novelty) {
            reasons rs;
            unit_design_t new_design = design_100;
            new_design.propulsion = 1;
            new_design.shields = 8;

            float const result = equipment_novelty(
                new_design,
                nation,
                detail::metadata<unit_design_t>::propulsion(),
                &rs);
            EXPECT_NEAR(result, novelty_factor, eps);
            EXPECT_EQ(rs.size(), 1u);
            EXPECT_EQ(
                rs[0].description_format(),
                "inexperience_with_propulsion_level");

            nation.unit_designs.push_back(design_100);
        }
        {
            reasons rs;
            unit_design_t new_design = design_100;
            new_design.propulsion = 1;
            new_design.shields = 8;

            float const result = equipment_novelty(
                new_design,
                nation,
                detail::metadata<unit_design_t>::propulsion(),
                &rs);
            EXPECT_NEAR(result, 0.0f, eps);
            EXPECT_EQ(rs.size(), 0u);
        }
    }
}

TEST(unit_design_tests, full_design_novelty)
{
    unit_design_t design_100 = {
        .hull = 100,
        .armor = 10,
        .propulsion = 1,
        .weapons = 1,
        .shields = 1,
        .detection = 1,
        .stealth = 1,
        .fuel = 1,
        .water = 1,
        .supplies = 1,
        .rounds = 1,
        .missiles = 1,
        .fighters = 1,
        .cargo = 1,
        .propulsion_space = 10,
        .weapons_space = 10,
        .shields_space = 10,
        .detection_space = 10,
        .stealth_space = 10,
        .effective_propulsion = 1,
        .effective_weapons = 1,
        .effective_shields = 1,
        .effective_detection = 1,
        .effective_stealth = 1,
        .propulsion_reliability = 100,
        .weapons_reliability = 100,
        .shields_reliability = 100,
        .detection_reliability = 100,
        .stealth_reliability = 100,
        .known_propulsion_failures = 0,
        .known_weapons_failures = 0,
        .known_shields_failures = 0,
        .known_detection_failures = 0,
        .known_stealth_failures = 0,
        .design_months_remaining = 0};

    nation_t nation = {
        .unit_designs = {},
        .construction_tech = 2,
        .propulsion_tech = 2,
        .weapons_tech = 2,
        .shields_tech = 2,
        .stealth_tech = 2,
        .detection_tech = 2,
        .automation_tech = 2,
        .construction_tradition = 30,
        .propulsion_tradition = 30,
        .weapons_tradition = 30,
        .shields_tradition = 30,
        .stealth_tradition = 30,
        .detection_tradition = 30};

    {
        scoped_reset _1(design_100);
        scoped_reset _2(nation);
        nation.unit_designs = {design_100};
        reasons rs;

        unit_design_t new_design = design_100;
        new_design.hull = 200;
        new_design.propulsion = 2;
        new_design.weapons = 3;
        new_design.shields = 4;

        float const result = design_novelty(new_design, nation, &rs);
        EXPECT_NEAR(result, 6.0f, eps);
        EXPECT_EQ(rs.size(), 7u);
        EXPECT_EQ(rs[0].description_format(), "inexperience_with_hull_size");
        EXPECT_EQ(rs[1].description_format(), "near_max_hull_size");
        EXPECT_EQ(
            rs[2].description_format(), "inexperience_with_propulsion_level");
        EXPECT_EQ(
            rs[3].description_format(), "inexperience_with_weapons_level");
        EXPECT_EQ(
            rs[4].description_format(), "inexperience_with_shields_level");
        EXPECT_EQ(
            rs[5].description_format(), "inexperience_with_detection_level");
        EXPECT_EQ(
            rs[6].description_format(), "inexperience_with_stealth_level");
    }
}

TEST(unit_design_tests, roll_design_before_final_month)
{
    unit_design_t design_100 = {
        .hull = 100,
        .armor = 10,
        .propulsion = 1,
        .weapons = 1,
        .shields = 1,
        .detection = 1,
        .stealth = 1,
        .fuel = 1,
        .water = 1,
        .supplies = 1,
        .rounds = 1,
        .missiles = 1,
        .fighters = 1,
        .cargo = 1,
        .propulsion_space = 10,
        .weapons_space = 10,
        .shields_space = 10,
        .detection_space = 10,
        .stealth_space = 10,
        .effective_propulsion = 1,
        .effective_weapons = 1,
        .effective_shields = 1,
        .effective_detection = 1,
        .effective_stealth = 1,
        .propulsion_reliability = 100,
        .weapons_reliability = 100,
        .shields_reliability = 100,
        .detection_reliability = 100,
        .stealth_reliability = 100,
        .known_propulsion_failures = 0,
        .known_weapons_failures = 0,
        .known_shields_failures = 0,
        .known_detection_failures = 0,
        .known_stealth_failures = 0,
        .design_months_remaining = 0};

    nation_t nation = {
        .unit_designs = {},
        .construction_tech = 2,
        .propulsion_tech = 2,
        .weapons_tech = 2,
        .shields_tech = 2,
        .stealth_tech = 2,
        .detection_tech = 2,
        .automation_tech = 2,
        .construction_tradition = 30,
        .propulsion_tradition = 30,
        .weapons_tradition = 30,
        .shields_tradition = 30,
        .stealth_tradition = 30,
        .detection_tradition = 30};

    {
        scoped_reset _1(design_100);
        scoped_reset _2(nation);
        nation.unit_designs = {design_100};
        reasons rs;

        unit_design_t new_design = design_100;
        new_design.hull = 100;
        new_design.propulsion = 2;
        new_design.design_months_remaining = 3;

        roll_design_month(new_design, nation);
        EXPECT_EQ(new_design.hit_table.size(), 6u);
        EXPECT_TRUE(
            std::ranges::all_of(new_design.hit_table, [](signed int roll) {
                return 0 <= roll && roll <= 100;
            }));
        roll_design_month(new_design, nation);
        EXPECT_EQ(new_design.hit_table.size(), 12u);
        EXPECT_TRUE(
            std::ranges::all_of(new_design.hit_table, [](signed int roll) {
                return 0 <= roll && roll <= 100;
            }));
    }
    {
        scoped_reset _1(design_100);
        scoped_reset _2(nation);
        nation.unit_designs = {design_100};
        nation.construction_tradition = 100;
        nation.propulsion_tradition = 100;
        nation.weapons_tradition = 100;
        nation.shields_tradition = 100;
        nation.stealth_tradition = 100;
        nation.detection_tradition = 100;
        reasons rs;

        unit_design_t new_design = design_100;
        new_design.hull = 100;
        new_design.propulsion = 2;
        new_design.design_months_remaining = 3;

        roll_design_month(new_design, nation);
        EXPECT_EQ(new_design.hit_table.size(), 6u);
        EXPECT_TRUE(
            std::ranges::all_of(new_design.hit_table, [](signed char roll) {
                return roll == 100;
            }));
        roll_design_month(new_design, nation);
        EXPECT_EQ(new_design.hit_table.size(), 12u);
        EXPECT_TRUE(
            std::ranges::all_of(new_design.hit_table, [](signed char roll) {
                return roll == 100;
            }));
    }
}

TEST(unit_design_tests, rolls_detail)
{
    unit_design_t design = {
        .hit_table = std::vector<signed char>(
            {0, 50, 0, 50, 0, 50, 100, 100, 100, 100, 100, 100})};
    std::array<double, 6> rolls = {{0}};
    detail::reconstruct_mean_rolls(design, rolls);
    EXPECT_NEAR(rolls[0], 0.50, eps);
    EXPECT_NEAR(rolls[1], 0.75, eps);
    EXPECT_NEAR(rolls[2], 0.50, eps);
    EXPECT_NEAR(rolls[3], 0.75, eps);
    EXPECT_NEAR(rolls[4], 0.50, eps);
    EXPECT_NEAR(rolls[5], 0.75, eps);

    auto const prev_rolls = rolls;
    auto const effectiveness_rolls =
        detail::space_and_effectiveness_from_rolls(rolls);

    for (int i = 0, last = (int)rolls.size(); i < last; ++i) {
        auto const prev_roll_badness = 1.0 - prev_rolls[0];
        auto const space_badness = 1.0 - rolls[0];
        auto const effectiveness_badness = 1.0 - effectiveness_rolls[0];
        EXPECT_NEAR(
            space_badness + effectiveness_badness, prev_roll_badness, eps);
    }
}

TEST(unit_design_tests, detail_too_little_space)
{
    unit_design_t design = {
        .hull = 60,
        .propulsion = 1,
        .weapons = 1,
        .shields = 1,
        .detection = 1,
        .stealth = 1,
        .fuel = 1,
        .water = 1,
        .supplies = 1,
        .rounds = 1,
        .missiles = 1,
        .fighters = 1,
        .cargo = 1,
        .propulsion_space = 10,
        .weapons_space = 10,
        .shields_space = 10,
        .detection_space = 10,
        .stealth_space = 10,
        .design_months_remaining = 0};

    int const base_design_unused_space =
        call_lua_func("unit_unused_space", design);
    EXPECT_EQ(base_design_unused_space, 0);

    { // free space is 1
        scoped_reset _(design);
        design.propulsion_space -= 1;
        auto const prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_EQ(design, prev_design);
    }

    { // free space is 0
        scoped_reset _(design);
        auto const prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_EQ(design, prev_design);
    }

    {
        scoped_reset _(design);
        design.propulsion_space += 1;
        auto prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_NE(design, prev_design);
        prev_design.cargo = 0;
        EXPECT_EQ(design, prev_design);
    }

    {
        scoped_reset _(design);
        design.propulsion_space += 2;
        auto prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_NE(design, prev_design);
        prev_design.cargo = 0;
        prev_design.fighters = 0;
        EXPECT_EQ(design, prev_design);
    }

    {
        scoped_reset _(design);
        design.propulsion_space += 3;
        auto prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_NE(design, prev_design);
        prev_design.cargo = 0;
        prev_design.fighters = 0;
        prev_design.missiles = 0;
        EXPECT_EQ(design, prev_design);
    }

    {
        scoped_reset _(design);
        design.propulsion_space += 4;
        auto prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_NE(design, prev_design);
        prev_design.cargo = 0;
        prev_design.fighters = 0;
        prev_design.missiles = 0;
        prev_design.rounds = 0;
        EXPECT_EQ(design, prev_design);
    }

    {
        scoped_reset _(design);
        design.propulsion_space += 5;
        auto prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_NE(design, prev_design);
        prev_design.cargo = 0;
        prev_design.fighters = 0;
        prev_design.missiles = 0;
        prev_design.rounds = 0;
        prev_design.supplies = 0;
        prev_design.failed_design = true;
        EXPECT_EQ(design, prev_design);
    }

    {
        scoped_reset _(design);
        design.cargo = 0;
        design.fighters = 0;
        design.missiles = 0;
        design.rounds = 0;
        design.propulsion_space += 6;
        auto prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_NE(design, prev_design);
        prev_design.supplies = 0;
        prev_design.failed_design = true;
        EXPECT_EQ(design, prev_design);
    }

    {
        scoped_reset _(design);
        design.cargo = 0;
        design.fighters = 0;
        design.missiles = 0;
        design.rounds = 0;
        design.supplies = 2;
        design.propulsion_space += 6;
        auto prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_NE(design, prev_design);
        prev_design.supplies = 1;
        prev_design.water = 0;
        prev_design.failed_design = true;
        EXPECT_EQ(design, prev_design);
    }

    {
        scoped_reset _(design);
        design.cargo = 0;
        design.fighters = 0;
        design.missiles = 0;
        design.rounds = 0;
        design.supplies = 2;
        design.water = 2;
        design.propulsion_space += 6;
        auto prev_design = design;
        detail::handle_too_little_space(design);
        EXPECT_NE(design, prev_design);
        prev_design.supplies = 1;
        prev_design.water = 1;
        prev_design.fuel = 0;
        prev_design.failed_design = true;
        EXPECT_EQ(design, prev_design);
    }
}

TEST(unit_design_tests, detail_generate_hit_table_quality_a)
{
    unit_design_t design = {
        .hull = 60,
        .propulsion = 1,
        .weapons = 1,
        .shields = 1,
        .detection = 1,
        .stealth = 1,
        .fuel = 1,
        .water = 1,
        .supplies = 1,
        .rounds = 1,
        .missiles = 1,
        .fighters = 1,
        .cargo = 1,
        .propulsion_space = 11,
        .weapons_space = 10,
        .shields_space = 10,
        .detection_space = 10,
        .stealth_space = 10,
        .design_months_remaining = 0};

    { // no unused space, no cargo
        scoped_reset _(design);
        design.cargo -= 1;
        detail::generate_hit_table(design, 1.0);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 0);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(
            it, design.cargo, (signed char)hit_table_entry_t::hit_cargo);
        it = std::fill_n(
            it, design.fuel, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        EXPECT_EQ(design.hit_table, expected);
    }

    { // no unused space, one cargo
        scoped_reset _(design);
        design.propulsion -= 1;
        detail::generate_hit_table(design, 1.0);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 0);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(
            it, design.cargo, (signed char)hit_table_entry_t::hit_cargo);
        it = std::fill_n(
            it, design.fuel, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        EXPECT_EQ(design.hit_table, expected);
    }

    { // one unused space, one cargo
        scoped_reset _(design);
        design.propulsion_space -= 2;
        detail::generate_hit_table(design, 1.0);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 1);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.fuel, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_cargo);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        EXPECT_EQ(design.hit_table, expected);
    }

    { // two unused space, one cargo
        scoped_reset _(design);
        design.propulsion_space -= 3;
        detail::generate_hit_table(design, 1.0);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 2);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.fuel, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_cargo);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        EXPECT_EQ(design.hit_table, expected);
    }

    { // three unused space, one cargo
        scoped_reset _(design);
        design.propulsion_space -= 4;
        detail::generate_hit_table(design, 1.0);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 3);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.fuel, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        expected.back() = (signed char)hit_table_entry_t::hit_cargo;
        EXPECT_EQ(design.hit_table, expected);
    }

    { // four unused space, one cargo
        scoped_reset _(design);
        design.propulsion_space -= 5;
        detail::generate_hit_table(design, 1.0);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 4);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.fuel, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_cargo);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        EXPECT_EQ(design.hit_table, expected);
    }

    { // four unused space, one cargo, two fuel
        scoped_reset _(design);
        design.fuel += 1;
        design.propulsion_space -= 6;
        detail::generate_hit_table(design, 1.0);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 4);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
                it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_cargo);
        EXPECT_EQ(design.hit_table, expected);
    }
}

TEST(unit_design_tests, detail_generate_hit_table_quality_b)
{
    unit_design_t design = {
        .hull = 60,
        .propulsion = 1,
        .weapons = 1,
        .shields = 1,
        .detection = 1,
        .stealth = 1,
        .fuel = 1,
        .water = 1,
        .supplies = 1,
        .rounds = 1,
        .missiles = 1,
        .fighters = 1,
        .cargo = 1,
        .propulsion_space = 11,
        .weapons_space = 10,
        .shields_space = 10,
        .detection_space = 10,
        .stealth_space = 10,
        .design_months_remaining = 0};

    { // four unused space, one cargo, two fuel
        scoped_reset _(design);
        design.fuel += 1;
        design.propulsion_space -= 6;
        detail::generate_hit_table(design, 0.85);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 4);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_cargo);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
                it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        EXPECT_EQ(design.hit_table, expected);
    }
}

TEST(unit_design_tests, detail_generate_hit_table_quality_c)
{
    unit_design_t design = {
        .hull = 60,
        .propulsion = 1,
        .weapons = 1,
        .shields = 1,
        .detection = 1,
        .stealth = 1,
        .fuel = 1,
        .water = 1,
        .supplies = 1,
        .rounds = 1,
        .missiles = 1,
        .fighters = 1,
        .cargo = 1,
        .propulsion_space = 11,
        .weapons_space = 10,
        .shields_space = 10,
        .detection_space = 10,
        .stealth_space = 10,
        .design_months_remaining = 0};

    { // four unused space, one cargo, two fuel
        scoped_reset _(design);
        design.fuel += 1;
        design.propulsion_space -= 6;
        detail::generate_hit_table(design, 0.75);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 4);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_cargo);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        EXPECT_EQ(design.hit_table, expected);
    }
}

TEST(unit_design_tests, detail_generate_hit_table_quality_f)
{
    unit_design_t design = {
        .hull = 60,
        .propulsion = 1,
        .weapons = 1,
        .shields = 1,
        .detection = 1,
        .stealth = 1,
        .fuel = 1,
        .water = 1,
        .supplies = 1,
        .rounds = 1,
        .missiles = 1,
        .fighters = 1,
        .cargo = 1,
        .propulsion_space = 11,
        .weapons_space = 10,
        .shields_space = 10,
        .detection_space = 10,
        .stealth_space = 10,
        .design_months_remaining = 0};

    { // four unused space, one cargo, two fuel
        scoped_reset _(design);
        design.fuel += 1;
        design.propulsion_space -= 6;
        detail::generate_hit_table(design, 0.0);

        double const crew = call_lua_func("unit_crew", design);
        int const crew_space = call_lua_func("unit_crew_space", crew);
        int const design_unused_space =
            call_lua_func("unit_unused_space", design);
        EXPECT_EQ(design_unused_space, 4);

        std::vector<signed char> expected;
        expected.resize(design.hull, 0);
        auto it = expected.begin();
        it = std::fill_n(
            it, crew_space, (signed char)hit_table_entry_t::hit_crew_space);
        it = std::fill_n(
            it,
            design.propulsion_space,
            (signed char)hit_table_entry_t::hit_propulsion);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_fuel);
        it = std::fill_n(
            it,
            design.weapons_space,
            (signed char)hit_table_entry_t::hit_weapons);
        it = std::fill_n(
            it, design.rounds, (signed char)hit_table_entry_t::hit_rounds);
        it = std::fill_n(
            it, design.missiles, (signed char)hit_table_entry_t::hit_missiles);
        it = std::fill_n(
            it,
            design.shields_space,
            (signed char)hit_table_entry_t::hit_shields);
        it = std::fill_n(
            it,
            design.detection_space,
            (signed char)hit_table_entry_t::hit_detection);
        it = std::fill_n(
            it,
            design.stealth_space,
            (signed char)hit_table_entry_t::hit_stealth);
        it = std::fill_n(
            it, design.water, (signed char)hit_table_entry_t::hit_water);
        it = std::fill_n(
            it, design.supplies, (signed char)hit_table_entry_t::hit_supplies);
        it = std::fill_n(
            it, design.fighters, (signed char)hit_table_entry_t::hit_fighters);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_unused);
        it = std::fill_n(it, 1, (signed char)hit_table_entry_t::hit_cargo);
        EXPECT_EQ(design.hit_table, expected);
    }
}
