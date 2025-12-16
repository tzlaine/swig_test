#include <generate_galaxy.hpp>
#include <model.hpp>
#include <serialization.hpp>
#include <task_system.hpp>

#include <gtest/gtest.h>

#include <format>


template<std::ranges::range R>
auto byte_span_of(R const & r)
{
    return std::as_bytes(std::span(r.begin(), r.end()));
}

unit_design_t design(int nation_id, int id)
{
    int const factor = id + 1;
    return {
        .id = nation_and_object_id_t{nation_id, id},
        .hull = factor,
        .armor = factor,
        .propulsion = 3 / factor,
        .weapons = factor,
        .shields = factor,
        .detection = 3 / factor,
        .stealth = 3 / factor,
        .automation = factor,
        .attack = factor,
        .defense = factor,
        .ground_attack = factor};
}

unit_design_t design(int nation_id, int id, game_state_t const & gs)
{
    return gs.nations[nation_id].unit_designs[id];
}

fleet_t fleet(int nation_id)
{
    return fleet_t{
        .id = nation_and_object_id_t{nation_id, 0},
        .mission = mission_t::explore,
        .units =
            {{.id = {nation_id, 0}, .health = 100},
             {.id = {nation_id, 1}, .health = 100},
             {.id = {nation_id, 2}, .health = 100}},
        .fuel = 100.0f,
        .rounds = 100,
        .missiles = 100,
        .fighters = 25,
        .position = {
            .world_pos_x = 1000.0 * nation_id,
            .world_pos_y = 1000.0 * nation_id,
            .system_id = 10 + nation_id,
            .at_permanent_location = true,
            .location_index = 2,
            .object_index = 3,
            .is_garrison = true}};
}

nation_t nation(int nation_id, game_state_t const & gs)
{
    hex_coord_t const home_hc = {.x = 5, .y = 1 + nation_id};
    int const home_hex = to_index(home_hc, gs.map_width);
    int const home_system = gs.hexes[home_hex].first_system;
    int const home_planet = gs.systems[home_system].first_planet;
    nation_t retval = {
        .id = nation_id,
        .unit_designs =
            {design(nation_id, 0), design(nation_id, 0), design(nation_id, 0)},
        .provinces = {},
        .fleets = {fleet(nation_id)},
        .planets = {home_planet},
        .foreign_designs_seen =
            {{.nation_id = (nation_id + 1) % 3, .object_id = 1}},
        .foreign_designs_glimpsed = {},
        .hexes_seen =
            {home_hex,
             to_index(hex_above_right(home_hc), gs.map_width),
             to_index(hex_above(home_hc), gs.map_width),
             to_index(hex_above_left(home_hc), gs.map_width),
             to_index(hex_below_left(home_hc), gs.map_width),
             to_index(hex_below(home_hc), gs.map_width),
             to_index(hex_below_right(home_hc), gs.map_width)},
        .systems_visited = {home_system},
        .defeated = false};
    return retval;
}

auto visible_fleets(int nation_id, game_state_t const & gs)
{
    return std::vector<fleet_t const *>(
        {&gs.nations[(nation_id + 1) % 3].fleets[0],
         &gs.nations[(nation_id + 2) % 3].fleets[0]});
}

game_state_t const gs = [] {
    task_system ts(4);
    game_state_t retval;
    generation::generate_galaxy(default_game_start_params(), retval, &ts);
    retval.nations = {nation(0, retval), nation(1, retval), nation(2, retval)};
    return retval;
}();

TEST(client_serialization_tests, serialize_for_client_single_object)
{
    using namespace detail;

    {
        std::vector<char> serialized;
        ostream_tarray_facade oss(serialized);
        int const nation_id = 0;
        std::vector<fleet_t const *> visible_fleets =
            ::visible_fleets(nation_id, gs);
        unit_design_t const design = ::design(nation_id, 0);

        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                design,
                visibility_kind::owner,
                &oss);

            unit_design_t client_design = {};
            auto const bytes =
                deserialize_impl(client_design, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_EQ(client_design, design);
        }
        {
            serialized.clear();
            serialize_for_client(
                gs,
                visible_fleets,
                nation_id,
                design,
                visibility_kind::neutral_or_enemy,
                &oss);

            unit_design_t client_design = {};
            auto const bytes =
                deserialize_impl(client_design, byte_span_of(serialized));
            EXPECT_TRUE(bytes.empty());
            EXPECT_NE(client_design, design);

            EXPECT_EQ(client_design.id, design.id);
            EXPECT_EQ(client_design.hull, design.hull);

            EXPECT_EQ(client_design.armor, 0);
            EXPECT_EQ(client_design.propulsion, 0);
            EXPECT_EQ(client_design.weapons, 0);
            EXPECT_EQ(client_design.shields, 0);
            EXPECT_EQ(client_design.detection, 0);
            EXPECT_EQ(client_design.stealth, 0);
            EXPECT_EQ(client_design.automation, 0);
            EXPECT_EQ(client_design.attack, 0);
            EXPECT_EQ(client_design.defense, 0);
            EXPECT_EQ(client_design.ground_attack, 0);
        }
    }
}
