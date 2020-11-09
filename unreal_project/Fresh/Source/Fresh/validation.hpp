#pragma once
#include "start_data.hpp"
#include "hex_operations.hpp"
#include "visual_config.hpp"

#include <adobe/name.hpp>

#include <boost/exception/diagnostic_information.hpp>


using name_t = adobe::name_t;
using namespace adobe::literals;

namespace start_data {

    template <typename T>
    inline void require_equal (T lhs, T rhs, const std::string& lhs_name, const std::string& rhs_name)
    {
        if (lhs != rhs) {
            throw std::runtime_error(
                lhs_name + " (=" + std::to_string(lhs) + ") and " +
                rhs_name + " (=" + std::to_string(rhs) + ") must be equal"
            );
        }
    }

    template <typename Cont>
    inline void require_nonempty (const Cont& c, const std::string& name)
    {
        if (c.empty())
            throw std::runtime_error(name + " must be provided and must not be empty");
    }

    template <typename T>
    inline void require_positive (T t, const std::string& name)
    {
        if (t <= 0) {
            throw std::runtime_error(name + " (=" + std::to_string(t) + ") must be positive (> 0)");
        }
    }

    template <typename T>
    inline void require_nonnegative (T t, const std::string& name)
    {
        if (t < 0) {
            throw std::runtime_error(name + " (=" + std::to_string(t) + ") must be nonnegative (>= 0)");
        }
    }

    template <typename T>
    inline void require_within (T t, T low, T high, const std::string& name)
    {
        if (t < low || high < t) {
            throw std::runtime_error(
                name + " (=" + std::to_string(t) + ") must be between " + std::to_string(low) +
                " and " + std::to_string(high) + " inclusive"
            );
        }
    }

    inline void require_hex_coord (int hex_id, int width, int height, const std::string& name)
    {
        const auto hc = hex_id_t(hex_id).to_hex_coord();
        if (!on_map(hc, width, height)) {
            throw std::runtime_error(
                name + " (=" + std::to_string(hex_id) + ") " +
                "must be a valid hex coordinate within the map (map width=" +
                std::to_string(width) + " and height=" + std::to_string(height) + ")"
            );
        }
    }

    inline void require_nation (name_t name, const nations_t& nations)
    {
        if (nations.nations.count(name) == 0) {
            throw std::runtime_error("Nation '" + make_string(name) + "' is not found in the nation definitions");
        }
    }

    inline void validate_turn (const turn_t& turn)
    {
        if (turn.year != 1000)
            require_within(turn.year, 50, 185, "turn.year"); // TODO: Use the right year range!
    }

    inline void validate_capital (const capital_t& capital)
    {
        for (const auto& capital_hex : capital.hexes) {
            require_nonempty(capital_hex.zones, "capital_hex_t.zones");
            for (const auto& zone : capital_hex.zones) {
                require_nonempty(zone.name, "capital_hex_zone_t.name");
            }
        }
    }

    inline void validate_offmap_possesions (const offmap_possesions_t& offmap_possesions)
    {
        require_nonnegative(offmap_possesions.provinces, "offmap_possesions_t.provinces");
        require_nonnegative(offmap_possesions.mins, "offmap_possesions_t.mins");
        require_nonnegative(offmap_possesions.majs, "offmap_possesions_t.majs");
        require_nonnegative(offmap_possesions.survey_ships, "offmap_possesions_t.survey_ships");
    }

    inline void validate_nation (const nation_t& nation)
    {
        require_nonempty(nation.name, "nation_t.name");
        require_nonempty(nation.short_name, "nation_t.short_name");
        validate_capital(nation.capital);
        require_nonnegative(nation.free_strategic_moves, "free_strategic_moves");
        require_within(nation.capital_star_points, 5, 10, "capital_star_points");
        validate_offmap_possesions(nation.offmap_possesions);
        if (nation.nation_id != 0)
            throw std::runtime_error("Starting nations must not define a nation_id");
    }

