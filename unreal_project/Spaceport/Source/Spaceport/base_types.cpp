// WARNING: Generated code.
// This file was generated from base_types.proto (proto3)

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "base_types.hpp"







namespace {

    bool bool_from_bin (unsigned char*& bin)
    {
        bool retval = static_cast<bool>(*bin);
        bin += 1;
        return retval;
    }

    int int_from_bin (unsigned char*& bin)
    {
        int retval =
            (bin[0] << 24) +
            (bin[1] << 16) +
            (bin[2] <<  8) +
            (bin[3] <<  0);
        bin += 4;
        return retval;
    }

    float float_from_bin (unsigned char*& bin)
    {
        float retval;
        memcpy(&retval, bin, sizeof(float));
        bin += sizeof(float);
        return retval;
    }

    double double_from_bin (unsigned char*& bin)
    {
        double retval;
        memcpy(&retval, bin, sizeof(double));
        bin += sizeof(double);
        return retval;
    }

    template <typename T>
    T enum_from_bin (unsigned char*& bin)
    {
        int i = int_from_bin(bin);
        return static_cast<T>(i);
    }

    std::string string_from_bin (unsigned char*& bin)
    {
        std::size_t length = int_from_bin(bin);
        std::string retval((const char*)bin, length);
        bin += length;
        return retval;
    }

    void to_bin (bool b, std::vector<unsigned char>& bin)
    {
        bin.push_back(static_cast<unsigned char>(b));
    }

    void to_bin (int i, std::vector<unsigned char>& bin)
    {
        bin.push_back(static_cast<unsigned char>((i >> 24) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >> 16) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >>  8) & 0xff));
        bin.push_back(static_cast<unsigned char>((i >>  0) & 0xff));
    }

    void to_bin (float f, std::vector<unsigned char>& bin)
    {
        bin.resize(bin.size() + sizeof(float));
        memcpy(&bin[bin.size() - sizeof(float)], &f, sizeof(float));
    }

    void to_bin (double d, std::vector<unsigned char>& bin)
    {
        bin.resize(bin.size() + sizeof(double));
        memcpy(&bin[bin.size() - sizeof(double)], &d, sizeof(double));
    }

    template <typename T>
    void to_bin (T e, std::vector<unsigned char>& bin)
    {
        int i = static_cast<int>(e);
        to_bin(i, bin);
    }

    void to_bin (const std::string s, std::vector<unsigned char>& bin)
    {
        int length = static_cast<int>(s.size());
        to_bin(length, bin);
        bin.resize(bin.size() + length);
        std::copy(s.begin(), s.end(), bin.end() - length);
    }

}


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

void to_bin (const ::turn_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.year, bin);
    to_bin(value.season, bin);
}

::turn_t turn_t_from_bin (unsigned char*& bin)
{
    ::turn_t retval;
    retval.year = int_from_bin(bin);
    retval.season = enum_from_bin<std::remove_reference<decltype(retval.season)>::type>(bin);
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

void to_bin (const ::hex_coord_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.x, bin);
    to_bin(value.y, bin);
}

::hex_coord_t hex_coord_t_from_bin (unsigned char*& bin)
{
    ::hex_coord_t retval;
    retval.x = int_from_bin(bin);
    retval.y = int_from_bin(bin);
    return retval;
}
