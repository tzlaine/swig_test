// WARNING: Generated code.
// This file was generated from model (proto3)
#pragma once

#include <model.pb.h>
#include <string>
#include <vector>
#include <boost/container/flat_map.hpp>




enum class feature_t {
    no_feature = 0,
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

enum class tug_mission_t {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7,
    I = 8,
    J1 = 9,
    J2 = 10,
    K1 = 11,
    K2 = 12,
    L = 13,
    M = 14,
    N = 15,
    O = 16,
};

enum class carrier_type_t {
    not_a_carrier = 0,
    single_ship_carrier = 1,
    light_carrier = 2,
    medium_carrier = 3,
    heavy_carrier = 4,
};

enum class escort_type_t {
    not_an_escort = 0,
    light_escort = 1,
    heavy_escort = 2,
};

struct turn_t
{
    int year;
    season_t season;
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
    bool cannot_build_offmap_capital;
    bool old_shipyard;
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
    boost::container::flat_map<std::string, nation_t> nations;
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
    boost::container::flat_map<std::string, starting_national_holdings_t> starting_national_holdings;
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
    bool reserve;
    std::vector<production_element_t> prewar_construction;
    int strategic_move_arrival_year;
    season_t strategic_move_arrival_season;
    boost::container::flat_map<int, int> hex_placement_limits;
};

struct mothball_reserve_t
{
    std::vector<oob_unit_t> units;
    std::vector<oob_unit_t> war_release;
    std::vector<oob_unit_t> limited_war_release;
};

struct order_of_battle_t
{
    boost::container::flat_map<std::string, starting_fleet_t> starting_fleets;
    mothball_reserve_t mothball_reserve;
    std::vector<production_element_t> production;
};

struct orders_of_battle_t
{
    boost::container::flat_map<std::string, order_of_battle_t> oobs;
};

struct unit_def_side_t
{
    float att;
    float def;
    bool scout;
    float fighters;
    float heavy_fighter_bonus;
    int pfs;
    int drones;
    bool mauler;
    std::vector<tug_mission_t> tug_missions;
};

struct towable_t
{
    int move_cost;
    int strat_move_limit;
};

struct production_cost_t
{
    int cost;
    int fighter_cost;
};

struct unit_def_t
{
    std::string name;
    int cmd;
    unit_def_side_t uncrippled;
    unit_def_side_t crippled;
    escort_type_t escort_type;
    turn_t available;
    bool pod;
    int max_in_service;
    production_cost_t construction;
    boost::container::flat_map<std::string, production_cost_t> substitutions;
    boost::container::flat_map<std::string, production_cost_t> conversions;
    int move;
    carrier_type_t carrier_type;
    bool not_spaceworthy;
    towable_t towable;
    int salvage;
    std::string notes;
};

struct nation_unit_defs_t
{
    std::vector<unit_def_t> units;
};

struct unit_defs_t
{
    boost::container::flat_map<std::string, nation_unit_defs_t> nation_units;
};

message::turn_t to_protobuf (const ::turn_t& value);
::turn_t from_protobuf (const message::turn_t& msg);

message::hex_coord_t to_protobuf (const ::hex_coord_t& value);
::hex_coord_t from_protobuf (const message::hex_coord_t& msg);

message::capital_hex_zone_t to_protobuf (const ::capital_hex_zone_t& value);
::capital_hex_zone_t from_protobuf (const message::capital_hex_zone_t& msg);

message::capital_hex_t to_protobuf (const ::capital_hex_t& value);
::capital_hex_t from_protobuf (const message::capital_hex_t& msg);

message::capital_t to_protobuf (const ::capital_t& value);
::capital_t from_protobuf (const message::capital_t& msg);

message::offmap_possesions_t to_protobuf (const ::offmap_possesions_t& value);
::offmap_possesions_t from_protobuf (const message::offmap_possesions_t& msg);

message::nation_t to_protobuf (const ::nation_t& value);
::nation_t from_protobuf (const message::nation_t& msg);

message::nations_t to_protobuf (const ::nations_t& value);
::nations_t from_protobuf (const message::nations_t& msg);

message::hex_t to_protobuf (const ::hex_t& value);
::hex_t from_protobuf (const message::hex_t& msg);

message::province_hex_t to_protobuf (const ::province_hex_t& value);
::province_hex_t from_protobuf (const message::province_hex_t& msg);

message::province_t to_protobuf (const ::province_t& value);
::province_t from_protobuf (const message::province_t& msg);

message::offmap_area_t to_protobuf (const ::offmap_area_t& value);
::offmap_area_t from_protobuf (const message::offmap_area_t& msg);

message::starting_national_holdings_t to_protobuf (const ::starting_national_holdings_t& value);
::starting_national_holdings_t from_protobuf (const message::starting_national_holdings_t& msg);

message::map_t to_protobuf (const ::map_t& value);
::map_t from_protobuf (const message::map_t& msg);

message::model_t to_protobuf (const ::model_t& value);
::model_t from_protobuf (const message::model_t& msg);

message::oob_unit_t to_protobuf (const ::oob_unit_t& value);
::oob_unit_t from_protobuf (const message::oob_unit_t& msg);

message::production_element_t to_protobuf (const ::production_element_t& value);
::production_element_t from_protobuf (const message::production_element_t& msg);

message::starting_fleet_t to_protobuf (const ::starting_fleet_t& value);
::starting_fleet_t from_protobuf (const message::starting_fleet_t& msg);

message::mothball_reserve_t to_protobuf (const ::mothball_reserve_t& value);
::mothball_reserve_t from_protobuf (const message::mothball_reserve_t& msg);

message::order_of_battle_t to_protobuf (const ::order_of_battle_t& value);
::order_of_battle_t from_protobuf (const message::order_of_battle_t& msg);

message::orders_of_battle_t to_protobuf (const ::orders_of_battle_t& value);
::orders_of_battle_t from_protobuf (const message::orders_of_battle_t& msg);

message::unit_def_side_t to_protobuf (const ::unit_def_side_t& value);
::unit_def_side_t from_protobuf (const message::unit_def_side_t& msg);

message::towable_t to_protobuf (const ::towable_t& value);
::towable_t from_protobuf (const message::towable_t& msg);

message::production_cost_t to_protobuf (const ::production_cost_t& value);
::production_cost_t from_protobuf (const message::production_cost_t& msg);

message::unit_def_t to_protobuf (const ::unit_def_t& value);
::unit_def_t from_protobuf (const message::unit_def_t& msg);

message::nation_unit_defs_t to_protobuf (const ::nation_unit_defs_t& value);
::nation_unit_defs_t from_protobuf (const message::nation_unit_defs_t& msg);

message::unit_defs_t to_protobuf (const ::unit_defs_t& value);
::unit_defs_t from_protobuf (const message::unit_defs_t& msg);


