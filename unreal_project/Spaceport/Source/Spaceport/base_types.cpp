// WARNING: Generated code.
// This file was generated from base_types.proto (proto3)

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "base_types.hpp"







pb_message::hex_coord_t to_protobuf (const ::hex_coord_t& value)
{
    pb_message::hex_coord_t retval;
    retval.set_x(value.x);
    retval.set_y(value.y);
    return retval;
}

::hex_coord_t from_protobuf (const pb_message::hex_coord_t& msg)
{
    ::hex_coord_t retval;
    retval.x = msg.x();
    retval.y = msg.y();
    return retval;
}
