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
