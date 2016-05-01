#pragma once

#include "start_data_t.hpp"
#include "game_data_t.hpp"


inline int owner (hex_coord_t hc, start_data::start_data_t const & start_data, game_data_t const & game_data)
{
    int const province_id = start_data.hex_province(hc);
    if (province_id == -1)
        return start_data.neutral_zone_id();
    return game_data.province(province_id)->owner;
}

inline int stored_mb_unit_id () { return 0; }
inline int deployed_mb_unit_id () { return 1; }

inline int early_bats_unit_id () { return 2; }
inline int early_sb_unit_id () { return 3; }
inline int early_pdu_unit_id () { return 4; }

inline int late_bats_unit_id () { return 5; }
inline int late_sb_unit_id () { return 6; }
inline int late_pdu_unit_id () { return 7; }

inline int repair_unit_id () { return 8; }
inline int convoy_unit_id () { return 9; }
inline int frd_unit_id () { return 10; }

inline int capital_planet_starting_pdus () { return 8; }
inline int major_planet_starting_pdus () { return 4; }
inline int minor_planet_starting_pdus () { return 2; }

inline bool is_mb (unit_t unit)
{ return unit.unit_id == deployed_mb_unit_id(); }

inline bool is_bats (unit_t unit)
{ return unit.unit_id == early_bats_unit_id() || unit.unit_id == late_bats_unit_id(); }

inline bool is_sb (unit_t unit)
{ return unit.unit_id == early_sb_unit_id() || unit.unit_id == late_sb_unit_id(); }

inline bool is_pdu (unit_t unit)
{ return unit.unit_id == early_pdu_unit_id() || unit.unit_id == late_pdu_unit_id(); }
