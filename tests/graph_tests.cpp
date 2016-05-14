#include <start_data_t.hpp>
#include <game_data_t.hpp>
#include <visual_config_t.hpp>
#include <json2pb.h>
#include <graph.hpp>
#include <gtest/gtest.h>

#include "test_data.hpp"


TEST(graph_tests, construct)
{
    auto get_map_str = [](std::string const &) { return map_json_string; };
    auto get_oob_str = [](std::string const &) { return oob_json_string; };

    start_data::start_data_t start_data;
    start_data.init_unit_defs(units_json_string);
    start_data.init_nations(nations_json_string);
    start_data.init_scenario(scenario_json_string, get_map_str, get_oob_str);

    graph::graph_t g;
    graph::hex_id_property_map_t hex_id_property_map;
    graph::edge_weight_property_map_t edge_weight_map;
    init_graph(
        g,
        hex_id_property_map,
        edge_weight_map,
        hex_id_t(1411).to_hex_coord(),
        [](hex_coord_t hc) { return true; },
        [](hex_coord_t lhs, hex_coord_t rhs) { return 1.0f; },
        start_data.map().width,
        start_data.map().height
    );
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
