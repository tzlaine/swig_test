#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "data_utility.hpp"


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
        pdu.unit_id = early_pdu_unit_id();
        pdu.owner = owner;
        pdu.original_owner = owner;

        switch (feature) {
        default:
        case start_data::feature_t::no_feature:
            assert(!"Unreachable");
            break;

        case start_data::feature_t::bats:
            retval.base.unit_id = early_bats_unit_id();
            break;
        case start_data::feature_t::sb:
            retval.base.unit_id = early_sb_unit_id();
            break;
        case start_data::feature_t::min:
            retval.planet.type = planet_t::type_t::minor;
            retval.planet.units.fleets[owner].units.resize(minor_planet_starting_pdus(), pdu);
            break;
        case start_data::feature_t::maj:
            retval.planet.type = planet_t::type_t::major;
            retval.planet.units.fleets[owner].units.resize(major_planet_starting_pdus(), pdu);
            break;
        case start_data::feature_t::capital:
            retval.planet.type = planet_t::type_t::capital;
            retval.planet.units.fleets[owner].units.resize(capital_planet_starting_pdus(), pdu);
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
            assert(hex.province_id < (int)map_.provinces.size());
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
                [=] (start_data::capital_hex_t const & x) { return x.coord == to_hex_id(hex.coord); }
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

    auto const & scenario = start_data.scenario();
    for (auto const & t : scenario.teams) {
        auto & team = teams_[t.name];

        team.nations.resize(t.nations.size());
        std::transform(
            t.nations.begin(), t.nations.end(),
            team.nations.begin(),
            [&](name_t nation_name) {
                return start_data.nation_id(nation_name);
            }
        );

        std::sort(team.nations.begin(), team.nations.end());

        for (auto const nation_id : team.nations) {
            nation_teams_[nation_id] = t.name;
        }
    }

    for (auto const & pair : scenario.nations) {
        auto const & nation = pair.second;
        for (auto const & enemy_nation_key : nation.at_war_with) {
            auto const nation_id = start_data.nation_id(pair.first);
            auto const enemy_nation_id = start_data.nation_id(enemy_nation_key);
            declare_war(nation_id, enemy_nation_id, start_data);
        }
    }
}

void game_data_t::declare_war (
    int nation_id,
    int enemy_nation_id,
    start_data::start_data_t const & start_data
) {
    auto const it = nation_teams_.find(enemy_nation_id);
    if (it != nation_teams_.end()) {
        auto const & enemy_team_name = it->second;
        reusable_elem_.team_ = enemy_team_name;
        reusable_elem_.nation_ = nation_id;
        team_and_nation_at_war_.insert(reusable_elem_);
    }

    nations_at_war_.insert(nations_at_war_elem_t(nation_id, enemy_nation_id));
}

void game_data_t::leave_war (int nation_id, start_data::start_data_t const & start_data)
{
    {
        auto const new_end_it = std::remove_if(
            team_and_nation_at_war_.begin(), team_and_nation_at_war_.end(),
            [=](team_and_nation_at_war_elem_t elem) {
                return elem.nation_ == nation_id;
            }
        );
        team_and_nation_at_war_.erase(new_end_it, team_and_nation_at_war_.end());
    }

    {
        auto const new_end_it = std::remove_if(
            nations_at_war_.begin(), nations_at_war_.end(),
            [=](nations_at_war_elem_t elem) {
                return elem.lhs_ == nation_id || elem.rhs_ == nation_id;
            }
        );
        nations_at_war_.erase(new_end_it, nations_at_war_.end());
    }
}
