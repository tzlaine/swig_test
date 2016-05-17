#include <start_data_t.hpp>
#include <game_data_t.hpp>
#include <visual_config_t.hpp>
#include <json2pb.h>
#include <graph.hpp>
#include <supply.hpp>
#include <gtest/gtest.h>

#include "test_data.hpp"


TEST(graph_tests, init_graph)
{
    auto get_map_str = [](std::string const &) { return map_json_string; };
    auto get_oob_str = [](std::string const &) { return oob_json_string; };

    start_data::start_data_t start_data;
    start_data.init_unit_defs(units_json_string);
    start_data.init_nations(nations_json_string);
    start_data.init_scenario(scenario_json_string, get_map_str, get_oob_str);

    graph::graph_t g = graph::local_graph(
        hex_id_t(1411).to_hex_coord(),
        [](hex_coord_t hc) { return true; },
        [](hex_coord_t lhs, hex_coord_t rhs) { return 1.0f; },
        start_data.map().width,
        start_data.map().height
    );
}

TEST(graph_tests, init_graph_at_origin)
{
    auto get_map_str = [](std::string const &) { return map_json_string; };
    auto get_oob_str = [](std::string const &) { return oob_json_string; };

    start_data::start_data_t start_data;
    start_data.init_unit_defs(units_json_string);
    start_data.init_nations(nations_json_string);
    start_data.init_scenario(scenario_json_string, get_map_str, get_oob_str);

    graph::graph_t g = graph::local_graph(
        hex_id_t(101).to_hex_coord(),
        [](hex_coord_t hc) { return true; },
        [](hex_coord_t lhs, hex_coord_t rhs) { return 1.0f; },
        start_data.map().width,
        start_data.map().height
    );
}

TEST(graph_tests, init_graph_near_bottom)
{
    auto get_map_str = [](std::string const &) { return map_json_string; };
    auto get_oob_str = [](std::string const &) { return oob_json_string; };

    start_data::start_data_t start_data;
    start_data.init_unit_defs(units_json_string);
    start_data.init_nations(nations_json_string);
    start_data.init_scenario(scenario_json_string, get_map_str, get_oob_str);

    graph::graph_t g = graph::local_graph(
        hex_id_t(1417).to_hex_coord(),
        [](hex_coord_t hc) { return true; },
        [](hex_coord_t lhs, hex_coord_t rhs) { return 1.0f; },
        start_data.map().width,
        start_data.map().height
    );
}

TEST(graph_tests, supply_source)
{
    auto get_map_str = [](std::string const &) { return map_json_string; };
    auto get_oob_str = [](std::string const &) { return oob_json_string; };

    start_data::start_data_t start_data;
    start_data.init_unit_defs(units_json_string);
    start_data.init_nations(nations_json_string);
    start_data.init_scenario(scenario_json_string, get_map_str, get_oob_str);

    game_data_t game_data(start_data);

    auto const & map = game_data.map();
    auto const width = map.width;

    {
        auto const nation_id = start_data.nation_id("KLI"_name);

        {
            hex_id_t const id(1411);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_TRUE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1312);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_TRUE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1611);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_TRUE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1509);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_TRUE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1209);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_TRUE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1410);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_FALSE(supply_source(nation_id, hex, map));
        }
    }


    {
        auto const nation_id = start_data.nation_id("FED"_name);

        {
            hex_id_t const id(1411);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_FALSE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1312);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_FALSE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1611);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_FALSE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1509);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_FALSE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1209);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_FALSE(supply_source(nation_id, hex, map));
        }

        {
            hex_id_t const id(1410);
            hex_index_t const index = id.to_hex_index(width);
            auto const hex = map.hexes[index];
            EXPECT_FALSE(supply_source(nation_id, hex, map));
        }
    }
}