    inline void validate_nations (const nations_t& nations)
    {
        require_nonempty(nations.nations, "nations_t.nations");
        for (const auto& n : nations.nations) {
            validate_nation(n.second);
        }
    }

    inline void validate_hex_coords (const nations_t& nations, int width, int height)
    {
        for (const auto& n : nations.nations) {
            for (const auto& hex : n.second.capital.hexes) {
                require_hex_coord(hex.coord, width, height, "capital_hex_t.coord");
            }
        }
    }

    inline void validate_offmap_area (const offmap_area_t& offmap_area, const map_t& map)
    {
        if (offmap_area.adjacent_hexes.empty())
            return;

        require_nonempty(offmap_area.name, "offmap_area_t.name");
        require_equal(
            offmap_area.features.size(), offmap_area.feature_hexes.size(),
            "offmap_area_t.features.size()", "offmap_area_t.feature_hexes.size()"
        );

        for (auto hex_id : offmap_area.feature_hexes) {
            const auto hc = hex_id_t(hex_id).to_hex_coord();
            if (hc.x != -1 && hc.x != map.width && hc.y != -1 && hc.y != map.height) {
                throw std::runtime_error(
                    "offmap_area_t.feature_hexes (=" + std::to_string(hex_id) + ") " +
                    "must be a hex coordinate one tile outside the map (map width=" +
                    std::to_string(map.width) + " and height=" + std::to_string(map.height) + ")"
                );
            }
        }

        require_within(
            offmap_area.adjacent_hexes.size(), std::size_t(2), std::size_t(1000),
            "offmap_area_t.adjacent_hexes.size()"
        );
        for (auto hex_id : offmap_area.adjacent_hexes) {
            const auto hc = hex_id_t(hex_id).to_hex_coord();
            if (hc.x != 0 && hc.x != map.width - 1 && hc.y != 0 && hc.y != map.height - 1) {
                throw std::runtime_error(
                    "offmap_area_t.adjacent_hexes (=" + std::to_string(hex_id) + ") " +
                    "must be a hex coordinate on the edge of the map (map width=" +
                    std::to_string(map.width) + " and height=" + std::to_string(map.height) + ")"
                );
            }
        }
        auto const first_hc = hex_id_t(offmap_area.adjacent_hexes[0]).to_hex_coord();
        auto const same_x = std::all_of(
            offmap_area.adjacent_hexes.begin(),
            offmap_area.adjacent_hexes.end(),
            [=](int hex_id) {
                auto const hc = hex_id_t(hex_id).to_hex_coord();
                return hc.x == first_hc.x;
            }
        );
        auto const same_y = std::all_of(
            offmap_area.adjacent_hexes.begin(),
            offmap_area.adjacent_hexes.end(),
            [=](int hex_id) {
                auto const hc = hex_id_t(hex_id).to_hex_coord();
                return hc.y == first_hc.y;
            }
        );
        if (!same_x && !same_y)
            throw std::runtime_error("offmap_area_t.adjacent_hexes must all have the same row or column");
    }

