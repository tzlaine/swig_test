// WARNING: Generated code.
// This file was generated from base_types.proto (proto3)

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "base_types.hpp"







message::turn_t to_protobuf (const ::turn_t& value)
{
    message::turn_t retval;
    retval.set_year(value.year);
    retval.set_season(static_cast< message::season_t >(value.season));
    return retval;
}

::turn_t from_protobuf (const message::turn_t& msg)
{
    ::turn_t retval;
    retval.year = msg.year();
    retval.season = static_cast<std::remove_reference<decltype(retval.season)>::type>(msg.season());
    return retval;
}

message::hex_coord_t to_protobuf (const ::hex_coord_t& value)
{
    message::hex_coord_t retval;
    retval.set_x(value.x);
    retval.set_y(value.y);
    return retval;
}

::hex_coord_t from_protobuf (const message::hex_coord_t& msg)
{
    ::hex_coord_t retval;
    retval.x = msg.x();
    retval.y = msg.y();
    return retval;
}
