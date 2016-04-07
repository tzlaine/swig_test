#pragma once
#include "model_data.hpp"
#include "hex_operations.hpp"

#include <boost/exception/diagnostic_information.hpp>

#include <boost/lexical_cast.hpp>


template <typename Cont>
inline void require_nonempty (const Cont& c, const std::string& name)
{
    if (c.empty())
        boost::throw_exception(std::runtime_error(name + " must be provided and must not be empty"));
}

template <typename T>
inline void require_positive (T t, const std::string& name)
{
    if (t <= 0) {
        boost::throw_exception(
            std::runtime_error(name + " (=" + std::to_string(t) + ") must be positive (> 0)")
        );
    }
}

template <typename T>
inline void require_nonnegative (T t, const std::string& name)
{
    if (t < 0) {
        boost::throw_exception(
            std::runtime_error(name + " (=" + std::to_string(t) + ") must be nonnegative (>= 0)")
        );
    }
}

template <typename T>
inline void require_within (T t, T low, T high, const std::string& name)
{
    if (t < low || high < t) {
        boost::throw_exception(
            std::runtime_error(
                name + " (=" + std::to_string(t) + ") must be between " + std::to_string(low) +
                " and " + std::to_string(high) + " inclusive"
            )
        );
    }
}

inline void require_hex_coord (int i, int width, int height, const std::string& name)
{
    const auto hc = to_hex_coord(i);
    if (!on_map(hc, width, height)) {
        boost::throw_exception(
            std::runtime_error(
                name + " (=" + std::to_string(i) + ") " +
#if LOG
                "(->" + boost::lexical_cast<std::string>(hc) + ") "
#endif
                "must be a valid hex coordinate within the map (map width=" +
                std::to_string(width) + " and height=" + std::to_string(height) + ")"
            )
        );
    }
}

inline void validate_turn (const turn_t& turn)
{
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
        boost::throw_exception(std::runtime_error("Starting nations must not define a nation_id"));
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

inline void validate_and_fill_in_map_hexes (map_t& map, const nations_t& nations)
{
    require_nonnegative(map.width, "map_t.width");
    require_nonnegative(map.height, "map_t.height");

    const hex_t uninitialized_hex = { invalid_hex_coord, -1 };
    map.hexes.resize(map.width * map.height, uninitialized_hex);

    // Add NZ hexes.
    for (auto hex_id : map.nz_hexes) {
        require_hex_coord(hex_id, map.width, map.height, "nz_hexes hex");

        const hex_coord_t hc = to_hex_coord(hex_id);
        const int i = hex_index(hc, map.width);
        hex_t& hex = map.hexes[i];

        if (hex.coord != invalid_hex_coord) {
            boost::throw_exception(
                std::runtime_error("Duplicate definition of NZ hex " + std::to_string(hex_id))
            );
        }

        hex.coord = hc;
        hex.owner = 0;
    }

    // Add NZ planets.
    for (auto hex_id : map.nz_planets) {
        require_hex_coord(hex_id, map.width, map.height, "nz_planets hex");

        const hex_coord_t hc = to_hex_coord(hex_id);
        const int i = hex_index(hc, map.width);
        hex_t& hex = map.hexes[i];

        if (hex.owner != 0) {
            boost::throw_exception(
                std::runtime_error(
                    "Cannot place NX planet in " + std::to_string(hex_id) +
                    ", because that is not a NZ hex.")
            );
        }

        hex.feature = feature_t::min;
    }

    // Add hexes from initial national holdings.
    for (const auto& holdings : map.starting_national_holdings) {
        auto nations_it = nations.nations.find(holdings.first);
        if (nations_it == nations.nations.end()) {
            boost::throw_exception(
                std::runtime_error("Unknown owner nation '" + holdings.first + "' encountered in map data")
            );
        }
        const int nation_id = nations_it->second.nation_id;

        require_nonempty(holdings.second.provinces, "starting_national_holdings_t.provinces");
        for (const auto& province : holdings.second.provinces) {
            require_nonempty(province.hexes, "province_t.hexes");
            for (const auto& province_hex : province.hexes) {
                const auto hex_id = province_hex.hex;
                require_hex_coord(hex_id, map.width, map.height, "starting province hex");

                const hex_coord_t hc = to_hex_coord(hex_id);
                const int i = hex_index(hc, map.width);
                hex_t& hex = map.hexes[i];

                if (hex.coord != invalid_hex_coord) {
                    boost::throw_exception(
                        std::runtime_error("Duplicate definition of hex " + std::to_string(hex_id))
                    );
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
            std::string hex_str = boost::lexical_cast<std::string>(hex_x * 100 + hex_y);
            while (hex_str.size() < 4u) {
                hex_str = '0' + hex_str;
            }
            boost::throw_exception(std::runtime_error("Hex " + hex_str + " not defined in map.json"));
        }
    }
}

inline void validate_and_fixup_oob_unit (oob_unit_t& oob_unit)
{
    // TODO: Check that the unit is a known unit for this nation.
    require_nonempty(oob_unit.unit, "oob_unit_t.unit");
    require_nonnegative(oob_unit.times, "oob_unit_t.times");
    if (oob_unit.times == 0)
        oob_unit.times = 1;
}

inline void validate_and_fixup_starting_fleet (starting_fleet_t& starting_fleet, int width, int height)
{
    for (auto hex_id : starting_fleet.hexes) {
        require_hex_coord(hex_id, width, height, "starting fleet hex");
    }
    require_nonempty(starting_fleet.units, "starting_fleet_t.units");
    for (auto& oob_unit : starting_fleet.units) {
        validate_and_fixup_oob_unit(oob_unit);
    }
    for (auto& element : starting_fleet.prewar_construction) {
        for (auto& oob_unit : element.units) {
            validate_and_fixup_oob_unit(oob_unit);
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

inline void validate_and_fill_in_unit_times (orders_of_battle_t& oobs, const map_t& map, const nations_t& nations)
{
    require_nonempty(oobs.oobs, "orders_of_battle_t.oobs");
    for (auto& oob : oobs.oobs) {
        if (nations.nations.count(oob.first) == 0) {
            boost::throw_exception(
                std::runtime_error("Unknown owner nation '" + oob.first + "' encountered in OOB data")
            );
        }

        require_nonempty(oob.second.starting_fleets, "order_of_battle_t.starting_fleets");
        for (auto& fleet : oob.second.starting_fleets) {
            require_nonempty(fleet.first, "starting_fleets_t name ('key')");

            validate_and_fixup_starting_fleet(fleet.second, map.width, map.height);
        }

        for (auto& oob_unit : oob.second.mothball_reserve.units) {
            validate_and_fixup_oob_unit(oob_unit);
        }
        for (auto& oob_unit : oob.second.mothball_reserve.war_release) {
            validate_and_fixup_oob_unit(oob_unit);
        }
        for (auto& oob_unit : oob.second.mothball_reserve.limited_war_release) {
            validate_and_fixup_oob_unit(oob_unit);
        }

        require_nonempty(oob.second.production, "order_of_battle_t.production");
        for (auto& element : oob.second.production) {
            for (auto& oob_unit : element.units) {
                validate_and_fixup_oob_unit(oob_unit);
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
