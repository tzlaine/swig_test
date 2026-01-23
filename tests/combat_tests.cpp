#include <combat.hpp>

#include <unit_design.hpp>

#include <game_data_formatters.hpp> // TODO

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

unit_design_t design_200 = {
    .hull = 200,
    .armor = 50,
    .propulsion = 4,
    .weapons = 4,
    .shields = 4,
    .detection = 4,
    .stealth = 4,
    .fuel = 4,
    .water = 4,
    .supplies = 4,
    .rounds = 4,
    .missiles = 4,
    .fighters = 4,
    .cargo = 4,
    .propulsion_space = 40,
    .weapons_space = 40,
    .shields_space = 40,
    .detection_space = 40,
    .stealth_space = 40,
    .effective_propulsion = 4,
    .effective_weapons = 4,
    .effective_shields = 4,
    .effective_detection = 4,
    .effective_stealth = 4,
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

unit_design_t design_150 = {
    .hull = 150,
    .armor = 50,
    .propulsion = 4,
    .weapons = 4,
    .shields = 4,
    .detection = 4,
    .stealth = 4,
    .fuel = 4,
    .water = 4,
    .supplies = 4,
    .rounds = 4,
    .missiles = 4,
    .fighters = 4,
    .cargo = 4,
    .propulsion_space = 40,
    .weapons_space = 40,
    .shields_space = 40,
    .detection_space = 40,
    .stealth_space = 40,
    .effective_propulsion = 4,
    .effective_weapons = 4,
    .effective_shields = 4,
    .effective_detection = 4,
    .effective_stealth = 4,
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

bool const dummy = [&] {
    design_200.crew = call_lua_func("unit_crew", design_200);
    design_150.crew = call_lua_func("unit_crew", design_150);
    return true;
}();

settlement_t resupply_settlement{
    .water = {.stockpile = 1.0e9},
    .fuel = {.stockpile = 1.0e9},
    .supplies = {.stockpile = 1.0e9},
    .crewpower = {.stockpile = 1.0e9},
    .rounds = {.stockpile = 1.0e9},
    .missiles = {.stockpile = 1.0e9},
    .fighters = {.stockpile = 1.0e9},
};

unit_t create_and_supply_unit(unit_design_t const & design)
{
    unit_t retval = call_lua_func("create_unit", design);
    call_lua_func("resupply_unit", retval, design, resupply_settlement);
    return retval;
}

TEST(combat_tests, combat_units_ctor)
{
    scoped_reset _1(design_200);
    scoped_reset _2(design_150);
    scoped_reset _3(nation);

    game_state_t gs;

    {
        nation.id = 0;
        design_150.id = {0, 0};
        design_200.id = {0, 1};
        nation.unit_designs.push_back(design_150);
        nation.unit_designs.push_back(design_200);
        fleet_t fleet{
            .id = {0, 0},
            .mission = mission_t::hunt_enemy_fleets,
            .fleet_combat_experience = 0};
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_200));
        nation.fleets.push_back(fleet);
        gs.nations.push_back(nation);
    }

    unit_t const unit_150 = create_and_supply_unit(design_150);
    unit_t const unit_200 = create_and_supply_unit(design_200);

    combat_units side(gs, gs.nations[0].fleets);

    std::vector<unit_t *> all_units(
        {side.combat_units_[0].unit_,
         side.combat_units_[1].unit_,
         side.combat_units_[2].unit_,
         side.combat_units_[3].unit_});
    for (auto * unit : all_units) {
        auto const it = std::ranges::find_if(
            gs.nations[0].fleets[0].units,
            [unit](auto const & e) { return &e == unit; });
        EXPECT_NE(it, gs.nations[0].fleets[0].units.end());
    }
    {
        std::ranges::sort(all_units, std::ranges::less{});
        auto const nonuniques = std::ranges::unique(all_units);
        EXPECT_EQ(nonuniques.size(), 0u);
    }

    std::vector<int> all_unit_fleet_indices(
        {side.combat_units_[0].fleet_unit_index_,
         side.combat_units_[1].fleet_unit_index_,
         side.combat_units_[2].fleet_unit_index_,
         side.combat_units_[3].fleet_unit_index_});
    for (auto i : all_unit_fleet_indices) {
        EXPECT_LT(i, (int)gs.nations[0].fleets[0].units.size());
    }
    {
        std::ranges::sort(all_unit_fleet_indices, std::ranges::less{});
        auto const nonuniques = std::ranges::unique(all_unit_fleet_indices);
        EXPECT_EQ(nonuniques.size(), 0u);
    }

    EXPECT_EQ(side.combat_units_.size(), 4u);
    EXPECT_EQ(*side.combat_units_[0].unit_, unit_200);
    EXPECT_EQ(*side.combat_units_[0].design_, design_200);
    EXPECT_EQ(side.combat_units_[0].fleet_, gs.nations[0].fleets.data());
    EXPECT_EQ(side.combat_units_[0].nation_, gs.nations.data());
    EXPECT_GT(side.combat_units_[0].acceleration_, 0.0f);
    EXPECT_EQ(side.combat_units_[0].combat_unit_index_, 0);
    EXPECT_TRUE(side.combat_units_[0].capital_);

    EXPECT_EQ(*side.combat_units_[1].unit_, unit_150);
    EXPECT_EQ(*side.combat_units_[1].design_, design_150);
    EXPECT_EQ(side.combat_units_[1].fleet_, gs.nations[0].fleets.data());
    EXPECT_EQ(side.combat_units_[1].nation_, gs.nations.data());
    EXPECT_GT(side.combat_units_[1].acceleration_, 0.0f);
    EXPECT_EQ(side.combat_units_[1].combat_unit_index_, 1);
    EXPECT_FALSE(side.combat_units_[1].capital_);

    EXPECT_EQ(*side.combat_units_[2].unit_, unit_150);
    EXPECT_EQ(*side.combat_units_[2].design_, design_150);
    EXPECT_EQ(side.combat_units_[2].fleet_, gs.nations[0].fleets.data());
    EXPECT_EQ(side.combat_units_[2].nation_, gs.nations.data());
    EXPECT_GT(side.combat_units_[2].acceleration_, 0.0f);
    EXPECT_EQ(side.combat_units_[2].combat_unit_index_, 2);
    EXPECT_FALSE(side.combat_units_[2].capital_);

    EXPECT_EQ(*side.combat_units_[3].unit_, unit_150);
    EXPECT_EQ(*side.combat_units_[3].design_, design_150);
    EXPECT_EQ(side.combat_units_[3].fleet_, gs.nations[0].fleets.data());
    EXPECT_EQ(side.combat_units_[3].nation_, gs.nations.data());
    EXPECT_GT(side.combat_units_[3].acceleration_, 0.0f);
    EXPECT_EQ(side.combat_units_[3].combat_unit_index_, 3);
    EXPECT_FALSE(side.combat_units_[3].capital_);

    EXPECT_EQ(
        std::ranges::count(side.target_table_, 0),
        capital_combat_selection_factor);
    EXPECT_EQ(
        std::ranges::count(side.target_table_, 1),
        screen_combat_selection_factor);
    EXPECT_EQ(
        std::ranges::count(side.target_table_, 2),
        screen_combat_selection_factor);
    EXPECT_EQ(
        std::ranges::count(side.target_table_, 3),
        screen_combat_selection_factor);
}

