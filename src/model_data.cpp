// WARNING: Generated code.
// This file was generated from model.proto (proto3)

#include "model_data.hpp"




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
    retval.season = static_cast< season_t >(msg.season());
    return retval;
}

void to_bin (const ::turn_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.year, bin);
    to_bin(value.season, bin);
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

void to_bin (const ::capital_hex_zone_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.name, bin);
    {
        int length = static_cast<int>(value.features.size());
        to_bin(length, bin);
        for (const auto& x : value.features) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::capital_hex_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.coord, bin);
    {
        int length = static_cast<int>(value.zones.size());
        to_bin(length, bin);
        for (const auto& x : value.zones) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::capital_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.hexes) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::offmap_possesions_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.provinces, bin);
    to_bin(value.mins, bin);
    to_bin(value.majs, bin);
    to_bin(value.survey_ships, bin);
    to_bin(value.cannot_build_offmap_capital, bin);
    to_bin(value.old_shipyard, bin);
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

void to_bin (const ::nation_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.name, bin);
    to_bin(value.short_name, bin);
    to_bin(value.capital, bin);
    to_bin(value.free_strategic_moves, bin);
    to_bin(value.capital_star_points, bin);
    to_bin(value.offmap_possesions, bin);
    to_bin(value.nation_id, bin);
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

void to_bin (const ::nations_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.nations.size());
        to_bin(length, bin);
        for (const auto& x : value.nations) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
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

void to_bin (const ::hex_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.coord, bin);
    to_bin(value.owner, bin);
    to_bin(value.feature, bin);
    {
        int length = static_cast<int>(value.neutral_zone_bordering.size());
        to_bin(length, bin);
        for (const auto& x : value.neutral_zone_bordering) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::province_hex_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.hex, bin);
    to_bin(value.feature, bin);
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

void to_bin (const ::province_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.hexes) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::offmap_area_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.name, bin);
    {
        int length = static_cast<int>(value.features.size());
        to_bin(length, bin);
        for (const auto& x : value.features) {
            to_bin(x, bin);
        }
    }
    to_bin(value.counter_hex, bin);
    {
        int length = static_cast<int>(value.adjacent_hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.adjacent_hexes) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::starting_national_holdings_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.provinces.size());
        to_bin(length, bin);
        for (const auto& x : value.provinces) {
            to_bin(x, bin);
        }
    }
    to_bin(value.offmap_area, bin);
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

void to_bin (const ::map_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.width, bin);
    to_bin(value.height, bin);
    {
        int length = static_cast<int>(value.nz_planets.size());
        to_bin(length, bin);
        for (const auto& x : value.nz_planets) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.nz_hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.nz_hexes) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.starting_national_holdings.size());
        to_bin(length, bin);
        for (const auto& x : value.starting_national_holdings) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
    {
        int length = static_cast<int>(value.hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.hexes) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::model_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.nations, bin);
    to_bin(value.map, bin);
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

void to_bin (const ::oob_unit_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.unit, bin);
    to_bin(value.times, bin);
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

void to_bin (const ::production_element_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.year, bin);
    to_bin(value.season, bin);
    {
        int length = static_cast<int>(value.units.size());
        to_bin(length, bin);
        for (const auto& x : value.units) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::starting_fleet_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.hexes) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.units.size());
        to_bin(length, bin);
        for (const auto& x : value.units) {
            to_bin(x, bin);
        }
    }
    to_bin(value.reserve, bin);
    {
        int length = static_cast<int>(value.prewar_construction.size());
        to_bin(length, bin);
        for (const auto& x : value.prewar_construction) {
            to_bin(x, bin);
        }
    }
    to_bin(value.strategic_move_arrival_year, bin);
    to_bin(value.strategic_move_arrival_season, bin);
    {
        int length = static_cast<int>(value.hex_placement_limits.size());
        to_bin(length, bin);
        for (const auto& x : value.hex_placement_limits) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
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

void to_bin (const ::mothball_reserve_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.units.size());
        to_bin(length, bin);
        for (const auto& x : value.units) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.war_release.size());
        to_bin(length, bin);
        for (const auto& x : value.war_release) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.limited_war_release.size());
        to_bin(length, bin);
        for (const auto& x : value.limited_war_release) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::order_of_battle_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.starting_fleets.size());
        to_bin(length, bin);
        for (const auto& x : value.starting_fleets) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
    to_bin(value.mothball_reserve, bin);
    {
        int length = static_cast<int>(value.production.size());
        to_bin(length, bin);
        for (const auto& x : value.production) {
            to_bin(x, bin);
        }
    }
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

