// WARNING: Generated code.
// This file was generated from visual_config (proto3)
#pragma once

#include <visual_config.pb.h>


#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>


namespace visual_config { 

    struct color_t
    {
        int r;
        int g;
        int b;
    };

    struct hex_map_t
    {
        boost::container::flat_map<std::string, color_t> primary_colors;
        boost::container::flat_map<std::string, color_t> secondary_colors;
        float national_border_thickness;
        float province_border_thickness;
        float planet_star_thickness;
        float minor_planet_scale;
        float major_planet_scale;
        float star_scale;
        float mb_scale;
        float bats_scale;
        float sb_scale;
        float offmap_z;
        float offmap_border_thickness;
        float offmap_label_size;
        float offmap_left_right_thickness;
        float offmap_top_bottom_thickness;
    };

} 

    message::visual_config::color_t to_protobuf (const visual_config::color_t& value);
    visual_config::color_t from_protobuf (const message::visual_config::color_t& msg);

    message::visual_config::hex_map_t to_protobuf (const visual_config::hex_map_t& value);
    visual_config::hex_map_t from_protobuf (const message::visual_config::hex_map_t& msg);
