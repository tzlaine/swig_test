#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "game_data_t.hpp"

#include "start_data_t.hpp"


namespace {

    bool planet (start_data::feature_t feature)
    {
        switch (feature) {
        default:
        case start_data::feature_t::no_feature:
            assert(!"Unreachable");
            return false;

        case start_data::feature_t::bats:
        case start_data::feature_t::sb:
            return false;

        case start_data::feature_t::min:
        case start_data::feature_t::maj:
        case start_data::feature_t::capital:
            return true;
        }
    }

    hex_zone_fixture_t make_fixture (start_data::feature_t feature, int owner)
    {
        hex_zone_fixture_t retval = {};

        if (planet(feature)) {
            retval.type = hex_zone_fixture_t::type_t::type_planet;
            retval.planet.owner = owner;
            retval.planet.original_owner = owner;
        } else {
            retval.type = hex_zone_fixture_t::type_t::type_base;
            retval.base.owner = owner;
            retval.base.original_owner = owner;
        }

        unit_t pdu;
        pdu.unit_id = 1;
        pdu.owner = owner;
        pdu.original_owner = owner;

        switch (feature) {
        default:
        case start_data::feature_t::no_feature:
            assert(!"Unreachable");
            break;

        case start_data::feature_t::bats:
            retval.base.unit_id = 0;
            break;
        case start_data::feature_t::sb:
            retval.base.unit_id = 2;
            break;
        case start_data::feature_t::min:
            retval.planet.type = planet_t::type_t::minor;
            // TODO: PDUs
            break;
        case start_data::feature_t::maj:
            retval.planet.type = planet_t::type_t::major;
            // TODO: PDUs
            break;
        case start_data::feature_t::capital:
            retval.planet.type = planet_t::type_t::capital;
            // TODO: PDUs
            break;
        }

        return retval;
    }

}

game_data_t::game_data_t ()
{}

game_data_t::game_data_t (start_data::start_data_t const & start_data)
{
    map_.width = start_data.map().width;
    map_.height = start_data.map().height;

    map_.provinces.resize(start_data.num_provinces(), province_t{-1});

    map_.hexes.resize(map_.width * map_.height);
    auto it = map_.hexes.begin();
    for (auto const & h : start_data.map().hexes) {
        auto & hex = *it++;
        hex.coord = h.coord;
        hex.province_id = start_data.hex_province(h.coord);

        if (hex.province_id != -1) {
            assert(hex.province_id < map_.provinces.size());
            auto & province = map_.provinces[hex.province_id];
            if (province.id == -1) {
                province.id = hex.province_id;
                province.owner = h.owner;
                province.original_owner = h.owner;
            }
        }

        switch (h.feature) {
        default:
        case start_data::feature_t::no_feature:
            break;

        case start_data::feature_t::bats: // fallthrough
        case start_data::feature_t::sb:   // fallthrough
        case start_data::feature_t::min:  // fallthrough
        case start_data::feature_t::maj:
            hex.zones.resize(1u);
            hex.zones.back().fixtures.resize(1u);
            hex.zones.back().fixtures.back() = make_fixture(h.feature, h.owner);
            break;

        case start_data::feature_t::capital: {
            auto const & nation = start_data.nation(h.owner);
            auto const & capital_hex = *std::find_if(
                nation.capital.hexes.begin(), nation.capital.hexes.end(),
                [=] (start_data::capital_hex_t const & x) { return x.coord == hex_id(hex.coord); }
            );
            hex.zones.resize(capital_hex.zones.size());
            auto start_zone_it = capital_hex.zones.begin();
            for (auto & zone : hex.zones) {
                zone.name = start_zone_it->name;
                zone.fixtures.resize(start_zone_it->features.size());
                auto feature_it = start_zone_it->features.begin();
                for (auto & fixture : zone.fixtures) {
                    fixture = make_fixture(*feature_it, h.owner);
                    ++feature_it;
                }
                ++start_zone_it;
            }
            break;
        }
        }
    }

    int i = 0;
    for (auto & province : map_.provinces) {
        if (province.id == -1)
            throw std::runtime_error(
                "No province " + std::to_string(i) + " could be defined from the list of national hexes."
            );
        ++i;
    }
}