TEST(combat_tests, rolls)
{
    std::vector<double> rolls({0, 0.5, 1});
    int i = 0;
    EXPECT_EQ(next_roll(rolls, i), 0.5);
    EXPECT_EQ(next_roll(rolls, i), 1);
    EXPECT_EQ(next_roll(rolls, i), 0);
    EXPECT_EQ(next_roll(rolls, i), 0.5);
}

TEST(combat_tests, apply_hit_design_grade_a)
{
    scoped_reset _(nation);

    unit_design_t design = {
        .hull = 70,
        .armor = 35,
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
        .cargo = 5,
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

    detail::generate_hit_table(design, 1.0);

    unit_t unit = create_and_supply_unit(design);
    combat_unit cu{.unit_ = &unit, .design_ = &design, .nation_ = &nation};
    cu.acceleration_ = combat_acceleration(cu);

    unit.hit_table = design.hit_table;
    load_cargo(unit.cargo, unit.hit_table);

    unit_t const unit_copy = unit;
    combat_unit const cu_copy = cu;

    EXPECT_EQ(
        unit.hit_table,
        std::vector<signed char>(
            {15, 15, 15, 1, 1,  1,  1, 1,  1,  1,  1,  1,  1,  2,  2,  2, 2, 2,
             2,  2,  2,  2, 2,  3,  3, 3,  3,  3,  3,  3,  3,  3,  3,  4, 4, 4,
             4,  4,  4,  4, 4,  4,  4, 5,  5,  5,  5,  5,  5,  5,  5,  5, 5, 7,
             8,  11, 14, 6, 14, 14, 9, 14, 14, 10, 14, 12, 12, 12, 12, 12}));

    combat_log log;

    // Check that multiple hits to the same location gets transferred to the
    // next non-destroyed location.

    apply_hit(cu, 0, log); // crew space
    EXPECT_EQ(unit.hit_table[0], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        1u);
    EXPECT_LT(
        unit.crew,
        design.crew -
            1000.0f / space_required_per_1k_crew * (1 - crew_onduty_factor));

    apply_hit(cu, 0, log); // same spot -- crew space, now destroyed
    EXPECT_EQ(unit.hit_table[1], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        2u);
    EXPECT_LT(
        unit.crew,
        design.crew -
            2000.0f / space_required_per_1k_crew * (1 - crew_onduty_factor));

    apply_hit(cu, 0, log); // same spot again -- crew space, now destroyed
    EXPECT_EQ(unit.hit_table[2], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        3u);
    EXPECT_LT(
        unit.crew,
        design.crew -
            3000.0f / space_required_per_1k_crew * (1 - crew_onduty_factor));

    apply_hit(cu, 0, log); // same spot again -- crew space, now destroyed
    EXPECT_EQ(unit.hit_table[3], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        4u);
    EXPECT_NEAR(unit.propulsion, 0.9f, eps);
    EXPECT_NEAR(cu.acceleration_, cu_copy.acceleration_ * 0.9f, eps);

    apply_hit(cu, unit.hit_table.size() - 1, log); // empty cargo hold
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 1],
        (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        5u);

    apply_hit(cu, unit.hit_table.size() - 1, log); // same spot again -- empty cargo hold
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 2],
        (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        6u);

    apply_hit(cu, unit.hit_table.size() - 1, log); // same spot again -- empty cargo hold
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 3],
        (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        7u);

    // Check damage to explosive locations, with lots of unused padding.

    apply_hit(cu, unit.hit_table.size() - 7, log); // missiles
    EXPECT_EQ(unit.missiles, 0);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 9],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 8],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 7],
        (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 6],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 5],
        (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        9u);

    apply_hit(cu, unit.hit_table.size() - 10, log); // rounds
    EXPECT_EQ(unit.rounds, 0);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 12],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 11],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 10],
        (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 9],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 8],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        10u);

    apply_hit(cu, unit.hit_table.size() - 13, log); // fuel
    EXPECT_NEAR(unit.fuel, 0.0f, eps);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 14],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 13],
        (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[unit.hit_table.size() - 12],
        (signed char)hit_table_entry_t::hit_unused);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        11u);
}

