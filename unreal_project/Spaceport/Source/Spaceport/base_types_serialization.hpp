// WARNING: Generated code.
// This file was generated from base_types.proto (proto3)

#include "base_types.hpp"



#include <serialization.hpp>
#if defined(BUILD_FOR_TEST)
#include <iosfwd>
#endif

namespace detail {

    template<ser_op Op, ser_field_op FieldOp>
    std::ptrdiff_t serialize_message_impl(hex_coord_t const & x, int field_number, std::ostream * os)
    {
        std::ptrdiff_t retval = 0;
    
        if constexpr (FieldOp == ser_field_op::write) {
            uint8_t buf[16];
            uint8_t * out = buf;
            out = os::WriteVarint32ToArray(field_number, out);
            detail::count_or_write<Op>(retval, buf, out - buf, os);
        }
    
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.x, 1, os);
        retval += detail::serialize_impl<Op, ser_field_op::write>(x.y, 2, os);
    
        retval += detail::serialize_message_end<Op>(os);
    
        return retval;
    }
    template<> inline std::span<std::byte const> deserialize_message_impl<hex_coord_t>(hex_coord_t & x, std::span<std::byte const> src)
    {
        using namespace std::literals;
        constexpr auto this_message_name = "hex_coord_t"sv;
        constexpr std::array<std::string_view, 3> field_names = {{"<UNKOWN_FIELD>"sv,
          "x"sv, "y"sv}};
        std::array<int, 2> expected_field_numbers = {{
          1, 2}};
    
        constexpr int lo_field_number = 1;
        constexpr int hi_field_number = 2;
    
        auto read_field = [] (hex_coord_t & x, int i, std::span<std::byte const> src) {
            switch (i) {
            case 1: return detail::deserialize_impl(x.x, src);
            case 2: return detail::deserialize_impl(x.y, src);
            default: return src; // unreachable
            }
        };
    
        return detail::deserialize_message_impl_impl<lo_field_number, hi_field_number>(
            x, src, this_message_name, field_names, expected_field_numbers, read_field);
    }

}