    inline void validate_and_fill_in_map_hexes (map_t& map, const nations_t& nations)
    {
        require_nonnegative(map.width, "map_t.width");
        require_nonnegative(map.height, "map_t.height");

        const hex_t uninitialized_hex = { invalid_hex_coord, -1 };
        map.hexes.resize(map.width * map.height, uninitialized_hex);

        auto nz_nation_id = nations.nations.find("NZ"_name)->second.nation_id;

        // Add NZ hexes.
        for (auto hex_id : map.nz_hexes) {
            require_hex_coord(hex_id, map.width, map.height, "nz_hexes hex");

            const hex_coord_t hc = hex_id_t(hex_id).to_hex_coord();
            auto const i = hex_index_t(hc, map.width);
            hex_t& hex = map.hexes[i];

            if (hex.coord != invalid_hex_coord) {
                throw std::runtime_error("Duplicate definition of NZ hex " + std::to_string(hex_id));
            }

            hex.coord = hc;
            hex.owner = nz_nation_id;
        }

        // Add NZ planets.
        for (auto hex_id : map.nz_planets) {
            require_hex_coord(hex_id, map.width, map.height, "nz_planets hex");

            const hex_coord_t hc = hex_id_t(hex_id).to_hex_coord();
            auto const i = hex_index_t(hc, map.width);
            hex_t& hex = map.hexes[i];

            if (hex.owner != nz_nation_id) {
                throw std::runtime_error(
                    "Cannot place NX planet in " + std::to_string(hex_id) +
                    ", because that is not a NZ hex.");
            }

            hex.feature = feature_t::min;
        }

        // Add hexes from initial national holdings.
        for (const auto& holdings : map.starting_national_holdings) {
            auto nations_it = nations.nations.find(holdings.first);
            if (nations_it == nations.nations.end()) {
                throw std::runtime_error("Unknown owner nation '" + make_string(holdings.first) + "' encountered in map data");
            }
            const int nation_id = nations_it->second.nation_id;

            validate_offmap_area(holdings.second.offmap_area, map);

            require_nonempty(holdings.second.provinces, "starting_national_holdings_t.provinces");
            for (const auto& province : holdings.second.provinces) {
                require_nonempty(province.hexes, "province_t.hexes");
                for (const auto& province_hex : province.hexes) {
                    const auto hex_id = province_hex.hex;
                    require_hex_coord(hex_id, map.width, map.height, "starting province hex");

                    const hex_coord_t hc = hex_id_t(hex_id).to_hex_coord();
                    auto const i = hex_index_t(hc, map.width);
                    hex_t& hex = map.hexes[i];

                    if (hex.coord != invalid_hex_coord) {
                        throw std::runtime_error("Duplicate definition of hex " + std::to_string(hex_id));
                    }

                    hex.coord = hc;
                    hex.owner = nation_id;
                    hex.feature = province_hex.feature;
                }
            }
        }

        for (std::size_t i = 0; i < map.hexes.size(); ++i) {
            if (map.hexes[i].coord == invalid_hex_coord) {
                int hex_x = i % map.width + 1;
                int hex_y = i / map.width + 1;
                std::string hex_str = std::to_string(hex_x * 100 + hex_y);
                while (hex_str.size() < 4u) {
                    hex_str = '0' + hex_str;
                }
                throw std::runtime_error("Hex " + hex_str + " not defined in map.json");
            }
        }
    }

    inline void validate_and_fixup_oob_unit (oob_unit_t& oob_unit, const nation_unit_defs_t& unit_defs)
    {
        // TODO: Check that the unit is a known unit for this nation.
        require_nonempty(oob_unit.unit, "oob_unit_t.unit");
        require_nonnegative(oob_unit.times, "oob_unit_t.times");
        if (oob_unit.times == 0)
            oob_unit.times = 1;
    }

    inline void validate_and_fixup_starting_fleet (starting_fleet_t& starting_fleet,
                                                   int width,
                                                   int height,
                                                   const nation_unit_defs_t& unit_defs)
    {
        for (auto hex_id : starting_fleet.hexes) {
            require_hex_coord(hex_id, width, height, "starting fleet hex");
        }
        require_nonempty(starting_fleet.units, "starting_fleet_t.units");
        for (auto& oob_unit : starting_fleet.units) {
            validate_and_fixup_oob_unit(oob_unit, unit_defs);
        }
        for (auto& element : starting_fleet.prewar_construction) {
            for (auto& oob_unit : element.units) {
                validate_and_fixup_oob_unit(oob_unit, unit_defs);
            }
        }
        require_nonnegative(
            starting_fleet.strategic_move_arrival_year,
            "starting_fleet_t.strategic_move_arrival_year"
        );
        for (auto limit : starting_fleet.hex_placement_limits) {
            require_hex_coord(limit.first, width, height, "hex_placement_limit hex");
            require_positive(limit.second, "hex_placement_limit unit limit number");
        }
    }

