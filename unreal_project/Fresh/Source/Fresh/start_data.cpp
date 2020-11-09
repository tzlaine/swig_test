// WARNING: Generated code.
// This file was generated from start_data.proto (proto3)

#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "start_data.hpp"
#include <base_types.hpp>


namespace start_data { 

} 

    pb_message::capital_hex_zone_t to_protobuf (const start_data::capital_hex_zone_t& value)
    {
        pb_message::capital_hex_zone_t retval;
        retval.set_name(value.name.c_str());
        for (const auto& x : value.features) {
            retval.add_features(static_cast< pb_message::feature_t >(x));
        }
        return retval;
    }

    start_data::capital_hex_zone_t from_protobuf (const pb_message::capital_hex_zone_t& msg)
    {
        start_data::capital_hex_zone_t retval;
        retval.name = adobe::name_t(msg.name().c_str());
        {
            retval.features.resize(msg.features_size());
            auto it = retval.features.begin();
            for (const auto& x : msg.features()) {
                *it++ = static_cast<std::remove_reference<decltype(*it++)>::type>(x);
            }
        }
        return retval;
    }

    pb_message::capital_hex_t to_protobuf (const start_data::capital_hex_t& value)
    {
        pb_message::capital_hex_t retval;
        retval.set_coord(value.coord);
        for (const auto& x : value.zones) {
            retval.add_zones()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::capital_hex_t from_protobuf (const pb_message::capital_hex_t& msg)
    {
        start_data::capital_hex_t retval;
        retval.coord = msg.coord();
        {
            retval.zones.resize(msg.zones_size());
            auto it = retval.zones.begin();
            for (const auto& x : msg.zones()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::capital_t to_protobuf (const start_data::capital_t& value)
    {
        pb_message::capital_t retval;
        for (const auto& x : value.hexes) {
            retval.add_hexes()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::capital_t from_protobuf (const pb_message::capital_t& msg)
    {
        start_data::capital_t retval;
        {
            retval.hexes.resize(msg.hexes_size());
            auto it = retval.hexes.begin();
            for (const auto& x : msg.hexes()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::offmap_possesions_t to_protobuf (const start_data::offmap_possesions_t& value)
    {
        pb_message::offmap_possesions_t retval;
        retval.set_provinces(value.provinces);
        retval.set_mins(value.mins);
        retval.set_majs(value.majs);
        retval.set_survey_ships(value.survey_ships);
        retval.set_cannot_build_offmap_capital(value.cannot_build_offmap_capital);
        retval.set_old_shipyard(value.old_shipyard);
        return retval;
    }

    start_data::offmap_possesions_t from_protobuf (const pb_message::offmap_possesions_t& msg)
    {
        start_data::offmap_possesions_t retval;
        retval.provinces = msg.provinces();
        retval.mins = msg.mins();
        retval.majs = msg.majs();
        retval.survey_ships = msg.survey_ships();
        retval.cannot_build_offmap_capital = msg.cannot_build_offmap_capital();
        retval.old_shipyard = msg.old_shipyard();
        return retval;
    }

    pb_message::nation_t to_protobuf (const start_data::nation_t& value)
    {
        pb_message::nation_t retval;
        retval.set_name(value.name.c_str());
        retval.set_short_name(value.short_name.c_str());
        retval.mutable_capital()->CopyFrom(to_protobuf(value.capital));
        retval.set_free_strategic_moves(value.free_strategic_moves);
        retval.set_capital_star_points(value.capital_star_points);
        retval.mutable_offmap_possesions()->CopyFrom(to_protobuf(value.offmap_possesions));
        retval.set_nation_id(value.nation_id);
        return retval;
    }

    start_data::nation_t from_protobuf (const pb_message::nation_t& msg)
    {
        start_data::nation_t retval;
        retval.name = adobe::name_t(msg.name().c_str());
        retval.short_name = adobe::name_t(msg.short_name().c_str());
        retval.capital = from_protobuf(msg.capital());
        retval.free_strategic_moves = msg.free_strategic_moves();
        retval.capital_star_points = msg.capital_star_points();
        retval.offmap_possesions = from_protobuf(msg.offmap_possesions());
        retval.nation_id = msg.nation_id();
        return retval;
    }

    pb_message::nations_t to_protobuf (const start_data::nations_t& value)
    {
        pb_message::nations_t retval;
        for (const auto& x : value.nations) {
            (*retval.mutable_nations())[x.first.c_str()] = to_protobuf(x.second);
        }
        return retval;
    }

    start_data::nations_t from_protobuf (const pb_message::nations_t& msg)
    {
        start_data::nations_t retval;
        {
            for (const auto& x : msg.nations()) {
                retval.nations[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        return retval;
    }

    pb_message::hex_t to_protobuf (const start_data::hex_t& value)
    {
        pb_message::hex_t retval;
        retval.mutable_coord()->CopyFrom(to_protobuf(value.coord));
        retval.set_owner(value.owner);
        retval.set_feature(static_cast< pb_message::feature_t >(value.feature));
        return retval;
    }

    start_data::hex_t from_protobuf (const pb_message::hex_t& msg)
    {
        start_data::hex_t retval;
        retval.coord = from_protobuf(msg.coord());
        retval.owner = msg.owner();
        retval.feature = static_cast<std::remove_reference<decltype(retval.feature)>::type>(msg.feature());
        return retval;
    }

    pb_message::province_hex_t to_protobuf (const start_data::province_hex_t& value)
    {
        pb_message::province_hex_t retval;
        retval.set_hex(value.hex);
        retval.set_feature(static_cast< pb_message::feature_t >(value.feature));
        return retval;
    }

    start_data::province_hex_t from_protobuf (const pb_message::province_hex_t& msg)
    {
        start_data::province_hex_t retval;
        retval.hex = msg.hex();
        retval.feature = static_cast<std::remove_reference<decltype(retval.feature)>::type>(msg.feature());
        return retval;
    }

    pb_message::province_t to_protobuf (const start_data::province_t& value)
    {
        pb_message::province_t retval;
        for (const auto& x : value.hexes) {
            retval.add_hexes()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::province_t from_protobuf (const pb_message::province_t& msg)
    {
        start_data::province_t retval;
        {
            retval.hexes.resize(msg.hexes_size());
            auto it = retval.hexes.begin();
            for (const auto& x : msg.hexes()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::offmap_area_t to_protobuf (const start_data::offmap_area_t& value)
    {
        pb_message::offmap_area_t retval;
        retval.set_name(value.name.c_str());
        for (const auto& x : value.features) {
            retval.add_features(static_cast< pb_message::feature_t >(x));
        }
        for (const auto& x : value.feature_hexes) {
            retval.add_feature_hexes(x);
        }
        for (const auto& x : value.adjacent_hexes) {
            retval.add_adjacent_hexes(x);
        }
        return retval;
    }

    start_data::offmap_area_t from_protobuf (const pb_message::offmap_area_t& msg)
    {
        start_data::offmap_area_t retval;
        retval.name = adobe::name_t(msg.name().c_str());
        {
            retval.features.resize(msg.features_size());
            auto it = retval.features.begin();
            for (const auto& x : msg.features()) {
                *it++ = static_cast<std::remove_reference<decltype(*it++)>::type>(x);
            }
        }
        {
            retval.feature_hexes.resize(msg.feature_hexes_size());
            auto it = retval.feature_hexes.begin();
            for (const auto& x : msg.feature_hexes()) {
                *it++ = x;
            }
        }
        {
            retval.adjacent_hexes.resize(msg.adjacent_hexes_size());
            auto it = retval.adjacent_hexes.begin();
            for (const auto& x : msg.adjacent_hexes()) {
                *it++ = x;
            }
        }
        return retval;
    }

    pb_message::starting_national_holdings_t to_protobuf (const start_data::starting_national_holdings_t& value)
    {
        pb_message::starting_national_holdings_t retval;
        for (const auto& x : value.provinces) {
            retval.add_provinces()->CopyFrom(to_protobuf(x));
        }
        retval.mutable_offmap_area()->CopyFrom(to_protobuf(value.offmap_area));
        return retval;
    }

    start_data::starting_national_holdings_t from_protobuf (const pb_message::starting_national_holdings_t& msg)
    {
        start_data::starting_national_holdings_t retval;
        {
            retval.provinces.resize(msg.provinces_size());
            auto it = retval.provinces.begin();
            for (const auto& x : msg.provinces()) {
                *it++ = from_protobuf(x);
            }
        }
        retval.offmap_area = from_protobuf(msg.offmap_area());
        return retval;
    }

    pb_message::map_t to_protobuf (const start_data::map_t& value)
    {
        pb_message::map_t retval;
        retval.set_width(value.width);
        retval.set_height(value.height);
        for (const auto& x : value.nz_planets) {
            retval.add_nz_planets(x);
        }
        for (const auto& x : value.nz_hexes) {
            retval.add_nz_hexes(x);
        }
        for (const auto& x : value.starting_national_holdings) {
            (*retval.mutable_starting_national_holdings())[x.first.c_str()] = to_protobuf(x.second);
        }
        for (const auto& x : value.hexes) {
            retval.add_hexes()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::map_t from_protobuf (const pb_message::map_t& msg)
    {
        start_data::map_t retval;
        retval.width = msg.width();
        retval.height = msg.height();
        {
            retval.nz_planets.resize(msg.nz_planets_size());
            auto it = retval.nz_planets.begin();
            for (const auto& x : msg.nz_planets()) {
                *it++ = x;
            }
        }
        {
            retval.nz_hexes.resize(msg.nz_hexes_size());
            auto it = retval.nz_hexes.begin();
            for (const auto& x : msg.nz_hexes()) {
                *it++ = x;
            }
        }
        {
            for (const auto& x : msg.starting_national_holdings()) {
                retval.starting_national_holdings[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        {
            retval.hexes.resize(msg.hexes_size());
            auto it = retval.hexes.begin();
            for (const auto& x : msg.hexes()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::model_t to_protobuf (const start_data::model_t& value)
    {
        pb_message::model_t retval;
        retval.mutable_nations()->CopyFrom(to_protobuf(value.nations));
        retval.mutable_map()->CopyFrom(to_protobuf(value.map));
        return retval;
    }

    start_data::model_t from_protobuf (const pb_message::model_t& msg)
    {
        start_data::model_t retval;
        retval.nations = from_protobuf(msg.nations());
        retval.map = from_protobuf(msg.map());
        return retval;
    }

    pb_message::oob_unit_t to_protobuf (const start_data::oob_unit_t& value)
    {
        pb_message::oob_unit_t retval;
        retval.set_unit(value.unit.c_str());
        retval.set_times(value.times);
        return retval;
    }

    start_data::oob_unit_t from_protobuf (const pb_message::oob_unit_t& msg)
    {
        start_data::oob_unit_t retval;
        retval.unit = adobe::name_t(msg.unit().c_str());
        retval.times = msg.times();
        return retval;
    }

    pb_message::production_element_t to_protobuf (const start_data::production_element_t& value)
    {
        pb_message::production_element_t retval;
        retval.set_year(value.year);
        retval.set_season(static_cast< pb_message::season_t >(value.season));
        for (const auto& x : value.units) {
            retval.add_units()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::production_element_t from_protobuf (const pb_message::production_element_t& msg)
    {
        start_data::production_element_t retval;
        retval.year = msg.year();
        retval.season = static_cast<std::remove_reference<decltype(retval.season)>::type>(msg.season());
        {
            retval.units.resize(msg.units_size());
            auto it = retval.units.begin();
            for (const auto& x : msg.units()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::starting_fleet_t to_protobuf (const start_data::starting_fleet_t& value)
    {
        pb_message::starting_fleet_t retval;
        for (const auto& x : value.hexes) {
            retval.add_hexes(x);
        }
        for (const auto& x : value.units) {
            retval.add_units()->CopyFrom(to_protobuf(x));
        }
        retval.set_reserve(value.reserve);
        for (const auto& x : value.prewar_construction) {
            retval.add_prewar_construction()->CopyFrom(to_protobuf(x));
        }
        retval.set_strategic_move_arrival_year(value.strategic_move_arrival_year);
        retval.set_strategic_move_arrival_season(static_cast< pb_message::season_t >(value.strategic_move_arrival_season));
        for (const auto& x : value.hex_placement_limits) {
            (*retval.mutable_hex_placement_limits())[x.first] = x.second;
        }
        return retval;
    }

    start_data::starting_fleet_t from_protobuf (const pb_message::starting_fleet_t& msg)
    {
        start_data::starting_fleet_t retval;
        {
            retval.hexes.resize(msg.hexes_size());
            auto it = retval.hexes.begin();
            for (const auto& x : msg.hexes()) {
                *it++ = x;
            }
        }
        {
            retval.units.resize(msg.units_size());
            auto it = retval.units.begin();
            for (const auto& x : msg.units()) {
                *it++ = from_protobuf(x);
            }
        }
        retval.reserve = msg.reserve();
        {
            retval.prewar_construction.resize(msg.prewar_construction_size());
            auto it = retval.prewar_construction.begin();
            for (const auto& x : msg.prewar_construction()) {
                *it++ = from_protobuf(x);
            }
        }
        retval.strategic_move_arrival_year = msg.strategic_move_arrival_year();
        retval.strategic_move_arrival_season = static_cast<std::remove_reference<decltype(retval.strategic_move_arrival_season)>::type>(msg.strategic_move_arrival_season());
        {
            for (const auto& x : msg.hex_placement_limits()) {
                retval.hex_placement_limits[x.first] = x.second;
            }
        }
        return retval;
    }

    pb_message::mothball_reserve_t to_protobuf (const start_data::mothball_reserve_t& value)
    {
        pb_message::mothball_reserve_t retval;
        for (const auto& x : value.units) {
            retval.add_units()->CopyFrom(to_protobuf(x));
        }
        for (const auto& x : value.war_release) {
            retval.add_war_release()->CopyFrom(to_protobuf(x));
        }
        for (const auto& x : value.limited_war_release) {
            retval.add_limited_war_release()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::mothball_reserve_t from_protobuf (const pb_message::mothball_reserve_t& msg)
    {
        start_data::mothball_reserve_t retval;
        {
            retval.units.resize(msg.units_size());
            auto it = retval.units.begin();
            for (const auto& x : msg.units()) {
                *it++ = from_protobuf(x);
            }
        }
        {
            retval.war_release.resize(msg.war_release_size());
            auto it = retval.war_release.begin();
            for (const auto& x : msg.war_release()) {
                *it++ = from_protobuf(x);
            }
        }
        {
            retval.limited_war_release.resize(msg.limited_war_release_size());
            auto it = retval.limited_war_release.begin();
            for (const auto& x : msg.limited_war_release()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::order_of_battle_t to_protobuf (const start_data::order_of_battle_t& value)
    {
        pb_message::order_of_battle_t retval;
        for (const auto& x : value.starting_fleets) {
            (*retval.mutable_starting_fleets())[x.first.c_str()] = to_protobuf(x.second);
        }
        retval.mutable_mothball_reserve()->CopyFrom(to_protobuf(value.mothball_reserve));
        for (const auto& x : value.production) {
            retval.add_production()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::order_of_battle_t from_protobuf (const pb_message::order_of_battle_t& msg)
    {
        start_data::order_of_battle_t retval;
        {
            for (const auto& x : msg.starting_fleets()) {
                retval.starting_fleets[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        retval.mothball_reserve = from_protobuf(msg.mothball_reserve());
        {
            retval.production.resize(msg.production_size());
            auto it = retval.production.begin();
            for (const auto& x : msg.production()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::orders_of_battle_t to_protobuf (const start_data::orders_of_battle_t& value)
    {
        pb_message::orders_of_battle_t retval;
        for (const auto& x : value.oobs) {
            (*retval.mutable_oobs())[x.first.c_str()] = to_protobuf(x.second);
        }
        return retval;
    }

    start_data::orders_of_battle_t from_protobuf (const pb_message::orders_of_battle_t& msg)
    {
        start_data::orders_of_battle_t retval;
        {
            for (const auto& x : msg.oobs()) {
                retval.oobs[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        return retval;
    }

    pb_message::unit_def_side_t to_protobuf (const start_data::unit_def_side_t& value)
    {
        pb_message::unit_def_side_t retval;
        retval.set_att(value.att);
        retval.set_def(value.def);
        retval.set_scout(value.scout);
        retval.set_fighters(value.fighters);
        retval.set_heavy_fighter_bonus(value.heavy_fighter_bonus);
        retval.set_pfs(value.pfs);
        retval.set_drones(value.drones);
        retval.set_mauler(value.mauler);
        for (const auto& x : value.tug_missions) {
            retval.add_tug_missions(static_cast< pb_message::tug_mission_t >(x));
        }
        return retval;
    }

    start_data::unit_def_side_t from_protobuf (const pb_message::unit_def_side_t& msg)
    {
        start_data::unit_def_side_t retval;
        retval.att = msg.att();
        retval.def = msg.def();
        retval.scout = msg.scout();
        retval.fighters = msg.fighters();
        retval.heavy_fighter_bonus = msg.heavy_fighter_bonus();
        retval.pfs = msg.pfs();
        retval.drones = msg.drones();
        retval.mauler = msg.mauler();
        {
            retval.tug_missions.resize(msg.tug_missions_size());
            auto it = retval.tug_missions.begin();
            for (const auto& x : msg.tug_missions()) {
                *it++ = static_cast<std::remove_reference<decltype(*it++)>::type>(x);
            }
        }
        return retval;
    }

    pb_message::towable_t to_protobuf (const start_data::towable_t& value)
    {
        pb_message::towable_t retval;
        retval.set_move_cost(value.move_cost);
        retval.set_strat_move_limit(value.strat_move_limit);
        return retval;
    }

    start_data::towable_t from_protobuf (const pb_message::towable_t& msg)
    {
        start_data::towable_t retval;
        retval.move_cost = msg.move_cost();
        retval.strat_move_limit = msg.strat_move_limit();
        return retval;
    }

    pb_message::production_cost_t to_protobuf (const start_data::production_cost_t& value)
    {
        pb_message::production_cost_t retval;
        retval.set_cost(value.cost);
        retval.set_fighter_cost(value.fighter_cost);
        return retval;
    }

    start_data::production_cost_t from_protobuf (const pb_message::production_cost_t& msg)
    {
        start_data::production_cost_t retval;
        retval.cost = msg.cost();
        retval.fighter_cost = msg.fighter_cost();
        return retval;
    }

    pb_message::unit_def_t to_protobuf (const start_data::unit_def_t& value)
    {
        pb_message::unit_def_t retval;
        retval.set_name(value.name.c_str());
        retval.set_cmd(value.cmd);
        retval.mutable_uncrippled()->CopyFrom(to_protobuf(value.uncrippled));
        retval.mutable_crippled()->CopyFrom(to_protobuf(value.crippled));
        retval.set_escort_type(static_cast< pb_message::escort_type_t >(value.escort_type));
        retval.mutable_available()->CopyFrom(to_protobuf(value.available));
        retval.set_pod(value.pod);
        retval.set_max_in_service(value.max_in_service);
        retval.mutable_construction()->CopyFrom(to_protobuf(value.construction));
        for (const auto& x : value.substitutions) {
            (*retval.mutable_substitutions())[x.first.c_str()] = to_protobuf(x.second);
        }
        for (const auto& x : value.conversions) {
            (*retval.mutable_conversions())[x.first.c_str()] = to_protobuf(x.second);
        }
        retval.set_move(value.move);
        retval.set_carrier_type(static_cast< pb_message::carrier_type_t >(value.carrier_type));
        retval.set_not_spaceworthy(value.not_spaceworthy);
        retval.mutable_towable()->CopyFrom(to_protobuf(value.towable));
        retval.set_salvage(value.salvage);
        retval.set_notes(value.notes.c_str());
        return retval;
    }

    start_data::unit_def_t from_protobuf (const pb_message::unit_def_t& msg)
    {
        start_data::unit_def_t retval;
        retval.name = adobe::name_t(msg.name().c_str());
        retval.cmd = msg.cmd();
        retval.uncrippled = from_protobuf(msg.uncrippled());
        retval.crippled = from_protobuf(msg.crippled());
        retval.escort_type = static_cast<std::remove_reference<decltype(retval.escort_type)>::type>(msg.escort_type());
        retval.available = from_protobuf(msg.available());
        retval.pod = msg.pod();
        retval.max_in_service = msg.max_in_service();
        retval.construction = from_protobuf(msg.construction());
        {
            for (const auto& x : msg.substitutions()) {
                retval.substitutions[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        {
            for (const auto& x : msg.conversions()) {
                retval.conversions[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        retval.move = msg.move();
        retval.carrier_type = static_cast<std::remove_reference<decltype(retval.carrier_type)>::type>(msg.carrier_type());
        retval.not_spaceworthy = msg.not_spaceworthy();
        retval.towable = from_protobuf(msg.towable());
        retval.salvage = msg.salvage();
        retval.notes = adobe::name_t(msg.notes().c_str());
        return retval;
    }

    pb_message::nation_unit_defs_t to_protobuf (const start_data::nation_unit_defs_t& value)
    {
        pb_message::nation_unit_defs_t retval;
        for (const auto& x : value.units) {
            retval.add_units()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::nation_unit_defs_t from_protobuf (const pb_message::nation_unit_defs_t& msg)
    {
        start_data::nation_unit_defs_t retval;
        {
            retval.units.resize(msg.units_size());
            auto it = retval.units.begin();
            for (const auto& x : msg.units()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::unit_defs_t to_protobuf (const start_data::unit_defs_t& value)
    {
        pb_message::unit_defs_t retval;
        for (const auto& x : value.nation_units) {
            (*retval.mutable_nation_units())[x.first.c_str()] = to_protobuf(x.second);
        }
        return retval;
    }

    start_data::unit_defs_t from_protobuf (const pb_message::unit_defs_t& msg)
    {
        start_data::unit_defs_t retval;
        {
            for (const auto& x : msg.nation_units()) {
                retval.nation_units[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        return retval;
    }

    pb_message::team_t to_protobuf (const start_data::team_t& value)
    {
        pb_message::team_t retval;
        retval.set_name(value.name.c_str());
        for (const auto& x : value.nations) {
            retval.add_nations(x.c_str());
        }
        return retval;
    }

    start_data::team_t from_protobuf (const pb_message::team_t& msg)
    {
        start_data::team_t retval;
        retval.name = adobe::name_t(msg.name().c_str());
        {
            retval.nations.resize(msg.nations_size());
            auto it = retval.nations.begin();
            for (const auto& x : msg.nations()) {
                *it++ = adobe::name_t(x.c_str());
            }
        }
        return retval;
    }

    pb_message::scenario_condition_t::object_t to_protobuf (const start_data::scenario_condition_t::object_t& value)
    {
        pb_message::scenario_condition_t::object_t retval;
        retval.set_type(static_cast< pb_message::scenario_condition_t::object_type_t >(value.type));
        for (const auto& x : value.names) {
            retval.add_names(x.c_str());
        }
        for (const auto& x : value.hexes) {
            retval.add_hexes(x);
        }
        return retval;
    }

    start_data::scenario_condition_t::object_t from_protobuf (const pb_message::scenario_condition_t::object_t& msg)
    {
        start_data::scenario_condition_t::object_t retval;
        retval.type = static_cast<std::remove_reference<decltype(retval.type)>::type>(msg.type());
        {
            retval.names.resize(msg.names_size());
            auto it = retval.names.begin();
            for (const auto& x : msg.names()) {
                *it++ = adobe::name_t(x.c_str());
            }
        }
        {
            retval.hexes.resize(msg.hexes_size());
            auto it = retval.hexes.begin();
            for (const auto& x : msg.hexes()) {
                *it++ = x;
            }
        }
        return retval;
    }

    pb_message::scenario_condition_t to_protobuf (const start_data::scenario_condition_t& value)
    {
        pb_message::scenario_condition_t retval;
        for (const auto& x : value.actors) {
            retval.add_actors(x.c_str());
        }
        retval.set_action(static_cast< pb_message::scenario_condition_t::action_t >(value.action));
        for (const auto& x : value.one_of) {
            retval.add_one_of()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::scenario_condition_t from_protobuf (const pb_message::scenario_condition_t& msg)
    {
        start_data::scenario_condition_t retval;
        {
            retval.actors.resize(msg.actors_size());
            auto it = retval.actors.begin();
            for (const auto& x : msg.actors()) {
                *it++ = adobe::name_t(x.c_str());
            }
        }
        retval.action = static_cast<std::remove_reference<decltype(retval.action)>::type>(msg.action());
        {
            retval.one_of.resize(msg.one_of_size());
            auto it = retval.one_of.begin();
            for (const auto& x : msg.one_of()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::fleet_release_condition_t to_protobuf (const start_data::fleet_release_condition_t& value)
    {
        pb_message::fleet_release_condition_t retval;
        retval.set_fleet(value.fleet.c_str());
        retval.mutable_condition()->CopyFrom(to_protobuf(value.condition));
        return retval;
    }

    start_data::fleet_release_condition_t from_protobuf (const pb_message::fleet_release_condition_t& msg)
    {
        start_data::fleet_release_condition_t retval;
        retval.fleet = adobe::name_t(msg.fleet().c_str());
        retval.condition = from_protobuf(msg.condition());
        return retval;
    }

    pb_message::war_entry_condition_t to_protobuf (const start_data::war_entry_condition_t& value)
    {
        pb_message::war_entry_condition_t retval;
        retval.set_economy(static_cast< pb_message::war_footing_t >(value.economy));
        retval.mutable_condition()->CopyFrom(to_protobuf(value.condition));
        return retval;
    }

    start_data::war_entry_condition_t from_protobuf (const pb_message::war_entry_condition_t& msg)
    {
        start_data::war_entry_condition_t retval;
        retval.economy = static_cast<std::remove_reference<decltype(retval.economy)>::type>(msg.economy());
        retval.condition = from_protobuf(msg.condition());
        return retval;
    }

    pb_message::scenario_turn_t::national_action_t::action_t to_protobuf (const start_data::scenario_turn_t::national_action_t::action_t& value)
    {
        pb_message::scenario_turn_t::national_action_t::action_t retval;
        retval.set_type(static_cast< pb_message::scenario_turn_t::national_action_t::action_type_t >(value.type));
        for (const auto& x : value.names) {
            retval.add_names(x.c_str());
        }
        return retval;
    }

    start_data::scenario_turn_t::national_action_t::action_t from_protobuf (const pb_message::scenario_turn_t::national_action_t::action_t& msg)
    {
        start_data::scenario_turn_t::national_action_t::action_t retval;
        retval.type = static_cast<std::remove_reference<decltype(retval.type)>::type>(msg.type());
        {
            retval.names.resize(msg.names_size());
            auto it = retval.names.begin();
            for (const auto& x : msg.names()) {
                *it++ = adobe::name_t(x.c_str());
            }
        }
        return retval;
    }

    pb_message::scenario_turn_t::national_action_t to_protobuf (const start_data::scenario_turn_t::national_action_t& value)
    {
        pb_message::scenario_turn_t::national_action_t retval;
        for (const auto& x : value.actions) {
            retval.add_actions()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::scenario_turn_t::national_action_t from_protobuf (const pb_message::scenario_turn_t::national_action_t& msg)
    {
        start_data::scenario_turn_t::national_action_t retval;
        {
            retval.actions.resize(msg.actions_size());
            auto it = retval.actions.begin();
            for (const auto& x : msg.actions()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::scenario_turn_t to_protobuf (const start_data::scenario_turn_t& value)
    {
        pb_message::scenario_turn_t retval;
        retval.set_turn(value.turn);
        for (const auto& x : value.national_actions) {
            (*retval.mutable_national_actions())[x.first.c_str()] = to_protobuf(x.second);
        }
        return retval;
    }

    start_data::scenario_turn_t from_protobuf (const pb_message::scenario_turn_t& msg)
    {
        start_data::scenario_turn_t retval;
        retval.turn = msg.turn();
        {
            for (const auto& x : msg.national_actions()) {
                retval.national_actions[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        return retval;
    }

    pb_message::scenario_t::nation_t to_protobuf (const start_data::scenario_t::nation_t& value)
    {
        pb_message::scenario_t::nation_t retval;
        for (const auto& x : value.at_war_with) {
            retval.add_at_war_with(x.c_str());
        }
        for (const auto& x : value.future_belligerents) {
            retval.add_future_belligerents(x.c_str());
        }
        retval.set_economy(static_cast< pb_message::war_footing_t >(value.economy));
        retval.set_exhaustion_turns(value.exhaustion_turns);
        retval.set_accumulate_exhaustion_at_peace(value.accumulate_exhaustion_at_peace);
        for (const auto& x : value.release_conditions) {
            retval.add_release_conditions()->CopyFrom(to_protobuf(x));
        }
        for (const auto& x : value.war_entry_conditions) {
            retval.add_war_entry_conditions()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::scenario_t::nation_t from_protobuf (const pb_message::scenario_t::nation_t& msg)
    {
        start_data::scenario_t::nation_t retval;
        {
            retval.at_war_with.resize(msg.at_war_with_size());
            auto it = retval.at_war_with.begin();
            for (const auto& x : msg.at_war_with()) {
                *it++ = adobe::name_t(x.c_str());
            }
        }
        {
            retval.future_belligerents.resize(msg.future_belligerents_size());
            auto it = retval.future_belligerents.begin();
            for (const auto& x : msg.future_belligerents()) {
                *it++ = adobe::name_t(x.c_str());
            }
        }
        retval.economy = static_cast<std::remove_reference<decltype(retval.economy)>::type>(msg.economy());
        retval.exhaustion_turns = msg.exhaustion_turns();
        retval.accumulate_exhaustion_at_peace = msg.accumulate_exhaustion_at_peace();
        {
            retval.release_conditions.resize(msg.release_conditions_size());
            auto it = retval.release_conditions.begin();
            for (const auto& x : msg.release_conditions()) {
                *it++ = from_protobuf(x);
            }
        }
        {
            retval.war_entry_conditions.resize(msg.war_entry_conditions_size());
            auto it = retval.war_entry_conditions.begin();
            for (const auto& x : msg.war_entry_conditions()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }

    pb_message::scenario_t to_protobuf (const start_data::scenario_t& value)
    {
        pb_message::scenario_t retval;
        retval.set_name(value.name.c_str());
        retval.set_description(value.description.c_str());
        retval.mutable_start_turn()->CopyFrom(to_protobuf(value.start_turn));
        for (const auto& x : value.teams) {
            retval.add_teams()->CopyFrom(to_protobuf(x));
        }
        for (const auto& x : value.team_turn_order) {
            retval.add_team_turn_order(x.c_str());
        }
        retval.set_map(value.map.c_str());
        retval.set_order_of_battle(value.order_of_battle.c_str());
        for (const auto& x : value.setup_order) {
            retval.add_setup_order(x.c_str());
        }
        for (const auto& x : value.nations) {
            (*retval.mutable_nations())[x.first.c_str()] = to_protobuf(x.second);
        }
        for (const auto& x : value.turns) {
            retval.add_turns()->CopyFrom(to_protobuf(x));
        }
        return retval;
    }

    start_data::scenario_t from_protobuf (const pb_message::scenario_t& msg)
    {
        start_data::scenario_t retval;
        retval.name = adobe::name_t(msg.name().c_str());
        retval.description = adobe::name_t(msg.description().c_str());
        retval.start_turn = from_protobuf(msg.start_turn());
        {
            retval.teams.resize(msg.teams_size());
            auto it = retval.teams.begin();
            for (const auto& x : msg.teams()) {
                *it++ = from_protobuf(x);
            }
        }
        {
            retval.team_turn_order.resize(msg.team_turn_order_size());
            auto it = retval.team_turn_order.begin();
            for (const auto& x : msg.team_turn_order()) {
                *it++ = adobe::name_t(x.c_str());
            }
        }
        retval.map = adobe::name_t(msg.map().c_str());
        retval.order_of_battle = adobe::name_t(msg.order_of_battle().c_str());
        {
            retval.setup_order.resize(msg.setup_order_size());
            auto it = retval.setup_order.begin();
            for (const auto& x : msg.setup_order()) {
                *it++ = adobe::name_t(x.c_str());
            }
        }
        {
            for (const auto& x : msg.nations()) {
                retval.nations[adobe::name_t(x.first.c_str())] = from_protobuf(x.second);
            }
        }
        {
            retval.turns.resize(msg.turns_size());
            auto it = retval.turns.begin();
            for (const auto& x : msg.turns()) {
                *it++ = from_protobuf(x);
            }
        }
        return retval;
    }
