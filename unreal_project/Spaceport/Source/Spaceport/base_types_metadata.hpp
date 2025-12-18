// WARNING: Generated code.
// This file was generated from base_types.proto (proto3)
#pragma once

#include <metadata.hpp>

#include "base_types.hpp"



namespace detail {

    template<> struct metadata<hex_coord_t>
    {
        static constexpr std::string_view struct_name()
        {
            using namespace std::literals;
            return "hex_coord_t"sv;
        }
        static constexpr int lo_field_number() { return 1; }
        static constexpr int hi_field_number() { return 2; }

        static constexpr metadatum<hex_coord_t, int> x()
        {
            using namespace std::literals;
            return {"x"sv, 1, &hex_coord_t::x};
        }
        static constexpr metadatum<hex_coord_t, int> y()
        {
            using namespace std::literals;
            return {"y"sv, 2, &hex_coord_t::y};
        }
    };

}