    inline void validate_and_fill_in_unit_times (orders_of_battle_t& oobs,
                                                 const map_t& map,
                                                 const nations_t& nations,
                                                 const unit_defs_t& unit_defs_)
    {
#if 0
        boost::container::flat_set<std::string> nation_units;
#endif
        require_nonempty(oobs.oobs, "orders_of_battle_t.oobs");
        for (auto& oob : oobs.oobs) {
            require_nation(oob.first, nations);

            if (unit_defs_.nation_units.count(oob.first) == 0) {
                static std::string msg;
                msg = "Cannot validate the OOB for " + make_string(oob.first) +
                    " because there are no units defined for that nation.";
                throw std::runtime_error(msg.c_str());
            }
            const auto& unit_defs =
                unit_defs_.nation_units.find(oob.first)->second;
#if 0
            nation_units.clear();
            for (const auto& unit : unit_defs.units) {
                nation_units.insert(unit.name);
            }
#endif

            require_nonempty(oob.second.starting_fleets, "order_of_battle_t.starting_fleets");
            for (auto& fleet : oob.second.starting_fleets) {
                require_nonempty(fleet.first, "starting_fleets_t name ('key')");

                validate_and_fixup_starting_fleet(fleet.second, map.width, map.height, unit_defs);
            }

            for (auto& oob_unit : oob.second.mothball_reserve.units) {
                validate_and_fixup_oob_unit(oob_unit, unit_defs);
            }
            for (auto& oob_unit : oob.second.mothball_reserve.war_release) {
                validate_and_fixup_oob_unit(oob_unit, unit_defs);
            }
            for (auto& oob_unit : oob.second.mothball_reserve.limited_war_release) {
                validate_and_fixup_oob_unit(oob_unit, unit_defs);
            }

            require_nonempty(oob.second.production, "order_of_battle_t.production");
            for (auto& element : oob.second.production) {
                for (auto& oob_unit : element.units) {
                    validate_and_fixup_oob_unit(oob_unit, unit_defs);
                }
            }
        }
    }

    inline void validate_unit_def_side (const unit_def_side_t& unit_def_side)
    {
        require_nonnegative(unit_def_side.att, "unit_def_side_t.att");
        require_nonnegative(unit_def_side.def, "unit_def_side_t.def");
        require_nonnegative(unit_def_side.fighters, "unit_def_side_t.fighters");
        require_nonnegative(unit_def_side.heavy_fighter_bonus, "unit_def_side_t.heavy_fighter_bonus");
        require_nonnegative(unit_def_side.drones, "unit_def_side_t.drones");
    }

    inline void validate_production_cost (const production_cost_t& production_cost)
    {
        require_nonnegative(production_cost.cost, "production_cost.cost");
        require_nonnegative(production_cost.fighter_cost, "production_cost.fighter_cost");
    }

    inline void validate_unit_def (const unit_def_t& unit_def)
    {
        require_nonempty(unit_def.name, "unit_def_t.name");
        require_nonnegative(unit_def.cmd, "unit_def_t.cmd");
        validate_unit_def_side(unit_def.uncrippled);
        validate_unit_def_side(unit_def.crippled);
        validate_turn(unit_def.available);
        validate_production_cost(unit_def.construction);
        for (auto const& pair : unit_def.substitutions) {
            require_nonempty(pair.first, "unit_def_t.substitutions.name");
            validate_production_cost(pair.second);
        }
        for (auto const& pair : unit_def.conversions) {
            require_nonempty(pair.first, "unit_def_t.conversions.name");
            validate_production_cost(pair.second);
        }
        require_nonnegative(unit_def.move, "unit_def_t.move");
        require_nonnegative(unit_def.towable.move_cost, "towable_t.move_cost");
        require_nonnegative(unit_def.towable.strat_move_limit, "towable_t.strat_move_limit");
        require_nonnegative(unit_def.salvage, "unit_def_t.salvage");
    }

