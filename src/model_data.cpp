// WARNING: Generated code.
// This file was generated from model.proto (proto3)

#include "model_data.hpp"




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

message::capital_hex_zone_t to_protobuf (const ::capital_hex_zone_t& value)
{
    message::capital_hex_zone_t retval;
    retval.set_name(value.name);
    for (const auto& x : value.features) {
        retval.add_features(static_cast< message::feature_t >(x));
    }
    return retval;
}

::capital_hex_zone_t from_protobuf (const message::capital_hex_zone_t& msg)
{
    ::capital_hex_zone_t retval;
    retval.name = msg.name();
    {
        retval.features.resize(msg.features_size());
        auto it = retval.features.begin();
        for (const auto& x : msg.features()) {
            *it++ = static_cast<feature_t>(x);
        }
    }
    return retval;
}

message::capital_hex_t to_protobuf (const ::capital_hex_t& value)
{
    message::capital_hex_t retval;
    retval.set_coord(value.coord);
    for (const auto& x : value.zones) {
        retval.add_zones()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::capital_hex_t from_protobuf (const message::capital_hex_t& msg)
{
    ::capital_hex_t retval;
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

message::capital_t to_protobuf (const ::capital_t& value)
{
    message::capital_t retval;
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::capital_t from_protobuf (const message::capital_t& msg)
{
    ::capital_t retval;
    {
        retval.hexes.resize(msg.hexes_size());
        auto it = retval.hexes.begin();
        for (const auto& x : msg.hexes()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

message::offmap_possesions_t to_protobuf (const ::offmap_possesions_t& value)
{
    message::offmap_possesions_t retval;
    retval.set_provinces(value.provinces);
    retval.set_mins(value.mins);
    retval.set_majs(value.majs);
    retval.set_survey_ships(value.survey_ships);
    retval.set_cannot_build_offmap_capital(value.cannot_build_offmap_capital);
    retval.set_old_shipyard(value.old_shipyard);
    return retval;
}

::offmap_possesions_t from_protobuf (const message::offmap_possesions_t& msg)
{
    ::offmap_possesions_t retval;
    retval.provinces = msg.provinces();
    retval.mins = msg.mins();
    retval.majs = msg.majs();
    retval.survey_ships = msg.survey_ships();
    retval.cannot_build_offmap_capital = msg.cannot_build_offmap_capital();
    retval.old_shipyard = msg.old_shipyard();
    return retval;
}

message::nation_t to_protobuf (const ::nation_t& value)
{
    message::nation_t retval;
    retval.set_name(value.name);
    retval.set_short_name(value.short_name);
    retval.mutable_capital()->CopyFrom(to_protobuf(value.capital));
    retval.set_free_strategic_moves(value.free_strategic_moves);
    retval.set_capital_star_points(value.capital_star_points);
    retval.mutable_offmap_possesions()->CopyFrom(to_protobuf(value.offmap_possesions));
    retval.set_nation_id(value.nation_id);
    return retval;
}

::nation_t from_protobuf (const message::nation_t& msg)
{
    ::nation_t retval;
    retval.name = msg.name();
    retval.short_name = msg.short_name();
    retval.capital = from_protobuf(msg.capital());
    retval.free_strategic_moves = msg.free_strategic_moves();
    retval.capital_star_points = msg.capital_star_points();
    retval.offmap_possesions = from_protobuf(msg.offmap_possesions());
    retval.nation_id = msg.nation_id();
    return retval;
}

message::nations_t to_protobuf (const ::nations_t& value)
{
    message::nations_t retval;
    for (const auto& x : value.nations) {
        (*retval.mutable_nations())[x.first] = to_protobuf(x.second);
    }
    return retval;
}

::nations_t from_protobuf (const message::nations_t& msg)
{
    ::nations_t retval;
    {
        for (const auto& x : msg.nations()) {
            retval.nations[x.first] = from_protobuf(x.second);
        }
    }
    return retval;
}

message::hex_t to_protobuf (const ::hex_t& value)
{
    message::hex_t retval;
    retval.mutable_coord()->CopyFrom(to_protobuf(value.coord));
    retval.set_owner(value.owner);
    retval.set_feature(static_cast< message::feature_t >(value.feature));
    for (const auto& x : value.neutral_zone_bordering) {
        retval.add_neutral_zone_bordering(x);
    }
    return retval;
}

::hex_t from_protobuf (const message::hex_t& msg)
{
    ::hex_t retval;
    retval.coord = from_protobuf(msg.coord());
    retval.owner = msg.owner();
    retval.feature = static_cast< feature_t >(msg.feature());
    {
        retval.neutral_zone_bordering.resize(msg.neutral_zone_bordering_size());
        auto it = retval.neutral_zone_bordering.begin();
        for (const auto& x : msg.neutral_zone_bordering()) {
            *it++ = x;
        }
    }
    return retval;
}

message::province_hex_t to_protobuf (const ::province_hex_t& value)
{
    message::province_hex_t retval;
    retval.set_hex(value.hex);
    retval.set_feature(static_cast< message::feature_t >(value.feature));
    return retval;
}

::province_hex_t from_protobuf (const message::province_hex_t& msg)
{
    ::province_hex_t retval;
    retval.hex = msg.hex();
    retval.feature = static_cast< feature_t >(msg.feature());
    return retval;
}

message::province_t to_protobuf (const ::province_t& value)
{
    message::province_t retval;
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::province_t from_protobuf (const message::province_t& msg)
{
    ::province_t retval;
    {
        retval.hexes.resize(msg.hexes_size());
        auto it = retval.hexes.begin();
        for (const auto& x : msg.hexes()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

message::offmap_area_t to_protobuf (const ::offmap_area_t& value)
{
    message::offmap_area_t retval;
    retval.set_name(value.name);
    for (const auto& x : value.features) {
        retval.add_features(static_cast< message::feature_t >(x));
    }
    retval.set_counter_hex(value.counter_hex);
    for (const auto& x : value.adjacent_hexes) {
        retval.add_adjacent_hexes(x);
    }
    return retval;
}

::offmap_area_t from_protobuf (const message::offmap_area_t& msg)
{
    ::offmap_area_t retval;
    retval.name = msg.name();
    {
        retval.features.resize(msg.features_size());
        auto it = retval.features.begin();
        for (const auto& x : msg.features()) {
            *it++ = static_cast<feature_t>(x);
        }
    }
    retval.counter_hex = msg.counter_hex();
    {
        retval.adjacent_hexes.resize(msg.adjacent_hexes_size());
        auto it = retval.adjacent_hexes.begin();
        for (const auto& x : msg.adjacent_hexes()) {
            *it++ = x;
        }
    }
    return retval;
}

message::starting_national_holdings_t to_protobuf (const ::starting_national_holdings_t& value)
{
    message::starting_national_holdings_t retval;
    for (const auto& x : value.provinces) {
        retval.add_provinces()->CopyFrom(to_protobuf(x));
    }
    retval.mutable_offmap_area()->CopyFrom(to_protobuf(value.offmap_area));
    return retval;
}

::starting_national_holdings_t from_protobuf (const message::starting_national_holdings_t& msg)
{
    ::starting_national_holdings_t retval;
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

message::map_t to_protobuf (const ::map_t& value)
{
    message::map_t retval;
    retval.set_width(value.width);
    retval.set_height(value.height);
    for (const auto& x : value.nz_planets) {
        retval.add_nz_planets(x);
    }
    for (const auto& x : value.nz_hexes) {
        retval.add_nz_hexes(x);
    }
    for (const auto& x : value.starting_national_holdings) {
        (*retval.mutable_starting_national_holdings())[x.first] = to_protobuf(x.second);
    }
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::map_t from_protobuf (const message::map_t& msg)
{
    ::map_t retval;
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
            retval.starting_national_holdings[x.first] = from_protobuf(x.second);
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

message::model_t to_protobuf (const ::model_t& value)
{
    message::model_t retval;
    retval.mutable_nations()->CopyFrom(to_protobuf(value.nations));
    retval.mutable_map()->CopyFrom(to_protobuf(value.map));
    return retval;
}

::model_t from_protobuf (const message::model_t& msg)
{
    ::model_t retval;
    retval.nations = from_protobuf(msg.nations());
    retval.map = from_protobuf(msg.map());
    return retval;
}

message::oob_unit_t to_protobuf (const ::oob_unit_t& value)
{
    message::oob_unit_t retval;
    retval.set_unit(value.unit);
    retval.set_times(value.times);
    return retval;
}

::oob_unit_t from_protobuf (const message::oob_unit_t& msg)
{
    ::oob_unit_t retval;
    retval.unit = msg.unit();
    retval.times = msg.times();
    return retval;
}

message::production_element_t to_protobuf (const ::production_element_t& value)
{
    message::production_element_t retval;
    retval.set_year(value.year);
    retval.set_season(static_cast< message::season_t >(value.season));
    for (const auto& x : value.units) {
        retval.add_units()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::production_element_t from_protobuf (const message::production_element_t& msg)
{
    ::production_element_t retval;
    retval.year = msg.year();
    retval.season = static_cast< season_t >(msg.season());
    {
        retval.units.resize(msg.units_size());
        auto it = retval.units.begin();
        for (const auto& x : msg.units()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

message::starting_fleet_t to_protobuf (const ::starting_fleet_t& value)
{
    message::starting_fleet_t retval;
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
    retval.set_strategic_move_arrival_season(static_cast< message::season_t >(value.strategic_move_arrival_season));
    for (const auto& x : value.hex_placement_limits) {
        (*retval.mutable_hex_placement_limits())[x.first] = x.second;
    }
    return retval;
}

::starting_fleet_t from_protobuf (const message::starting_fleet_t& msg)
{
    ::starting_fleet_t retval;
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
    retval.strategic_move_arrival_season = static_cast< season_t >(msg.strategic_move_arrival_season());
    {
        for (const auto& x : msg.hex_placement_limits()) {
            retval.hex_placement_limits[x.first] = x.second;
        }
    }
    return retval;
}

message::mothball_reserve_t to_protobuf (const ::mothball_reserve_t& value)
{
    message::mothball_reserve_t retval;
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

::mothball_reserve_t from_protobuf (const message::mothball_reserve_t& msg)
{
    ::mothball_reserve_t retval;
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

message::order_of_battle_t to_protobuf (const ::order_of_battle_t& value)
{
    message::order_of_battle_t retval;
    for (const auto& x : value.starting_fleets) {
        (*retval.mutable_starting_fleets())[x.first] = to_protobuf(x.second);
    }
    retval.mutable_mothball_reserve()->CopyFrom(to_protobuf(value.mothball_reserve));
    for (const auto& x : value.production) {
        retval.add_production()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::order_of_battle_t from_protobuf (const message::order_of_battle_t& msg)
{
    ::order_of_battle_t retval;
    {
        for (const auto& x : msg.starting_fleets()) {
            retval.starting_fleets[x.first] = from_protobuf(x.second);
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

message::orders_of_battle_t to_protobuf (const ::orders_of_battle_t& value)
{
    message::orders_of_battle_t retval;
    for (const auto& x : value.oobs) {
        (*retval.mutable_oobs())[x.first] = to_protobuf(x.second);
    }
    return retval;
}

::orders_of_battle_t from_protobuf (const message::orders_of_battle_t& msg)
{
    ::orders_of_battle_t retval;
    {
        for (const auto& x : msg.oobs()) {
            retval.oobs[x.first] = from_protobuf(x.second);
        }
    }
    return retval;
}