TEST(combat_tests, apply_hit_design_grade_f)
{
    scoped_reset _(nation);

    unit_design_t design = {
        .hull = 70,
        .armor = 35,
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
        .cargo = 5,
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

    detail::generate_hit_table(design, 0.0);

    unit_t unit = create_and_supply_unit(design);
    combat_unit cu{
        .unit_ = &unit,
        .design_ = &design,
        .nation_ = &nation,
    };

    unit.hit_table = design.hit_table;
    load_cargo(unit.cargo, unit.hit_table);

    unit_t const unit_copy = unit;

    EXPECT_EQ(
        unit.hit_table,
        std::vector<signed char>(
            {15, 15, 15, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  6,  2,  2, 2, 2,
             2,  2,  2,  2, 2,  2,  9,  10, 3,  3,  3,  3,  3,  3,  3,  3, 3, 3,
             4,  4,  4,  4, 4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5, 5, 5,
             5,  5,  7,  8, 11, 14, 14, 14, 14, 14, 14, 12, 12, 12, 12, 12}));

    combat_log log;

    EXPECT_EQ(unit.hit_table[13], (signed char)hit_table_entry_t::hit_fuel);
    apply_hit(cu, 13, log);
    EXPECT_NEAR(unit.fuel, 0.0f, eps);
    EXPECT_EQ(
        unit.hit_table[12], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[13], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[14], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        3u);

    EXPECT_EQ(unit.hit_table[24], (signed char)hit_table_entry_t::hit_rounds);
    apply_hit(cu, 24, log);
    EXPECT_EQ(unit.rounds, 0);
    EXPECT_EQ(unit.missiles, 0);
    EXPECT_EQ(
        unit.hit_table[22], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[23], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[24], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[25], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[26], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        unit.hit_table[27], (signed char)hit_table_entry_t::hit_destroyed);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        9u);

    EXPECT_EQ(unit.hit_table[56], (signed char)hit_table_entry_t::hit_water);
    unit.water = 0.5;
    apply_hit(cu, 56, log);
    EXPECT_NEAR(unit.water, 0.0f, eps);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        10u);

    EXPECT_EQ(unit.hit_table[57], (signed char)hit_table_entry_t::hit_supplies);
    unit.supplies = 0.5;
    apply_hit(cu, 57, log);
    EXPECT_NEAR(unit.supplies, 0.0f, eps);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        11u);
}

