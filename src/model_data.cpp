
// WARNING: Generated code.

#include "model_data.hpp"


message::hex_coord_t ToProtobuf (const hex_coord_t& value)
{
    message::hex_coord_t retval;
    retval.set_x(value.x);
    retval.set_y(value.y);
    return retval;
}

message::capital_hex_zone_t ToProtobuf (const capital_hex_zone_t& value)
{
    message::capital_hex_zone_t retval;
    retval.set_name(value.name);
    for (const auto& x : value.features) {
        retval.add_features(static_cast<message::feature_t>(x));
    }
    return retval;
}

message::capital_hex_t ToProtobuf (const capital_hex_t& value)
{
    message::capital_hex_t retval;
    retval.set_coord(value.coord);
    for (const auto& x : value.zones) {
        retval.add_zones()->CopyFrom(ToProtobuf(x));
    }
    return retval;
}

message::capital_t ToProtobuf (const capital_t& value)
{
    message::capital_t retval;
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(ToProtobuf(x));
    }
    return retval;
}

message::offmap_t ToProtobuf (const offmap_t& value)
{
    message::offmap_t retval;
    retval.set_provinces(value.provinces);
    retval.set_mins(value.mins);
    retval.set_majs(value.majs);
    return retval;
}

message::nation_t ToProtobuf (const nation_t& value)
{
    message::nation_t retval;
    retval.set_name(value.name);
    retval.set_short_name(value.short_name);
    retval.mutable_capital()->CopyFrom(ToProtobuf(value.capital));
    retval.set_free_strategic_moves(value.free_strategic_moves);
    retval.set_capital_star_points(value.capital_star_points);
    retval.mutable_offmap()->CopyFrom(ToProtobuf(value.offmap));
    retval.set_offmap_survey_ships(value.offmap_survey_ships);
    retval.set_nation_id(value.nation_id);
    retval.set_cannot_build_offmap_capital(value.cannot_build_offmap_capital);
    retval.set_old_offmap_shipyard(value.old_offmap_shipyard);
    return retval;
}

message::nations_t ToProtobuf (const nations_t& value)
{
    message::nations_t retval;
    for (const auto& x : value.nations) {
        (*retval.mutable_nations())[x.first] = ToProtobuf(x.second);
    }
    return retval;
}

message::hex_t ToProtobuf (const hex_t& value)
{
    message::hex_t retval;
    retval.mutable_coord()->CopyFrom(ToProtobuf(value.coord));
    retval.set_owner(value.owner);
    retval.set_feature(static_cast<message::feature_t>(value.feature));
    for (const auto& x : value.neutral_zone_bordering) {
        retval.add_neutral_zone_bordering(x);
    }
    return retval;
}

message::province_t ToProtobuf (const province_t& value)
{
    message::province_t retval;
    for (const auto& x : value.hexes) {
        retval.add_hexes(x);
    }
    return retval;
}

message::province_list_t ToProtobuf (const province_list_t& value)
{
    message::province_list_t retval;
    for (const auto& x : value.provinces) {
        retval.add_provinces()->CopyFrom(ToProtobuf(x));
    }
    return retval;
}

message::map_t ToProtobuf (const map_t& value)
{
    message::map_t retval;
    retval.set_width(value.width);
    retval.set_height(value.height);
    for (const auto& x : value.provinces) {
        (*retval.mutable_provinces())[x.first] = ToProtobuf(x.second);
    }
    for (const auto& x : value.hexes) {
        retval.add_hexes()->CopyFrom(ToProtobuf(x));
    }
    return retval;
}

message::model_t ToProtobuf (const model_t& value)
{
    message::model_t retval;
    retval.mutable_nations()->CopyFrom(ToProtobuf(value.nations));
    retval.mutable_map()->CopyFrom(ToProtobuf(value.map));
    return retval;
}

hex_coord_t FromProtobuf (const message::hex_coord_t& msg)
{
    hex_coord_t retval;
    retval.x = msg.x();
    retval.y = msg.y();
    return retval;
}

