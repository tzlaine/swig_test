#pragma once

// WARNING: Generated code.

#include <model.pb.h>
#include <string>
#include <vector>


enum class feature_t {
    none = 0,
    bats = 1,
    sb = 2,
    min = 3,
    maj = 4,
    capital = 5,
};

struct hex_coord_t
{
    int x;
    int y;
};

struct capital_hex_zone_t
{
    std::string name;
    std::vector<feature_t> features;
};

struct capital_hex_t
{
    hex_coord_t coord;
    std::vector<capital_hex_zone_t> zones;
};

struct capital_t
{
    std::vector<capital_hex_t> hexes;
};

struct offmap_t
{
    int provinces;
    int mins;
    int majs;
};

struct nation_t
{
    std::string short_name;
    capital_t capital;
    int free_strat_moves;
    int capital_star_points;
    offmap_t offmap;
    int offmap_survey_ships;
    int nation_id;
};

struct nations_t
{
    std::map<std::string, nation_t> nations;
};

struct hex_t
{
    hex_coord_t coord;
    unsigned int owner;
    feature_t feature;
    std::vector<int> neutral_zone_bordering;
};

struct province_t
{
    std::vector<int> hexes;
};

struct province_list_t
{
    std::vector<province_t> provinces;
};

struct map_t
{
    int width;
    int height;
    std::map<int, province_list_t> provinces;
    std::vector<hex_t> hexes;
};

struct model_t
{
    nations_t nations;
    map_t map;
};

message::hex_coord_t ToProtobuf (const hex_coord_t& value);
hex_coord_t FromProtobuf (const message::hex_coord_t& msg);

message::capital_hex_zone_t ToProtobuf (const capital_hex_zone_t& value);
capital_hex_zone_t FromProtobuf (const message::capital_hex_zone_t& msg);

message::capital_hex_t ToProtobuf (const capital_hex_t& value);
capital_hex_t FromProtobuf (const message::capital_hex_t& msg);

message::capital_t ToProtobuf (const capital_t& value);
capital_t FromProtobuf (const message::capital_t& msg);

message::offmap_t ToProtobuf (const offmap_t& value);
offmap_t FromProtobuf (const message::offmap_t& msg);

message::nation_t ToProtobuf (const nation_t& value);
nation_t FromProtobuf (const message::nation_t& msg);

message::nations_t ToProtobuf (const nations_t& value);
nations_t FromProtobuf (const message::nations_t& msg);

message::hex_t ToProtobuf (const hex_t& value);
hex_t FromProtobuf (const message::hex_t& msg);

message::province_t ToProtobuf (const province_t& value);
province_t FromProtobuf (const message::province_t& msg);

message::province_list_t ToProtobuf (const province_list_t& value);
province_list_t FromProtobuf (const message::province_list_t& msg);

message::map_t ToProtobuf (const map_t& value);
map_t FromProtobuf (const message::map_t& msg);

message::model_t ToProtobuf (const model_t& value);
model_t FromProtobuf (const message::model_t& msg);
