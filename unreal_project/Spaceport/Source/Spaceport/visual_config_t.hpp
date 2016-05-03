#pragma once

#include "visual_config.hpp"


namespace start_data { struct start_data_t; }

namespace visual_config {

    struct visual_config_t
    {
        visual_config_t () :
            hex_map_initialized_ (false)
        {}

        hex_map_t const & hex_map () const
        { assert(hex_map_initialized_); return hex_map_; }

        void init_hex_map (
            std::string const & hex_map_str,
            start_data::start_data_t const & start_data
        );

    private:
        hex_map_t hex_map_;
        bool hex_map_initialized_;
    };

}
