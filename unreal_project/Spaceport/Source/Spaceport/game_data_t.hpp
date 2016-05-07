#pragma once

#include "game_data.hpp"
#include "hex_operations.hpp"

#include <boost/utility.hpp>
#include <boost/container/flat_set.hpp>


namespace start_data { struct start_data_t; }

struct game_data_t
{
    using team_map_t = boost::container::flat_map<name_t, team_t>;

    game_data_t ();
    game_data_t (start_data::start_data_t const & start_data);

    hex_t const & hex (hex_coord_t hc) const
    {
        auto const i = to_hex_index(hc, map_.width);
        assert(i < (int)map_.hexes.size());
        return map_.hexes[i];
    }

    province_t const * province (int province_id) const
    {
        if (province_id < 0 || (int)map_.provinces.size() < province_id)
            return nullptr;
        return &map_.provinces[province_id];
    }

    map_t const & map () const
    { return map_; }

    offmap_areas_t const & offmap_areas () const
    { return offmap_areas_; }

    team_map_t const & teams () const
    { return teams_; }

    team_t const & team (name_t team_name) const
    { assert(teams_.count(team_name) != 0); return teams_.find(team_name)->second; }

    team_t const * team (int nation_id, start_data::start_data_t const & start_data) const
    {
        name_t const nation_key(start_data.nation_key(nation_id).c_str()); // TODO: nation_key -> name_t
        auto const it = teams_.find(nation_key);
        if (it == teams_.end())
            return nullptr;
        return &it->second;
    }

    bool at_war_with (name_t team, int nation_id) const
    {
        reusable_elem_.team_ = team;
        reusable_elem_.nation_ = nation_id;
        return team_and_nation_at_war_.count(reusable_elem_);
    }

    bool at_war_with (int nation_lhs, int nation_rhs) const
    { return nations_at_war_.count(nations_at_war_elem_t(nation_lhs, nation_rhs)) != 0; }

    void declare_war (
        int nation_id,
        int enemy_nation_id,
        start_data::start_data_t const & start_data
    );

    void leave_war (int nation_id, start_data::start_data_t const & start_data);

private:
    map_t map_;
    offmap_areas_t offmap_areas_;
    team_map_t teams_;
    boost::container::flat_map<int, name_t> nation_teams_;

    struct team_and_nation_at_war_elem_t
    {
        name_t team_;
        int nation_;
    };
    friend bool operator< (
        team_and_nation_at_war_elem_t const & lhs,
        team_and_nation_at_war_elem_t const & rhs
    ) {
        if (lhs.team_ < rhs.team_)
            return true;
        if (rhs.team_ < lhs.team_)
            return false;
        return lhs.nation_ < rhs.nation_;
    }
    // TODO: Replace this hack with something like adobe::name_t.
    mutable team_and_nation_at_war_elem_t reusable_elem_;
    boost::container::flat_set<team_and_nation_at_war_elem_t> team_and_nation_at_war_;

    struct nations_at_war_elem_t
    {
        nations_at_war_elem_t (int nation_id_lhs, int nation_id_rhs) :
            lhs_ ((std::min)(nation_id_lhs, nation_id_rhs)),
            rhs_ ((std::max)(nation_id_lhs, nation_id_rhs))
        {
            assert(0 <= nation_id_lhs);
            assert(0 <= nation_id_rhs);
        }

        int lhs_;
        int rhs_;
    };
    friend bool operator< (
        nations_at_war_elem_t const & lhs,
        nations_at_war_elem_t const & rhs
    ) {
        if (lhs.lhs_ < rhs.lhs_)
            return true;
        if (rhs.lhs_ < lhs.lhs_)
            return false;
        return lhs.rhs_ < rhs.rhs_;
    }
    boost::container::flat_set<nations_at_war_elem_t> nations_at_war_;
};