capital_hex_zone_t FromProtobuf (const message::capital_hex_zone_t& msg)
{
    capital_hex_zone_t retval;
    retval.name = msg.name();
    retval.features.resize(msg.features_size());
    auto it = retval.features.begin();
    for (const auto& x : msg.features()) {
        *it++ = static_cast<feature_t>(x);
    }
    return retval;
}

capital_hex_t FromProtobuf (const message::capital_hex_t& msg)
{
    capital_hex_t retval;
    retval.coord = msg.coord();
    retval.zones.resize(msg.zones_size());
    auto it = retval.zones.begin();
    for (const auto& x : msg.zones()) {
        *it++ = FromProtobuf(x);
    }
    return retval;
}

capital_t FromProtobuf (const message::capital_t& msg)
{
    capital_t retval;
    retval.hexes.resize(msg.hexes_size());
    auto it = retval.hexes.begin();
    for (const auto& x : msg.hexes()) {
        *it++ = FromProtobuf(x);
    }
    return retval;
}

offmap_t FromProtobuf (const message::offmap_t& msg)
{
    offmap_t retval;
    retval.provinces = msg.provinces();
    retval.mins = msg.mins();
    retval.majs = msg.majs();
    return retval;
}

nation_t FromProtobuf (const message::nation_t& msg)
{
    nation_t retval;
    retval.name = msg.name();
    retval.short_name = msg.short_name();
    retval.capital = FromProtobuf(msg.capital());
    retval.free_strategic_moves = msg.free_strategic_moves();
    retval.capital_star_points = msg.capital_star_points();
    retval.offmap = FromProtobuf(msg.offmap());
    retval.offmap_survey_ships = msg.offmap_survey_ships();
    retval.nation_id = msg.nation_id();
    retval.cannot_build_offmap_capital = msg.cannot_build_offmap_capital();
    retval.old_offmap_shipyard = msg.old_offmap_shipyard();
    return retval;
}

nations_t FromProtobuf (const message::nations_t& msg)
{
    nations_t retval;
    for (const auto& x : msg.nations()) {
        retval.nations[x.first] = FromProtobuf(x.second);
    }
    return retval;
}

hex_t FromProtobuf (const message::hex_t& msg)
{
    hex_t retval;
    retval.coord = FromProtobuf(msg.coord());
    retval.owner = msg.owner();
    retval.feature = static_cast<feature_t>(msg.feature());
    retval.neutral_zone_bordering.resize(msg.neutral_zone_bordering_size());
    auto it = retval.neutral_zone_bordering.begin();
    for (const auto& x : msg.neutral_zone_bordering()) {
        *it++ = x;
    }
    return retval;
}

province_t FromProtobuf (const message::province_t& msg)
{
    province_t retval;
    retval.hexes.resize(msg.hexes_size());
    auto it = retval.hexes.begin();
    for (const auto& x : msg.hexes()) {
        *it++ = x;
    }
    return retval;
}

province_list_t FromProtobuf (const message::province_list_t& msg)
{
    province_list_t retval;
    retval.provinces.resize(msg.provinces_size());
    auto it = retval.provinces.begin();
    for (const auto& x : msg.provinces()) {
        *it++ = FromProtobuf(x);
    }
    return retval;
}

map_t FromProtobuf (const message::map_t& msg)
{
    map_t retval;
    retval.width = msg.width();
    retval.height = msg.height();
    for (const auto& x : msg.provinces()) {
        retval.provinces[x.first] = FromProtobuf(x.second);
    }
    retval.hexes.resize(msg.hexes_size());
    auto it = retval.hexes.begin();
    for (const auto& x : msg.hexes()) {
        *it++ = FromProtobuf(x);
    }
    return retval;
}

model_t FromProtobuf (const message::model_t& msg)
{
    model_t retval;
    retval.nations = FromProtobuf(msg.nations());
    retval.map = FromProtobuf(msg.map());
    return retval;
}