    inline void validate_unit_defs (const unit_defs_t& unit_defs)
    {
        require_nonempty(unit_defs.nation_units, "unit_defs_t.nation_units");
        for (const auto& pair : unit_defs.nation_units) {
            require_nonempty(pair.first, "unit_def_t.nation_units.name");
            require_nonempty(pair.second.units, "unit_def_t.nation_units.units");
            for (const auto& u : pair.second.units) {
                validate_unit_def(u);
            }
        }
    }

    inline void validate_team (const team_t& team, const nations_t& nations)
    {
        require_nonempty(team.name, "team_t.name");
        for (const auto& nation : team.nations) {
            require_nation(nation, nations);
        }
    }

    inline bool any_sb (const scenario_condition_t::object_t& object)
    {
        return
            object.type == scenario_condition_t::object_type_t::sb &&
            object.hexes.empty() &&
            object.names.size() == 1u && object.names[0] == "any"_name;
    }

    inline void validate_scenario_condition (const scenario_condition_t& scenario_condition,
                                             const team_t& team,
                                             const nations_t& nations)
    {
        auto require_different_team = [&team](name_t name) {
            auto it = std::find(team.nations.begin(), team.nations.end(), name);
            if (it != team.nations.end()) {
                static std::string msg;
                msg = "A release or war entry condition cannot be based on the "
                    "actions of fellow team member " + make_string(name);
                throw std::runtime_error(msg.c_str());
            }
        };

        for (const auto& actor : scenario_condition.actors) {
            if (actor == "future belligerent"_name)
                continue;
            require_nation(actor, nations);
            require_different_team(actor);
        }

        auto find_sb = [&scenario_condition]() {
            return std::find_if(
                scenario_condition.one_of.begin(), scenario_condition.one_of.end(),
                [](const scenario_condition_t::object_t& o) {
                    return o.type == scenario_condition_t::object_type_t::sb;
                }
            );
        };

        switch (scenario_condition.action) {
        case scenario_condition_t::action_t::occupies:
            if (find_sb() != scenario_condition.one_of.end()) {
                throw std::runtime_error(
                    "scenario_condition_t.action_t 'occupies' is incompatible with object_type_t 'sb'"
                );
            }
            break;
        case scenario_condition_t::action_t::destroys:
            if (find_sb() == scenario_condition.one_of.end()) {
                throw std::runtime_error(
                    "scenario_condition_t.action_t 'destroys' is only compatible with object_type_t 'sb'"
                );
            }
            break;
        }

        for (const auto& object : scenario_condition.one_of) {
            switch (object.type) {
            case scenario_condition_t::object_type_t::hexes: // fallthrough
            case scenario_condition_t::object_type_t::sb:
                if (!object.names.empty() && !any_sb(object)) {
                    throw std::runtime_error(
                        "You cannot define object_t.names when object_t.type is 'hexes' or 'sb', "
                        "except that you can say 'any' with 'sb'"
                    );
                }
#if 0 // TODO: Do after the map is loaded.
                if (any_sb(object)) {
                    // TODO: Fill in hexes with all the starting SB hexes.
                }
                for (auto hex_id : object.hexes) {
                    require_hex_coord(hex_id, map.width, map.height, "scenario_condition_t.object_t.hexes");
                    if (object.type == scenario_condition_t::object_type_t::sb) {
                        // TODO: Verify that there is a starbase at hex_id.
                    }
                }
#endif
                break;
            case scenario_condition_t::object_type_t::fleet_area:
#if 0 // TODO: Do after the map is loaded.
                for (const auto& name : object.names) {
                    // TODO: Verify that the given fleet exists.
                    // TODO: Fill in object.hexes from fleet area.
                    std::sort(object.hexes.begin(), object.hexes.end());
                }
                object.names.clear();
#endif
                break;
            case scenario_condition_t::object_type_t::nation:
                for (const auto& name : object.names) {
                    require_nation(name, nations);
#if 0 // TODO: Do after the map is loaded.
                    // TODO: Fill in object.hexes from nation's area.
                    std::sort(object.hexes.begin(), object.hexes.end());
#endif
                }
#if 0 // TODO: Do after the map is loaded.
                object.names.clear();
#endif
                break;
            }
        }
    }