TEST(combat_tests, apply_hit_design_explosion_chaining)
{
    scoped_reset _(nation);

    unit_design_t design = {
        .hull = 70,
        .armor = 35,
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
        .cargo = 5,
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

    detail::generate_hit_table(design, 0.0);

    unit_t unit = create_and_supply_unit(design);
    combat_unit cu{
        .unit_ = &unit,
        .design_ = &design,
        .nation_ = &nation,
    };

    unit.hit_table = design.hit_table;
    load_cargo(unit.cargo, unit.hit_table);

    unit_t const unit_copy = unit;

    EXPECT_EQ(
        unit.hit_table,
        std::vector<signed char>(
            {15, 15, 15, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  6,  2,  2, 2, 2,
             2,  2,  2,  2, 2,  2,  9,  10, 3,  3,  3,  3,  3,  3,  3,  3, 3, 3,
             4,  4,  4,  4, 4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5, 5, 5,
             5,  5,  7,  8, 11, 14, 14, 14, 14, 14, 14, 12, 12, 12, 12, 12}));

    combat_log log;

    for (int i = (int)unit.hit_table.size() - 4,
             last = (int)unit.hit_table.size();
         i < last;
         ++i) {
        unit = unit_copy;
        unit.hit_table = std::vector<signed char>(
            {15, 15, 15, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  6,  2, 2, 2,
             2,  2,  2,  2, 2, 2, 2, 9,  10, 3,  3,  3,  3,  3,  3, 3, 3,
             3,  3,  4,  4, 4, 4, 4, 4,  4,  4,  4,  4,  5,  5,  5, 5, 5,
             5,  5,  5,  5, 5, 7, 8, 11, 14, 14, 14, 14, 14, 14, 12});
        unit.cargo = {
            (signed char)cargo_kind_t::fuel,
            (signed char)cargo_kind_t::missiles};
        unit.hit_table.insert(
            unit.hit_table.end(),
            {(signed char)hit_table_entry_t::hit_fuel,
             (signed char)cargo_kind_t::fuel,
             (signed char)hit_table_entry_t::hit_rounds,
             (signed char)cargo_kind_t::missiles});

        apply_hit(cu, i, log);
        EXPECT_EQ(
            std::ranges::count(
                unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
            5u);
        EXPECT_TRUE(unit.cargo.empty());
    }

    for (int i = (int)unit.hit_table.size() - 4,
             last = (int)unit.hit_table.size();
         i < last;
         ++i) {
        unit = unit_copy;
        unit.hit_table = std::vector<signed char>(
            {15, 15, 15, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  6,  2, 2, 2,
             2,  2,  2,  2, 2, 2, 2, 9,  10, 3,  3,  3,  3,  3,  3, 3, 3,
             3,  3,  4,  4, 4, 4, 4, 4,  4,  4,  4,  4,  5,  5,  5, 5, 5,
             5,  5,  5,  5, 5, 7, 8, 11, 14, 14, 14, 14, 14, 12, 12});
        unit.cargo = {
            (signed char)cargo_kind_t::fuel,
            (signed char)cargo_kind_t::missiles};
        unit.hit_table.insert(
            unit.hit_table.end(),
            {(signed char)hit_table_entry_t::hit_fuel,
             (signed char)hit_table_entry_t::hit_rounds,
             (signed char)cargo_kind_t::fuel,
             (signed char)cargo_kind_t::missiles});

        apply_hit(cu, i, log);
        EXPECT_EQ(
            std::ranges::count(
                unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
            5u);
        EXPECT_TRUE(unit.cargo.empty());
    }

    unit = unit_copy;
    unit.hit_table = std::vector<signed char>(
        {15, 15, 15, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  6, 2, 2, 2,
         2,  2,  2,  2, 2, 2, 2, 9,  10, 3,  3,  3,  3,  3, 3, 3, 3,
         3,  3,  4,  4, 4, 4, 4, 4,  4,  4,  4,  4,  5,  5, 5, 5, 5,
         5,  5,  5,  5, 5, 7, 8, 11, 14, 14, 14, 14, 12, 12});
    unit.cargo = {
        (signed char)cargo_kind_t::fuel, (signed char)cargo_kind_t::missiles};
    unit.hit_table.insert(
        unit.hit_table.end(),
        {(signed char)hit_table_entry_t::hit_fuel,
         (signed char)hit_table_entry_t::hit_cargo,
         (signed char)hit_table_entry_t::hit_rounds,
         (signed char)cargo_kind_t::fuel,
         (signed char)cargo_kind_t::missiles});
    apply_hit(cu, unit.hit_table.size() - 5, log);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        3u);

    unit = unit_copy;
    unit.hit_table = std::vector<signed char>(
        {15, 15, 15, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  6, 2, 2, 2,
         2,  2,  2,  2, 2, 2, 2, 9,  10, 3,  3,  3,  3,  3, 3, 3, 3,
         3,  3,  4,  4, 4, 4, 4, 4,  4,  4,  4,  4,  5,  5, 5, 5, 5,
         5,  5,  5,  5, 5, 7, 8, 11, 14, 14, 14, 14, 12, 12});
    unit.cargo = {
        (signed char)cargo_kind_t::fuel, (signed char)cargo_kind_t::missiles};
    unit.hit_table.insert(
        unit.hit_table.end(),
        {(signed char)hit_table_entry_t::hit_fuel,
         (signed char)hit_table_entry_t::hit_cargo,
         (signed char)hit_table_entry_t::hit_rounds,
         (signed char)cargo_kind_t::fuel,
         (signed char)cargo_kind_t::missiles});
    apply_hit(cu, unit.hit_table.size() - 3, log);
    EXPECT_EQ(
        std::ranges::count(
            unit.hit_table, (signed char)hit_table_entry_t::hit_destroyed),
        6u);
    EXPECT_TRUE(unit.cargo.empty());
}

TEST(combat_tests, TODO)
{
    scoped_reset _2(design_200);
    scoped_reset _1(design_150);
    scoped_reset _3(nation);

    game_state_t gs;

    {
        auto this_nation = nation;
        this_nation.id = 0;
        design_150.id = {0, 0};
        design_200.id = {0, 1};
        this_nation.unit_designs.push_back(design_150);
        this_nation.unit_designs.push_back(design_200);
        fleet_t fleet{
            .id = {0, 0},
            .mission = mission_t::hunt_enemy_fleets,
            .fleet_combat_experience = 0};
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_200));
        this_nation.fleets.push_back(fleet);
        gs.nations.push_back(this_nation);
    }
    {
        auto this_nation = nation;
        this_nation.id = 1;
        design_150.id = {1, 0};
        design_200.id = {1, 1};
        this_nation.unit_designs.push_back(design_150);
        this_nation.unit_designs.push_back(design_200);
        fleet_t fleet{
            .id = {1, 0},
            .mission = mission_t::hunt_enemy_fleets,
            .fleet_combat_experience = 0};
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_150));
        fleet.units.push_back(create_and_supply_unit(design_200));
        this_nation.fleets.push_back(fleet);
        gs.nations.push_back(this_nation);
    }

    combat_units side_1(gs, gs.nations[0].fleets);
    combat_units side_2(gs, gs.nations[1].fleets);

    EXPECT_TRUE(true); // TODO
}
