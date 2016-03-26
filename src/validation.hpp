#pragma once
#include "model_data.hpp"
#include "hex_operations.hpp"

#include <boost/exception/diagnostic_information.hpp>

#include <boost/lexical_cast.hpp>


inline void require_boolean_int(int i, const std::string& name)
{
    if (i != 0 && i != 1) {
        boost::throw_exception(
            std::runtime_error(
                name + " (=" + std::to_string(i) +") must be 1 for true or 0 for false"
            )
        );
    }
}

inline void require_nonempty(const std::string& s, const std::string& name)
{
    if (s.empty())
        boost::throw_exception(std::runtime_error(name + " must be provided and must not be empty"));
}

inline void require_nonnegative(int i, const std::string& name)
{
    if (i < 0) {
        boost::throw_exception(
            std::runtime_error(name + " (=" + std::to_string(i) + ") must be nonnegative")
        );
    }
}

inline void require_within(int i, int low, int high, const std::string& name)
{
    if (i < low || high < i) {
        boost::throw_exception(
            std::runtime_error(
                name + " (=" + std::to_string(i) + ") must be between " + std::to_string(low) +
                " and " + std::to_string(high) + " inclusive"
            )
        );
    }
}

inline void require_hex_coord(int i, int width, int height, const std::string& name)
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

inline void validate_capital (const capital_t& capital)
{
    for (const auto& hex : capital.hexes) {
        for (const auto& zone : hex.zones) {
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
    require_boolean_int(offmap_possesions.cannot_build_offmap_capital, "offmap_possesions_t.cannot_build_offmap_capital");
    require_boolean_int(offmap_possesions.old_shipyard, "offmap_possesions_t.old_shipyard");
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
    for (const auto& n : nations.nations) {
        validate_nation(n.second);
    }
}

inline void validate_hex_coords (const nations_t& nations, int width, int height)
{
    // TODO: This obviously needs to be called after loading the map
    for (const auto& n : nations.nations) {
        for (const auto& hex : n.second.capital.hexes) {
            require_hex_coord(hex.coord, width, height, "capital_hex_t.coord");
        }
    }
    // TODO: other hex coords
}

void validate_and_fill_in_map_hexes (map_t& map, const nations_t& nations)
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

        for (const auto& province : holdings.second.provinces) {
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