void to_bin (const ::orders_of_battle_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.oobs.size());
        to_bin(length, bin);
        for (const auto& x : value.oobs) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
}

message::unit_def_side_t to_protobuf (const ::unit_def_side_t& value)
{
    message::unit_def_side_t retval;
    retval.set_att(value.att);
    retval.set_def(value.def);
    retval.set_scout(value.scout);
    retval.set_fighters(value.fighters);
    retval.set_heavy_fighter_bonus(value.heavy_fighter_bonus);
    retval.set_pfs(value.pfs);
    retval.set_drones(value.drones);
    retval.set_mauler(value.mauler);
    for (const auto& x : value.tug_missions) {
        retval.add_tug_missions(static_cast< message::tug_mission_t >(x));
    }
    return retval;
}

::unit_def_side_t from_protobuf (const message::unit_def_side_t& msg)
{
    ::unit_def_side_t retval;
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
            *it++ = static_cast<tug_mission_t>(x);
        }
    }
    return retval;
}

void to_bin (const ::unit_def_side_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.att, bin);
    to_bin(value.def, bin);
    to_bin(value.scout, bin);
    to_bin(value.fighters, bin);
    to_bin(value.heavy_fighter_bonus, bin);
    to_bin(value.pfs, bin);
    to_bin(value.drones, bin);
    to_bin(value.mauler, bin);
    {
        int length = static_cast<int>(value.tug_missions.size());
        to_bin(length, bin);
        for (const auto& x : value.tug_missions) {
            to_bin(x, bin);
        }
    }
}

message::towable_t to_protobuf (const ::towable_t& value)
{
    message::towable_t retval;
    retval.set_move_cost(value.move_cost);
    retval.set_strat_move_limit(value.strat_move_limit);
    return retval;
}

::towable_t from_protobuf (const message::towable_t& msg)
{
    ::towable_t retval;
    retval.move_cost = msg.move_cost();
    retval.strat_move_limit = msg.strat_move_limit();
    return retval;
}

void to_bin (const ::towable_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.move_cost, bin);
    to_bin(value.strat_move_limit, bin);
}

message::production_cost_t to_protobuf (const ::production_cost_t& value)
{
    message::production_cost_t retval;
    retval.set_cost(value.cost);
    retval.set_fighter_cost(value.fighter_cost);
    return retval;
}

::production_cost_t from_protobuf (const message::production_cost_t& msg)
{
    ::production_cost_t retval;
    retval.cost = msg.cost();
    retval.fighter_cost = msg.fighter_cost();
    return retval;
}

void to_bin (const ::production_cost_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.cost, bin);
    to_bin(value.fighter_cost, bin);
}

message::unit_def_t to_protobuf (const ::unit_def_t& value)
{
    message::unit_def_t retval;
    retval.set_name(value.name);
    retval.set_cmd(value.cmd);
    retval.mutable_uncrippled()->CopyFrom(to_protobuf(value.uncrippled));
    retval.mutable_crippled()->CopyFrom(to_protobuf(value.crippled));
    retval.set_escort_type(static_cast< message::escort_type_t >(value.escort_type));
    retval.mutable_available()->CopyFrom(to_protobuf(value.available));
    retval.set_pod(value.pod);
    retval.set_max_in_service(value.max_in_service);
    retval.mutable_construction()->CopyFrom(to_protobuf(value.construction));
    for (const auto& x : value.substitutions) {
        (*retval.mutable_substitutions())[x.first] = to_protobuf(x.second);
    }
    for (const auto& x : value.conversions) {
        (*retval.mutable_conversions())[x.first] = to_protobuf(x.second);
    }
    retval.set_move(value.move);
    retval.set_carrier_type(static_cast< message::carrier_type_t >(value.carrier_type));
    retval.set_not_spaceworthy(value.not_spaceworthy);
    retval.mutable_towable()->CopyFrom(to_protobuf(value.towable));
    retval.set_salvage(value.salvage);
    retval.set_notes(value.notes);
    return retval;
}

