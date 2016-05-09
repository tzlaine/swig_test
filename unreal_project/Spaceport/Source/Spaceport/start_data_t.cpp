#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "start_data_t.hpp"
#include "validation.hpp"
#include "hex_operations.hpp"

#include <json2pb.h>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <map>
#include <string>


using namespace start_data;

const hex_coord_t invalid_hex_coord{1000, 1000};
const hex_direction_t all_hex_directions[6] = {
    hex_direction_t::above_right,
    hex_direction_t::above,
    hex_direction_t::above_left,
    hex_direction_t::below_left,
    hex_direction_t::below,
    hex_direction_t::below_right
};


namespace pb = google::protobuf;

void fill_in_nation_ids (nations_t& nations)
{
    int id = 0;
    for (auto& n : nations.nations) {
        n.second.nation_id = id++;
    }
}

char const * start_data_t::hex_string (hex_coord_t hc) const
{ return hex_strings_[to_hex_index(hc, map_.width)].c_str(); }

void start_data_t::init_nations (std::string const & nations_str)
{
    assert(nations_str != "");

    {
        pb_message::nations_t nations_msg;
        json2pb(nations_msg, nations_str, map_encoding_t::compact);
        nations_ = from_protobuf(nations_msg);
        validate_nations(nations_);
        fill_in_nation_ids(nations_);
    }

    nations_initialized_ = true;
}

void start_data_t::init_unit_defs (std::string const & unit_defs_str)
{
    assert(unit_defs_str != "");

    {
        pb_message::unit_defs_t unit_defs_msg;
        json2pb(unit_defs_msg, unit_defs_str, map_encoding_t::compact);
        unit_defs_ = from_protobuf(unit_defs_msg);
        validate_unit_defs(unit_defs_);
    }

    unit_defs_initialized_ = true;
}

void start_data_t::parse_scenario_message (std::string const & scenario_str)
{
    assert(scenario_str != "");

    pb_message::scenario_t scenario_msg;
    json2pb(scenario_msg, scenario_str, map_encoding_t::compact);
    scenario_ = from_protobuf(scenario_msg);
    validate_scenario(scenario_, nations_);
}

void start_data_t::init_map (std::string const & map_str)
{
    assert(map_str != "");

    {
        pb_message::map_t map_msg;
        json2pb(map_msg, map_str, map_encoding_t::compact);
        map_ = from_protobuf(map_msg);
        validate_and_fill_in_map_hexes(map_, nations_);
    }

    hex_strings_.resize(map_.width * map_.height);
    for (int i = 0; i < map_.width; ++i) {
        for (int j = 0; j < map_.height; ++j) {
            hex_coord_t const hc = {i, j};
            hex_strings_[to_hex_index(hc, map_.width)] = name_t(::hex_string(hc).c_str());
        }
    }
}

void start_data_t::init_oob (std::string const & oob_str)
{
    assert(oob_str != "");

    {
        pb_message::orders_of_battle_t oob_msg;
        json2pb(oob_msg, oob_str, map_encoding_t::compact);
        oob_ = from_protobuf(oob_msg);
        validate_and_fill_in_unit_times(oob_, map_, nations_, unit_defs_);
    }
}

void start_data_t::init_scenario_impl ()
{
    assert(unit_defs_initialized_);
    assert(nations_initialized_);

    validate_hex_coords(nations_, map_.width, map_.height);

    validate_scenario_with_map_and_oob(scenario_, map_, oob_);

    for (auto const & pair : map_.starting_national_holdings) {
        for (auto const & province : pair.second.provinces) {
            for (auto const & province_hex : province.hexes) {
                auto const hc = to_hex_coord(province_hex.hex);
                auto const inserted = provinces_.insert(std::make_pair(hc, num_provinces_)).second;
                if (!inserted) {
                    throw std::runtime_error(
                        std::string("Hex ") + hex_string(hc) + " cannot be associated with two provinces."
                    );
                }
                province_hexes_.insert(std::make_pair(num_provinces_, hc));
            }
            ++num_provinces_;
        }
    }

    for (auto const & pair : nations_.nations) {
        for (auto const & capital_hex : pair.second.capital.hexes) {
            int const hex_index = to_hex_index(to_hex_coord(capital_hex.coord), map_.width);
            for (auto const & zone : capital_hex.zones) {
                if (0 < std::count(zone.features.begin(), zone.features.end(), feature_t::capital))
                    national_capitals_[nation_id(pair.first)] = hex_index;
            }
        }
    }

    scenario_initialized_ = true;
#if 0
    init_graph(
        g_model_state.g,
        g_model_state.hex_id_property_map,
        g_model_state.edge_weight_map,
        g_model_state.m.width,
        g_model_state.m.height,
        [] (int id1, int id2) {return true;},
        [] (int id1, int id2) {return 1.0;}
    );

    g_model_state.initialized = true;
#endif
}
