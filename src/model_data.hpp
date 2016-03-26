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

enum class season_t {
    spring = 0,
    fall = 1,
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
    int coord;
    std::vector<capital_hex_zone_t> zones;
};

struct capital_t
{
    std::vector<capital_hex_t> hexes;
};

struct offmap_possesions_t
{
    int provinces;
    int mins;
    int majs;
    int survey_ships;
    int cannot_build_offmap_capital;
    int old_shipyard;
};

struct nation_t
{
    std::string name;
    std::string short_name;
    capital_t capital;
    int free_strategic_moves;
    int capital_star_points;
    offmap_possesions_t offmap_possesions;
    int nation_id;
};

struct nations_t
{
    std::map<std::string, nation_t> nations;
};

struct hex_t
{
    hex_coord_t coord;
    int owner;
    feature_t feature;
    std::vector<int> neutral_zone_bordering;
};

struct province_hex_t
{
    int hex;
    feature_t feature;
};

struct province_t
{
    std::vector<province_hex_t> hexes;
};

struct offmap_area_t
{
    std::string name;
    std::vector<feature_t> features;
    int counter_hex;
    std::vector<int> adjacent_hexes;
};

struct starting_national_holdings_t
{
    std::vector<province_t> provinces;
    offmap_area_t offmap_area;
};

struct map_t
{
    int width;
    int height;
    std::vector<int> nz_planets;
    std::vector<int> nz_hexes;
    std::map<std::string, starting_national_holdings_t> starting_national_holdings;
    std::vector<hex_t> hexes;
};

struct model_t
{
    nations_t nations;
    map_t map;
};

struct oob_unit_t
{
    std::string unit;
    int times;
};

struct production_element_t
{
    int year;
    season_t season;
    std::vector<oob_unit_t> units;
};

struct starting_fleet_t
{
    std::vector<int> hexes;
    std::vector<oob_unit_t> units;
    int reserve;
    std::vector<production_element_t> prewar_construction;
    int strategic_move_arrival_year;
    season_t strategic_move_arrival_season;
    std::map<int, int> hex_placement_limits;
};

struct mothball_reserve_t
{
    std::vector<oob_unit_t> units;
    std::vector<oob_unit_t> war_release;
    std::vector<oob_unit_t> limited_war_release;
};

struct order_of_battle_t
{
    std::map<std::string, starting_fleet_t> starting_fleets;
    mothball_reserve_t mothball_reserve;
    std::vector<production_element_t> production;
};

struct orders_of_battle_t
{
    std::map<std::string, order_of_battle_t> oobs;
};

message::hex_coord_t ToProtobuf (const hex_coord_t& value);
hex_coord_t FromProtobuf (const message::hex_coord_t& msg);

message::capital_hex_zone_t ToProtobuf (const capital_hex_zone_t& value);
capital_hex_zone_t FromProtobuf (const message::capital_hex_zone_t& msg);

message::capital_hex_t ToProtobuf (const capital_hex_t& value);
capital_hex_t FromProtobuf (const message::capital_hex_t& msg);

message::capital_t ToProtobuf (const capital_t& value);
capital_t FromProtobuf (const message::capital_t& msg);

message::offmap_possesions_t ToProtobuf (const offmap_possesions_t& value);
offmap_possesions_t FromProtobuf (const message::offmap_possesions_t& msg);

message::nation_t ToProtobuf (const nation_t& value);
nation_t FromProtobuf (const message::nation_t& msg);

message::nations_t ToProtobuf (const nations_t& value);
nations_t FromProtobuf (const message::nations_t& msg);

message::hex_t ToProtobuf (const hex_t& value);
hex_t FromProtobuf (const message::hex_t& msg);

message::province_hex_t ToProtobuf (const province_hex_t& value);
province_hex_t FromProtobuf (const message::province_hex_t& msg);

message::province_t ToProtobuf (const province_t& value);
province_t FromProtobuf (const message::province_t& msg);

message::offmap_area_t ToProtobuf (const offmap_area_t& value);
offmap_area_t FromProtobuf (const message::offmap_area_t& msg);

message::starting_national_holdings_t ToProtobuf (const starting_national_holdings_t& value);
starting_national_holdings_t FromProtobuf (const message::starting_national_holdings_t& msg);

message::map_t ToProtobuf (const map_t& value);
map_t FromProtobuf (const message::map_t& msg);

message::model_t ToProtobuf (const model_t& value);
model_t FromProtobuf (const message::model_t& msg);

message::oob_unit_t ToProtobuf (const oob_unit_t& value);
oob_unit_t FromProtobuf (const message::oob_unit_t& msg);

message::production_element_t ToProtobuf (const production_element_t& value);
production_element_t FromProtobuf (const message::production_element_t& msg);

message::starting_fleet_t ToProtobuf (const starting_fleet_t& value);
starting_fleet_t FromProtobuf (const message::starting_fleet_t& msg);

message::mothball_reserve_t ToProtobuf (const mothball_reserve_t& value);
mothball_reserve_t FromProtobuf (const message::mothball_reserve_t& msg);

message::order_of_battle_t ToProtobuf (const order_of_battle_t& value);
order_of_battle_t FromProtobuf (const message::order_of_battle_t& msg);

message::orders_of_battle_t ToProtobuf (const orders_of_battle_t& value);
orders_of_battle_t FromProtobuf (const message::orders_of_battle_t& msg);
