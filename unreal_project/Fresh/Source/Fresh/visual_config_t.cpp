#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "visual_config_t.hpp"
#include "validation.hpp"
#include "start_data_t.hpp"

#include <json2pb.h>


using namespace visual_config;

void visual_config_t::init_hex_map (
    std::string const & hex_map_str,
    start_data::start_data_t const & start_data
) {
    assert(hex_map_str != "");

    {
        pb_message::visual_config::hex_map_t hex_map_msg;
        json2pb(hex_map_msg, hex_map_str, map_encoding_t::compact);
        hex_map_ = from_protobuf(hex_map_msg);
        visual_config::validate_hex_map(hex_map_, start_data.nations());
    }

    hex_map_initialized_ = true;
}
