#include <start_data_t.hpp>
#include <game_data_t.hpp>
#include <visual_config_t.hpp>
#include <json2pb.h>
#include <graph.hpp>
#include <gtest/gtest.h>

// This horrible business works around the fact that the MS compiler
// chokes on string literals that are "too long".
#ifdef _MSC_VER
#include <fstream>

std::string nations_json_string;
std::string map_json_string;
std::string oob_json_string;
std::string units_json_string;
std::string scenario_json_string;
std::string hex_map_config_json_string;

std::string file_slurp (const char* filename)
{
    std::string retval;
    std::ifstream ifs(filename);
    std::getline(ifs, retval, '\0');
    return retval;
}
#else
#include <test_data.hpp>
#endif

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
#ifdef _MSC_VER
    if (1 < argc)
        nations_json_string = file_slurp(argv[1]);

    if (2 < argc)
        map_json_string = file_slurp(argv[2]);

    if (3 < argc)
        oob_json_string = file_slurp(argv[3]);

    if (4 < argc)
        units_json_string = file_slurp(argv[4]);

    if (5 < argc)
        scenario_json_string = file_slurp(argv[5]);

    if (6 < argc)
        hex_map_config_json_string = file_slurp(argv[6]);
#endif

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
