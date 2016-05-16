#include <start_data_t.hpp>
#include <game_data_t.hpp>
#include <visual_config_t.hpp>
#include <json2pb.h>
#include <graph.hpp>
#include <supply.hpp>
#include <benchmark/benchmark.h>

#include "../tests/test_data.hpp"

#include <sstream>


void BM_find_supply_grids (benchmark::State & state)
{
    std::vector<supply_grid_t> grids;

    while (state.KeepRunning()) {
        state.PauseTiming();
            auto get_map_str = [](std::string const &) { return map_json_string; };
            auto get_oob_str = [](std::string const &) { return oob_json_string; };

            start_data::start_data_t start_data;
            start_data.init_unit_defs(units_json_string);
            start_data.init_nations(nations_json_string);
            start_data.init_scenario(scenario_json_string, get_map_str, get_oob_str);

            game_data_t game_data(start_data);

            auto const nation_id = start_data.nation_id("KLI"_name);

            std::vector<supply_grid_t> empty_grids;
            grids.swap(empty_grids);
        state.ResumeTiming();

        // Timing on from here.
        grids = find_supply_grids(nation_id, start_data, game_data);
    }

    // Prevent optimiations from gutting the test.
    std::stringstream ss;
    ss << grids.size() << " grids found.\n";
    state.SetLabel(ss.str());
}

BENCHMARK(BM_find_supply_grids);

BENCHMARK_MAIN()
