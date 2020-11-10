// WARNING: Generated code.
// This file was generated from start_data (proto3)
#pragma once

#include <start_data.pb.h>
#include <base_types.hpp>

#include <string>
#include <vector>
#include <adobe/name.hpp>
#include <boost/container/flat_map.hpp>


namespace start_data { 

    enum class feature_t {
        no_feature = 0,
        bats = 1,
        sb = 2,
        min = 3,
        maj = 4,
        capital = 5,
    };

    struct capital_hex_zone_t
    {
        adobe::name_t name;
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
        adobe::name_t name;
        adobe::name_t short_name;
        capital_t capital;
        int free_strategic_moves;
        int capital_star_points;
        offmap_possesions_t offmap_possesions;
        int nation_id;
    };

    struct nations_t
    {
        boost::container::flat_map<adobe::name_t, nation_t> nations;
    };

    struct hex_t
    {
        hex_coord_t coord;
        int owner;
        feature_t feature;
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
        adobe::name_t name;
        std::vector<feature_t> features;
        std::vector<int> feature_hexes;
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
        boost::container::flat_map<adobe::name_t, starting_national_holdings_t> starting_national_holdings;
        std::vector<hex_t> hexes;
    };

    struct model_t
    {
        nations_t nations;
        map_t map;
    };

    struct oob_unit_t
    {
        adobe::name_t unit;
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
        boost::container::flat_map<adobe::name_t, starting_fleet_t> starting_fleets;
        mothball_reserve_t mothball_reserve;
        std::vector<production_element_t> production;
    };

    struct orders_of_battle_t
    {
        boost::container::flat_map<adobe::name_t, order_of_battle_t> oobs;
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
        adobe::name_t name;
        int cmd;
        unit_def_side_t uncrippled;
        unit_def_side_t crippled;
        escort_type_t escort_type;
        turn_t available;
        bool pod;
        int max_in_service;
        production_cost_t construction;
        boost::container::flat_map<adobe::name_t, production_cost_t> substitutions;
        boost::container::flat_map<adobe::name_t, production_cost_t> conversions;
        int move;
        carrier_type_t carrier_type;
        bool not_spaceworthy;
        towable_t towable;
        int salvage;
        adobe::name_t notes;
    };

    struct nation_unit_defs_t
    {
        std::vector<unit_def_t> units;
    };

    struct unit_defs_t
    {
        boost::container::flat_map<adobe::name_t, nation_unit_defs_t> nation_units;
    };

    struct team_t
    {
        adobe::name_t name;
        std::vector<adobe::name_t> nations;
    };

    struct scenario_condition_t
    {

        enum class action_t {
            occupies = 0,
            destroys = 1,
        };

        enum class object_type_t {
            hexes = 0,
            sb = 1,
            fleet_area = 2,
            nation = 3,
        };

        struct object_t
        {
            scenario_condition_t::object_type_t type;
            std::vector<adobe::name_t> names;
            std::vector<int> hexes;
        };
        std::vector<adobe::name_t> actors;
        scenario_condition_t::action_t action;
        std::vector<scenario_condition_t::object_t> one_of;
    };

    struct fleet_release_condition_t
    {
        adobe::name_t fleet;
        scenario_condition_t condition;
    };

    struct war_entry_condition_t
    {
        war_footing_t economy;
        scenario_condition_t condition;
    };

    struct scenario_turn_t
    {

        struct national_action_t
        {

            enum class action_type_t {
                release_fleets = 0,
                declare_war = 1,
            };

            struct action_t
            {
                scenario_turn_t::national_action_t::action_type_t type;
                std::vector<adobe::name_t> names;
            };
            std::vector<scenario_turn_t::national_action_t::action_t> actions;
        };
        int turn;
        boost::container::flat_map<adobe::name_t, scenario_turn_t::national_action_t> national_actions;
    };

    struct scenario_t
    {