::unit_def_t from_protobuf (const message::unit_def_t& msg)
{
    ::unit_def_t retval;
    retval.name = msg.name();
    retval.cmd = msg.cmd();
    retval.uncrippled = from_protobuf(msg.uncrippled());
    retval.crippled = from_protobuf(msg.crippled());
    retval.escort_type = static_cast< escort_type_t >(msg.escort_type());
    retval.available = from_protobuf(msg.available());
    retval.pod = msg.pod();
    retval.max_in_service = msg.max_in_service();
    retval.construction = from_protobuf(msg.construction());
    {
        for (const auto& x : msg.substitutions()) {
            retval.substitutions[x.first] = from_protobuf(x.second);
        }
    }
    {
        for (const auto& x : msg.conversions()) {
            retval.conversions[x.first] = from_protobuf(x.second);
        }
    }
    retval.move = msg.move();
    retval.carrier_type = static_cast< carrier_type_t >(msg.carrier_type());
    retval.not_spaceworthy = msg.not_spaceworthy();
    retval.towable = from_protobuf(msg.towable());
    retval.salvage = msg.salvage();
    retval.notes = msg.notes();
    return retval;
}

void to_bin (const ::unit_def_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.name, bin);
    to_bin(value.cmd, bin);
    to_bin(value.uncrippled, bin);
    to_bin(value.crippled, bin);
    to_bin(value.escort_type, bin);
    to_bin(value.available, bin);
    to_bin(value.pod, bin);
    to_bin(value.max_in_service, bin);
    to_bin(value.construction, bin);
    {
        int length = static_cast<int>(value.substitutions.size());
        to_bin(length, bin);
        for (const auto& x : value.substitutions) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
    {
        int length = static_cast<int>(value.conversions.size());
        to_bin(length, bin);
        for (const auto& x : value.conversions) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
    to_bin(value.move, bin);
    to_bin(value.carrier_type, bin);
    to_bin(value.not_spaceworthy, bin);
    to_bin(value.towable, bin);
    to_bin(value.salvage, bin);
    to_bin(value.notes, bin);
}

message::nation_unit_defs_t to_protobuf (const ::nation_unit_defs_t& value)
{
    message::nation_unit_defs_t retval;
    for (const auto& x : value.units) {
        retval.add_units()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::nation_unit_defs_t from_protobuf (const message::nation_unit_defs_t& msg)
{
    ::nation_unit_defs_t retval;
    {
        retval.units.resize(msg.units_size());
        auto it = retval.units.begin();
        for (const auto& x : msg.units()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

void to_bin (const ::nation_unit_defs_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.units.size());
        to_bin(length, bin);
        for (const auto& x : value.units) {
            to_bin(x, bin);
        }
    }
}

message::unit_defs_t to_protobuf (const ::unit_defs_t& value)
{
    message::unit_defs_t retval;
    for (const auto& x : value.nation_units) {
        (*retval.mutable_nation_units())[x.first] = to_protobuf(x.second);
    }
    return retval;
}

::unit_defs_t from_protobuf (const message::unit_defs_t& msg)
{
    ::unit_defs_t retval;
    {
        for (const auto& x : msg.nation_units()) {
            retval.nation_units[x.first] = from_protobuf(x.second);
        }
    }
    return retval;
}

void to_bin (const ::unit_defs_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.nation_units.size());
        to_bin(length, bin);
        for (const auto& x : value.nation_units) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
}

message::team_t to_protobuf (const ::team_t& value)
{
    message::team_t retval;
    retval.set_name(value.name);
    for (const auto& x : value.nations) {
        retval.add_nations(x);
    }
    return retval;
}

::team_t from_protobuf (const message::team_t& msg)
{
    ::team_t retval;
    retval.name = msg.name();
    {
        retval.nations.resize(msg.nations_size());
        auto it = retval.nations.begin();
        for (const auto& x : msg.nations()) {
            *it++ = x;
        }
    }
    return retval;
}

void to_bin (const ::team_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.name, bin);
    {
        int length = static_cast<int>(value.nations.size());
        to_bin(length, bin);
        for (const auto& x : value.nations) {
            to_bin(x, bin);
        }
    }
}

message::scenario_condition_t::object_t to_protobuf (const ::scenario_condition_t::object_t& value)
{
    message::scenario_condition_t::object_t retval;
    retval.set_type(static_cast< message::scenario_condition_t::object_type_t >(value.type));
    for (const auto& x : value.names) {
        retval.add_names(x);
    }
    for (const auto& x : value.hexes) {
        retval.add_hexes(x);
    }
    return retval;
}

::scenario_condition_t::object_t from_protobuf (const message::scenario_condition_t::object_t& msg)
{
    ::scenario_condition_t::object_t retval;
    retval.type = static_cast< scenario_condition_t::object_type_t >(msg.type());
    {
        retval.names.resize(msg.names_size());
        auto it = retval.names.begin();
        for (const auto& x : msg.names()) {
            *it++ = x;
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

void to_bin (const ::scenario_condition_t::object_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.type, bin);
    {
        int length = static_cast<int>(value.names.size());
        to_bin(length, bin);
        for (const auto& x : value.names) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.hexes.size());
        to_bin(length, bin);
        for (const auto& x : value.hexes) {
            to_bin(x, bin);
        }
    }
}

message::scenario_condition_t to_protobuf (const ::scenario_condition_t& value)
{
    message::scenario_condition_t retval;
    for (const auto& x : value.actors) {
        retval.add_actors(x);
    }
    retval.set_action(static_cast< message::scenario_condition_t::action_t >(value.action));
    for (const auto& x : value.one_of) {
        retval.add_one_of()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::scenario_condition_t from_protobuf (const message::scenario_condition_t& msg)
{
    ::scenario_condition_t retval;
    {
        retval.actors.resize(msg.actors_size());
        auto it = retval.actors.begin();
        for (const auto& x : msg.actors()) {
            *it++ = x;
        }
    }
    retval.action = static_cast< scenario_condition_t::action_t >(msg.action());
    {
        retval.one_of.resize(msg.one_of_size());
        auto it = retval.one_of.begin();
        for (const auto& x : msg.one_of()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

void to_bin (const ::scenario_condition_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.actors.size());
        to_bin(length, bin);
        for (const auto& x : value.actors) {
            to_bin(x, bin);
        }
    }
    to_bin(value.action, bin);
    {
        int length = static_cast<int>(value.one_of.size());
        to_bin(length, bin);
        for (const auto& x : value.one_of) {
            to_bin(x, bin);
        }
    }
}

message::fleet_release_condition_t to_protobuf (const ::fleet_release_condition_t& value)
{
    message::fleet_release_condition_t retval;
    retval.set_fleet(value.fleet);
    retval.mutable_condition()->CopyFrom(to_protobuf(value.condition));
    return retval;
}

::fleet_release_condition_t from_protobuf (const message::fleet_release_condition_t& msg)
{
    ::fleet_release_condition_t retval;
    retval.fleet = msg.fleet();
    retval.condition = from_protobuf(msg.condition());
    return retval;
}

void to_bin (const ::fleet_release_condition_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.fleet, bin);
    to_bin(value.condition, bin);
}

message::war_entry_condition_t to_protobuf (const ::war_entry_condition_t& value)
{
    message::war_entry_condition_t retval;
    retval.set_economy(static_cast< message::war_footing_t >(value.economy));
    retval.mutable_condition()->CopyFrom(to_protobuf(value.condition));
    return retval;
}

::war_entry_condition_t from_protobuf (const message::war_entry_condition_t& msg)
{
    ::war_entry_condition_t retval;
    retval.economy = static_cast< war_footing_t >(msg.economy());
    retval.condition = from_protobuf(msg.condition());
    return retval;
}

void to_bin (const ::war_entry_condition_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.economy, bin);
    to_bin(value.condition, bin);
}

message::scenario_turn_t::national_action_t::action_t to_protobuf (const ::scenario_turn_t::national_action_t::action_t& value)
{
    message::scenario_turn_t::national_action_t::action_t retval;
    retval.set_type(static_cast< message::scenario_turn_t::national_action_t::action_type_t >(value.type));
    for (const auto& x : value.names) {
        retval.add_names(x);
    }
    return retval;
}

::scenario_turn_t::national_action_t::action_t from_protobuf (const message::scenario_turn_t::national_action_t::action_t& msg)
{
    ::scenario_turn_t::national_action_t::action_t retval;
    retval.type = static_cast< scenario_turn_t::national_action_t::action_type_t >(msg.type());
    {
        retval.names.resize(msg.names_size());
        auto it = retval.names.begin();
        for (const auto& x : msg.names()) {
            *it++ = x;
        }
    }
    return retval;
}

void to_bin (const ::scenario_turn_t::national_action_t::action_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.type, bin);
    {
        int length = static_cast<int>(value.names.size());
        to_bin(length, bin);
        for (const auto& x : value.names) {
            to_bin(x, bin);
        }
    }
}

message::scenario_turn_t::national_action_t to_protobuf (const ::scenario_turn_t::national_action_t& value)
{
    message::scenario_turn_t::national_action_t retval;
    for (const auto& x : value.actions) {
        retval.add_actions()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::scenario_turn_t::national_action_t from_protobuf (const message::scenario_turn_t::national_action_t& msg)
{
    ::scenario_turn_t::national_action_t retval;
    {
        retval.actions.resize(msg.actions_size());
        auto it = retval.actions.begin();
        for (const auto& x : msg.actions()) {
            *it++ = from_protobuf(x);
        }
    }
    return retval;
}

void to_bin (const ::scenario_turn_t::national_action_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.actions.size());
        to_bin(length, bin);
        for (const auto& x : value.actions) {
            to_bin(x, bin);
        }
    }
}

message::scenario_turn_t to_protobuf (const ::scenario_turn_t& value)
{
    message::scenario_turn_t retval;
    retval.set_turn(value.turn);
    for (const auto& x : value.national_actions) {
        (*retval.mutable_national_actions())[x.first] = to_protobuf(x.second);
    }
    return retval;
}

::scenario_turn_t from_protobuf (const message::scenario_turn_t& msg)
{
    ::scenario_turn_t retval;
    retval.turn = msg.turn();
    {
        for (const auto& x : msg.national_actions()) {
            retval.national_actions[x.first] = from_protobuf(x.second);
        }
    }
    return retval;
}

void to_bin (const ::scenario_turn_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.turn, bin);
    {
        int length = static_cast<int>(value.national_actions.size());
        to_bin(length, bin);
        for (const auto& x : value.national_actions) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
}

message::scenario_t::nation_t to_protobuf (const ::scenario_t::nation_t& value)
{
    message::scenario_t::nation_t retval;
    for (const auto& x : value.at_war_with) {
        retval.add_at_war_with(x);
    }
    for (const auto& x : value.future_belligerents) {
        retval.add_future_belligerents(x);
    }
    retval.set_economy(static_cast< message::war_footing_t >(value.economy));
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

::scenario_t::nation_t from_protobuf (const message::scenario_t::nation_t& msg)
{
    ::scenario_t::nation_t retval;
    {
        retval.at_war_with.resize(msg.at_war_with_size());
        auto it = retval.at_war_with.begin();
        for (const auto& x : msg.at_war_with()) {
            *it++ = x;
        }
    }
    {
        retval.future_belligerents.resize(msg.future_belligerents_size());
        auto it = retval.future_belligerents.begin();
        for (const auto& x : msg.future_belligerents()) {
            *it++ = x;
        }
    }
    retval.economy = static_cast< war_footing_t >(msg.economy());
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

void to_bin (const ::scenario_t::nation_t& value, std::vector<unsigned char>& bin)
{
    {
        int length = static_cast<int>(value.at_war_with.size());
        to_bin(length, bin);
        for (const auto& x : value.at_war_with) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.future_belligerents.size());
        to_bin(length, bin);
        for (const auto& x : value.future_belligerents) {
            to_bin(x, bin);
        }
    }
    to_bin(value.economy, bin);
    to_bin(value.exhaustion_turns, bin);
    to_bin(value.accumulate_exhaustion_at_peace, bin);
    {
        int length = static_cast<int>(value.release_conditions.size());
        to_bin(length, bin);
        for (const auto& x : value.release_conditions) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.war_entry_conditions.size());
        to_bin(length, bin);
        for (const auto& x : value.war_entry_conditions) {
            to_bin(x, bin);
        }
    }
}

message::scenario_t to_protobuf (const ::scenario_t& value)
{
    message::scenario_t retval;
    retval.set_name(value.name);
    retval.set_description(value.description);
    retval.mutable_start_turn()->CopyFrom(to_protobuf(value.start_turn));
    for (const auto& x : value.teams) {
        retval.add_teams()->CopyFrom(to_protobuf(x));
    }
    for (const auto& x : value.team_turn_order) {
        retval.add_team_turn_order(x);
    }
    retval.set_map(value.map);
    retval.set_order_of_battle(value.order_of_battle);
    for (const auto& x : value.setup_order) {
        retval.add_setup_order(x);
    }
    for (const auto& x : value.nations) {
        (*retval.mutable_nations())[x.first] = to_protobuf(x.second);
    }
    for (const auto& x : value.turns) {
        retval.add_turns()->CopyFrom(to_protobuf(x));
    }
    return retval;
}

::scenario_t from_protobuf (const message::scenario_t& msg)
{
    ::scenario_t retval;
    retval.name = msg.name();
    retval.description = msg.description();
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
            *it++ = x;
        }
    }
    retval.map = msg.map();
    retval.order_of_battle = msg.order_of_battle();
    {
        retval.setup_order.resize(msg.setup_order_size());
        auto it = retval.setup_order.begin();
        for (const auto& x : msg.setup_order()) {
            *it++ = x;
        }
    }
    {
        for (const auto& x : msg.nations()) {
            retval.nations[x.first] = from_protobuf(x.second);
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

void to_bin (const ::scenario_t& value, std::vector<unsigned char>& bin)
{
    to_bin(value.name, bin);
    to_bin(value.description, bin);
    to_bin(value.start_turn, bin);
    {
        int length = static_cast<int>(value.teams.size());
        to_bin(length, bin);
        for (const auto& x : value.teams) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.team_turn_order.size());
        to_bin(length, bin);
        for (const auto& x : value.team_turn_order) {
            to_bin(x, bin);
        }
    }
    to_bin(value.map, bin);
    to_bin(value.order_of_battle, bin);
    {
        int length = static_cast<int>(value.setup_order.size());
        to_bin(length, bin);
        for (const auto& x : value.setup_order) {
            to_bin(x, bin);
        }
    }
    {
        int length = static_cast<int>(value.nations.size());
        to_bin(length, bin);
        for (const auto& x : value.nations) {
            to_bin(x.first, bin);
            to_bin(x.second, bin);
        }
    }
    {
        int length = static_cast<int>(value.turns.size());
        to_bin(length, bin);
        for (const auto& x : value.turns) {
            to_bin(x, bin);
        }
    }
}