TEST(graph_tests, find_supply_relevant_contents)
{
    auto get_map_str = [](std::string const &) { return map_json_string; };
    auto get_oob_str = [](std::string const &) { return oob_json_string; };

    start_data::start_data_t start_data;
    start_data.init_unit_defs(units_json_string);
    start_data.init_nations(nations_json_string);
    start_data.init_scenario(scenario_json_string, get_map_str, get_oob_str);

    game_data_t game_data(start_data);

    auto const & map = game_data.map();
    auto const width = map.width;

    auto const nation_id = start_data.nation_id("KLI"_name);
    auto const nz_nation_id = start_data.nation_id("NZ"_name);
    auto const team = game_data.team(nation_id, start_data);

    { // friendly capital
        hex_id_t const id(1411);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_FALSE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 3);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 0);
    }

    { // friendly bats
        hex_id_t const id(1507);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_FALSE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 1);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 0);
    }

    { // friendly sb
        hex_id_t const id(1509);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_FALSE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 1);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 0);
    }

    { // friendly planet
        hex_id_t const id(1611);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_FALSE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 0);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 0);
    }

    { // unfriendly capital
        hex_id_t const id(2908);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_FALSE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 0);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 3);
    }

    { // unfriendly bats
        hex_id_t const id(2012);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_FALSE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 0);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 1);
    }

    { // unfriendly sb
        hex_id_t const id(2211);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_FALSE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 0);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 1);
    }

    { // unfriendly planet
        hex_id_t const id(2106);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_FALSE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 0);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 0);
    }

    { // NZ hex
        hex_id_t const id(1406);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_TRUE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 0);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 0);
    }

    { // NZ hex with planet
        hex_id_t const id(1506);
        hex_index_t const index = id.to_hex_index(width);
        auto const hex = map.hexes[index];
        auto const contents = detail::find_supply_relevant_contents(
            hex,
            nation_id,
            team,
            start_data,
            game_data,
            nz_nation_id
        );
        EXPECT_TRUE(contents.neutral_zone);
        EXPECT_EQ(contents.friendly_ships, 0);
        EXPECT_EQ(contents.friendly_units, 0);
        EXPECT_EQ(contents.friendly_bases, 0);
        EXPECT_EQ(contents.enemy_ships, 0);
        EXPECT_EQ(contents.enemy_units, 0);
        EXPECT_EQ(contents.enemy_bases, 0);
    }
}


TEST(graph_tests, find_supply_grids)
{
    auto get_map_str = [](std::string const &) { return map_json_string; };
    auto get_oob_str = [](std::string const &) { return oob_json_string; };

    start_data::start_data_t start_data;
    start_data.init_unit_defs(units_json_string);
    start_data.init_nations(nations_json_string);
    start_data.init_scenario(scenario_json_string, get_map_str, get_oob_str);

    game_data_t game_data(start_data);

    auto const nation_id = start_data.nation_id("KLI"_name);

    std::vector<supply_grid_t> grids = find_supply_grids(nation_id, start_data, game_data);

    EXPECT_EQ(grids.size(), std::size_t(1));

    auto points = grids[0].supply_points;
    std::sort(points.begin(), points.end());
    EXPECT_EQ(std::unique(points.begin(), points.end()), points.end());

    auto hexes = grids[0].hexes_in_supply;
    std::sort(hexes.begin(), hexes.end());
    EXPECT_EQ(std::unique(hexes.begin(), hexes.end()), hexes.end());

    for (int i = 0; i < game_data.map().width * game_data.map().height; ++i) {
        hex_index_t const hex_index(i);
        auto const hc = hex_index.to_hex_coord(game_data.map().width);
        auto const province = game_data.province(start_data.hex_province(hc));
        if (std::binary_search(hexes.begin(), hexes.end(), hc) !=
            (province && province->owner == nation_id)) {
            std::cout << "hex-id=" << hex_id_t(hc).to_int() << "\n";
            auto const & hex_supply_source = grids[0].hex_supply_sources[hc];
            std::cout << "  supplied from " << hex_supply_source.origin_.to_int() << "\n";
            for (auto id : hex_supply_source.path_) {
                std::cout << "    -> " << id.to_int() << "\n";
            }
        }
        EXPECT_EQ(
            std::binary_search(hexes.begin(), hexes.end(), hc),
            province && province->owner == nation_id
        );
    }

    // TODO: Test for more of these.
    {
        hex_id_t const id(2519);
        auto const hc = id.to_hex_coord();
        EXPECT_EQ(
            std::count(grids[0].supply_points.begin(), grids[0].supply_points.end(), hc),
            std::size_t(1)
        );
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