    inline void validate_scenario_nation (const scenario_t::nation_t& nation,
                                          const team_t& team,
                                          const nations_t& nations)
    {
        auto require_different_team = [&team](name_t name) {
            auto it = std::find(team.nations.begin(), team.nations.end(), name);
            if (it != team.nations.end()) {
                static std::string msg;
                auto name_str = make_string(name);
                msg =
                    "A nation cannot be at war with or a future belligerent of " +
                    name_str + " and on the same team with " + name_str;
                throw std::runtime_error(msg.c_str());
            }
        };

        for (const auto& name : nation.at_war_with) {
            require_nation(name, nations);
            require_different_team(name);
        }
        for (const auto& name : nation.future_belligerents) {
            require_nation(name, nations);
            require_different_team(name);
        }

        require_nonnegative(nation.exhaustion_turns, "scenario_t.nation_t.exhaustion_turns");

        for (const auto& condition : nation.release_conditions) {
            // TODO: Compare condition.fleet against fleet names in OOB.
            validate_scenario_condition(condition.condition, team, nations);
        }

        for (const auto& condition : nation.war_entry_conditions) {
            validate_scenario_condition(condition.condition, team, nations);
        }
    }

    inline void validate_scenario_turn (const scenario_turn_t& turn, const nations_t& nations)
    {
        require_nonnegative(turn.turn, "scenario_turn_t.turn");
        for (const auto& national_action : turn.national_actions) {
            require_nation(national_action.first, nations);
            for (const auto& action : national_action.second.actions) {
                switch (action.type) {
                case scenario_turn_t::national_action_t::action_type_t::release_fleets:
                    // TODO: Compare action.names against fleet names in OOB.
                    break;
                case scenario_turn_t::national_action_t::action_type_t::declare_war:
                    for (const auto& name : action.names) {
                        require_nation(name, nations);
                    }
                    break;
                }
            }
        }
    }

    inline void validate_scenario (const scenario_t& scenario, const nations_t& nations)
    {
        require_nonempty(scenario.name, "scenario.name");
        require_nonempty(scenario.description, "scenario.description");
        validate_turn(scenario.start_turn);

        std::vector<name_t> team_names;
        std::vector<name_t> team_member_names;
        for (const auto& team : scenario.teams) {
            validate_team(team, nations);
            team_names.push_back(team.name);
            team_member_names.insert(team_member_names.end(), team.nations.begin(), team.nations.end());
        }

        std::sort(team_names.begin(), team_names.end());
        if (std::unique(team_names.begin(), team_names.end()) != team_names.end())
            throw std::runtime_error("Each team name in a scenario must be unique");

        std::sort(team_member_names.begin(), team_member_names.end());
        if (std::unique(team_member_names.begin(), team_member_names.end()) != team_member_names.end())
            throw std::runtime_error("Each nation in a scenario can belong to at most one team");

        auto team_turn_order = scenario.team_turn_order;
        std::sort(team_turn_order.begin(), team_turn_order.end());
        if (!std::equal(team_turn_order.begin(), team_turn_order.end(), team_names.begin())) {
            throw std::runtime_error("Every team in a scenario must be listed in the turn order (with no extras)");
        }

        require_nonempty(scenario.map, "scenario.map");
        require_nonempty(scenario.order_of_battle, "scenario.order_of_battle");

        auto setup_order = scenario.setup_order;
        std::sort(setup_order.begin(), setup_order.end());
        if (!std::equal(setup_order.begin(), setup_order.end(), team_member_names.begin())) {
            throw std::runtime_error("Every nation in a scenario must be listed in the setup order (with no extras)");
        }

        for (const auto& nation : scenario.nations) {
            require_nation(nation.first, nations);

            auto it = std::find_if(
                scenario.teams.begin(), scenario.teams.end(),
                [&nation](const team_t& team) {
                    auto it = std::find(team.nations.begin(), team.nations.end(), nation.first);
                    return it != team.nations.end();
                }
            );
            if (it == scenario.teams.end()) {
                static std::string msg;
                msg = "Unable to find the team to which nation " + make_string(nation.first) + " belongs.";
                throw std::runtime_error(msg.c_str());
            }

            validate_scenario_nation(nation.second, *it, nations);
        }

        for (const auto& turn : scenario.turns) {
            validate_scenario_turn(turn, nations);
        }
    }