        struct nation_t
        {
            std::vector<adobe::name_t> at_war_with;
            std::vector<adobe::name_t> future_belligerents;
            war_footing_t economy;
            int exhaustion_turns;
            bool accumulate_exhaustion_at_peace;
            std::vector<fleet_release_condition_t> release_conditions;
            std::vector<war_entry_condition_t> war_entry_conditions;
        };
        adobe::name_t name;
        adobe::name_t description;
        turn_t start_turn;
        std::vector<team_t> teams;
        std::vector<adobe::name_t> team_turn_order;
        adobe::name_t map;
        adobe::name_t order_of_battle;
        std::vector<adobe::name_t> setup_order;
        boost::container::flat_map<adobe::name_t, scenario_t::nation_t> nations;
        std::vector<scenario_turn_t> turns;
    };

} 

    pb_message::capital_hex_zone_t to_protobuf (const start_data::capital_hex_zone_t& value);
    start_data::capital_hex_zone_t from_protobuf (const pb_message::capital_hex_zone_t& msg);

    pb_message::capital_hex_t to_protobuf (const start_data::capital_hex_t& value);
    start_data::capital_hex_t from_protobuf (const pb_message::capital_hex_t& msg);

    pb_message::capital_t to_protobuf (const start_data::capital_t& value);
    start_data::capital_t from_protobuf (const pb_message::capital_t& msg);

    pb_message::offmap_possesions_t to_protobuf (const start_data::offmap_possesions_t& value);
    start_data::offmap_possesions_t from_protobuf (const pb_message::offmap_possesions_t& msg);

    pb_message::nation_t to_protobuf (const start_data::nation_t& value);
    start_data::nation_t from_protobuf (const pb_message::nation_t& msg);

    pb_message::nations_t to_protobuf (const start_data::nations_t& value);
    start_data::nations_t from_protobuf (const pb_message::nations_t& msg);

    pb_message::hex_t to_protobuf (const start_data::hex_t& value);
    start_data::hex_t from_protobuf (const pb_message::hex_t& msg);

    pb_message::province_hex_t to_protobuf (const start_data::province_hex_t& value);
    start_data::province_hex_t from_protobuf (const pb_message::province_hex_t& msg);

    pb_message::province_t to_protobuf (const start_data::province_t& value);
    start_data::province_t from_protobuf (const pb_message::province_t& msg);

    pb_message::offmap_area_t to_protobuf (const start_data::offmap_area_t& value);
    start_data::offmap_area_t from_protobuf (const pb_message::offmap_area_t& msg);

    pb_message::starting_national_holdings_t to_protobuf (const start_data::starting_national_holdings_t& value);
    start_data::starting_national_holdings_t from_protobuf (const pb_message::starting_national_holdings_t& msg);

    pb_message::map_t to_protobuf (const start_data::map_t& value);
    start_data::map_t from_protobuf (const pb_message::map_t& msg);

    pb_message::model_t to_protobuf (const start_data::model_t& value);
    start_data::model_t from_protobuf (const pb_message::model_t& msg);

    pb_message::oob_unit_t to_protobuf (const start_data::oob_unit_t& value);
    start_data::oob_unit_t from_protobuf (const pb_message::oob_unit_t& msg);

    pb_message::production_element_t to_protobuf (const start_data::production_element_t& value);
    start_data::production_element_t from_protobuf (const pb_message::production_element_t& msg);

    pb_message::starting_fleet_t to_protobuf (const start_data::starting_fleet_t& value);
    start_data::starting_fleet_t from_protobuf (const pb_message::starting_fleet_t& msg);

    pb_message::mothball_reserve_t to_protobuf (const start_data::mothball_reserve_t& value);
    start_data::mothball_reserve_t from_protobuf (const pb_message::mothball_reserve_t& msg);

    pb_message::order_of_battle_t to_protobuf (const start_data::order_of_battle_t& value);
    start_data::order_of_battle_t from_protobuf (const pb_message::order_of_battle_t& msg);

    pb_message::orders_of_battle_t to_protobuf (const start_data::orders_of_battle_t& value);
    start_data::orders_of_battle_t from_protobuf (const pb_message::orders_of_battle_t& msg);

    pb_message::unit_def_side_t to_protobuf (const start_data::unit_def_side_t& value);
    start_data::unit_def_side_t from_protobuf (const pb_message::unit_def_side_t& msg);

    pb_message::towable_t to_protobuf (const start_data::towable_t& value);
    start_data::towable_t from_protobuf (const pb_message::towable_t& msg);

    pb_message::production_cost_t to_protobuf (const start_data::production_cost_t& value);
    start_data::production_cost_t from_protobuf (const pb_message::production_cost_t& msg);

    pb_message::unit_def_t to_protobuf (const start_data::unit_def_t& value);
    start_data::unit_def_t from_protobuf (const pb_message::unit_def_t& msg);

    pb_message::nation_unit_defs_t to_protobuf (const start_data::nation_unit_defs_t& value);
    start_data::nation_unit_defs_t from_protobuf (const pb_message::nation_unit_defs_t& msg);

    pb_message::unit_defs_t to_protobuf (const start_data::unit_defs_t& value);
    start_data::unit_defs_t from_protobuf (const pb_message::unit_defs_t& msg);

    pb_message::team_t to_protobuf (const start_data::team_t& value);
    start_data::team_t from_protobuf (const pb_message::team_t& msg);

    pb_message::scenario_condition_t::object_t to_protobuf (const start_data::scenario_condition_t::object_t& value);
    start_data::scenario_condition_t::object_t from_protobuf (const pb_message::scenario_condition_t::object_t& msg);

    pb_message::scenario_condition_t to_protobuf (const start_data::scenario_condition_t& value);
    start_data::scenario_condition_t from_protobuf (const pb_message::scenario_condition_t& msg);

    pb_message::fleet_release_condition_t to_protobuf (const start_data::fleet_release_condition_t& value);
    start_data::fleet_release_condition_t from_protobuf (const pb_message::fleet_release_condition_t& msg);

    pb_message::war_entry_condition_t to_protobuf (const start_data::war_entry_condition_t& value);
    start_data::war_entry_condition_t from_protobuf (const pb_message::war_entry_condition_t& msg);

    pb_message::scenario_turn_t::national_action_t::action_t to_protobuf (const start_data::scenario_turn_t::national_action_t::action_t& value);
    start_data::scenario_turn_t::national_action_t::action_t from_protobuf (const pb_message::scenario_turn_t::national_action_t::action_t& msg);

    pb_message::scenario_turn_t::national_action_t to_protobuf (const start_data::scenario_turn_t::national_action_t& value);
    start_data::scenario_turn_t::national_action_t from_protobuf (const pb_message::scenario_turn_t::national_action_t& msg);

    pb_message::scenario_turn_t to_protobuf (const start_data::scenario_turn_t& value);
    start_data::scenario_turn_t from_protobuf (const pb_message::scenario_turn_t& msg);

    pb_message::scenario_t::nation_t to_protobuf (const start_data::scenario_t::nation_t& value);
    start_data::scenario_t::nation_t from_protobuf (const pb_message::scenario_t::nation_t& msg);

    pb_message::scenario_t to_protobuf (const start_data::scenario_t& value);
    start_data::scenario_t from_protobuf (const pb_message::scenario_t& msg);
