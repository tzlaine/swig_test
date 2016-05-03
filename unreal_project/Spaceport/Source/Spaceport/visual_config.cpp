// WARNING: Generated code.
// This file was generated from visual_config.proto (proto3)

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "visual_config.hpp"



namespace visual_config { 

} 

    message::visual_config::color_t to_protobuf (const visual_config::color_t& value)
    {
        message::visual_config::color_t retval;
        retval.set_r(value.r);
        retval.set_g(value.g);
        retval.set_b(value.b);
        return retval;
    }

    visual_config::color_t from_protobuf (const message::visual_config::color_t& msg)
    {
        visual_config::color_t retval;
        retval.r = msg.r();
        retval.g = msg.g();
        retval.b = msg.b();
        return retval;
    }

    message::visual_config::hex_map_t to_protobuf (const visual_config::hex_map_t& value)
    {
        message::visual_config::hex_map_t retval;
        for (const auto& x : value.primary_colors) {
            (*retval.mutable_primary_colors())[x.first] = to_protobuf(x.second);
        }
        for (const auto& x : value.secondary_colors) {
            (*retval.mutable_secondary_colors())[x.first] = to_protobuf(x.second);
        }
        retval.set_national_border_thickness(value.national_border_thickness);
        retval.set_province_border_thickness(value.province_border_thickness);
        retval.set_planet_star_thickness(value.planet_star_thickness);
        retval.set_minor_planet_scale(value.minor_planet_scale);
        retval.set_major_planet_scale(value.major_planet_scale);
        retval.set_star_scale(value.star_scale);
        retval.set_mb_scale(value.mb_scale);
        retval.set_bats_scale(value.bats_scale);
        retval.set_sb_scale(value.sb_scale);
        retval.set_offmap_z(value.offmap_z);
        retval.set_offmap_border_thickness(value.offmap_border_thickness);
        retval.set_offmap_label_size(value.offmap_label_size);
        retval.set_offmap_left_right_thickness(value.offmap_left_right_thickness);
        retval.set_offmap_top_bottom_thickness(value.offmap_top_bottom_thickness);
        return retval;
    }

    visual_config::hex_map_t from_protobuf (const message::visual_config::hex_map_t& msg)
    {
        visual_config::hex_map_t retval;
        {
            for (const auto& x : msg.primary_colors()) {
                retval.primary_colors[x.first] = from_protobuf(x.second);
            }
        }
        {
            for (const auto& x : msg.secondary_colors()) {
                retval.secondary_colors[x.first] = from_protobuf(x.second);
            }
        }
        retval.national_border_thickness = msg.national_border_thickness();
        retval.province_border_thickness = msg.province_border_thickness();
        retval.planet_star_thickness = msg.planet_star_thickness();
        retval.minor_planet_scale = msg.minor_planet_scale();
        retval.major_planet_scale = msg.major_planet_scale();
        retval.star_scale = msg.star_scale();
        retval.mb_scale = msg.mb_scale();
        retval.bats_scale = msg.bats_scale();
        retval.sb_scale = msg.sb_scale();
        retval.offmap_z = msg.offmap_z();
        retval.offmap_border_thickness = msg.offmap_border_thickness();
        retval.offmap_label_size = msg.offmap_label_size();
        retval.offmap_left_right_thickness = msg.offmap_left_right_thickness();
        retval.offmap_top_bottom_thickness = msg.offmap_top_bottom_thickness();
        return retval;
    }