    inline void validate_scenario_with_map_and_oob (const scenario_t& scenario,
                                                    const map_t& map,
                                                    const orders_of_battle_t& oobs)
    {
        // TODO
    }

}

namespace visual_config {

    inline void validate_color (const color_t& color)
    {
        start_data::require_within(color.r, 0, 255, "color.r");
        start_data::require_within(color.g, 0, 255, "color.g");
        start_data::require_within(color.b, 0, 255, "color.b");
    }

    inline void validate_hex_map (const hex_map_t& hex_map, const start_data::nations_t& nations)
    {
        for (auto const & pair: hex_map.primary_colors) {
            start_data::require_nation(pair.first, nations);
            validate_color(pair.second);
        }
        for (auto const & pair: hex_map.secondary_colors) {
            start_data::require_nation(pair.first, nations);
            validate_color(pair.second);
        }

        start_data::require_equal(
            hex_map.primary_colors.size(), hex_map.secondary_colors.size(),
            "hex_map.primary_colors.size()", "hex_map.secondary_colors.size()"
        );

        using pair_t = boost::container::flat_map<name_t, color_t>::value_type;
        if (!std::equal(
                hex_map.primary_colors.begin(), hex_map.primary_colors.end(),
                hex_map.secondary_colors.begin(),
                [](pair_t const & pair_1, pair_t const & pair_2) {
                    return pair_1.first == pair_2.first;
                })) {
            throw std::runtime_error("Every nation listed in primary_colors must also be listed in secondary_colors (with no extras)");
        }

        start_data::require_within(hex_map.national_border_thickness, 0.0f, 1.0f, "national_border_thickness");
        start_data::require_within(hex_map.province_border_thickness, 0.0f, 1.0f, "province_border_thickness");
        start_data::require_within(hex_map.planet_star_thickness, 0.0f, 1.0f, "planet_star_thickness");
        start_data::require_within(hex_map.minor_planet_scale, 0.0f, 1.0f, "minor_planet_scale");
        start_data::require_within(hex_map.major_planet_scale, 0.0f, 1.0f, "major_planet_scale");
        start_data::require_within(hex_map.star_scale, 0.0f, 1.0f, "star_scale");
        start_data::require_within(hex_map.mb_scale, 0.0f, 1.0f, "mb_scale");
        start_data::require_within(hex_map.bats_scale, 0.0f, 1.0f, "bats_scale");
        start_data::require_within(hex_map.sb_scale, 0.0f, 1.0f, "sb_scale");
        start_data::require_within(hex_map.offmap_z, 0.0f, 1.0f, "offmap_z");
        start_data::require_within(hex_map.offmap_border_thickness, 0.0f, 1.0f, "offmap_border_thickness");
        start_data::require_within(hex_map.offmap_label_size, 0.0f, 500.0f, "offmap_label_size");
        start_data::require_within(hex_map.offmap_left_right_thickness, 0.0f, 20.0f, "offmap_left_right_thickness");
        start_data::require_within(hex_map.offmap_top_bottom_thickness, 0.0f, 20.0f, "offmap_top_bottom_thickness");
    }

}
