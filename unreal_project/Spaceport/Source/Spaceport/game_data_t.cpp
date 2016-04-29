#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "game_data_t.hpp"

#include "start_data_t.hpp"


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
        case start_data::feature_t::sb: {
            hex.zones.resize(1u);
            hex.zones.back().fixtures.resize(1u);
            hex_zone_fixture_t & fixture = hex.zones.back().fixtures.back();
            fixture.type = hex_zone_fixture_t::type_t::type_base;
            // fixture.base.unit_id = TODO;
            fixture.base.owner = h.owner;
            fixture.base.original_owner = h.owner;
            break;
        }

        case start_data::feature_t::min: // fallthrough
        case start_data::feature_t::maj: {
            hex.zones.resize(1u);
            hex.zones.back().fixtures.resize(1u);
            hex_zone_fixture_t & fixture = hex.zones.back().fixtures.back();
            fixture.type = hex_zone_fixture_t::type_t::type_planet;
            fixture.planet.owner = h.owner;
            fixture.planet.original_owner = h.owner;
            fixture.planet.type =
                h.feature == start_data::feature_t::min ?
                planet_t::type_t::minor : planet_t::type_t::major;
            break;
        }

        case start_data::feature_t::capital:
            // TODO
            break;
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
