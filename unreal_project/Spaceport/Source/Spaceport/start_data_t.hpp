#pragma once

#include "start_data.hpp"
#include "hex_operations.hpp"
#include <boost/utility.hpp>

namespace start_data {

    struct start_data_t
    {
        start_data_t () :
            nations_initialized_ (false),
            unit_defs_initialized_ (false),
            scenario_initialized_ (false),
            num_provinces_ (0)
        {}

        nations_t const & nations () const
        { assert(nations_initialized_); return nations_; }

        unit_defs_t const & unit_defs () const
        { assert(unit_defs_initialized_); return unit_defs_; }

        scenario_t const & scenario () const
        { assert(scenario_initialized_); return scenario_; }

        map_t const & map () const
        { assert(scenario_initialized_); return map_; }

        orders_of_battle_t const & oob () const
        { assert(scenario_initialized_); return oob_; }

        std::string const & nation_key (int i) const
        { return boost::next(nations_.nations.begin(), i)->first; }

        nation_t const & nation (int i) const
        { return boost::next(nations_.nations.begin(), i)->second; }

        nation_t const & nation (std::string const & key) const
        { return nations_.nations.find(key)->second; }

        int neutral_zone_id () const
        { return nation("NZ").nation_id; }

        char const * hex_string (hex_coord_t hc) const;

        int num_provinces() const
        { return num_provinces_; }

        struct const_province_hex_range
        {
            using iterator = boost::container::flat_multimap<int, hex_coord_t>::const_iterator;
            iterator begin () const { return first; }
            iterator end() const { return last; }
            iterator first, last;
        };

        const_province_hex_range province_hexes (int province_id) const
        {
            auto const pair = province_hexes_.equal_range(province_id);
            return const_province_hex_range{pair.first, pair.second};
        }

        int hex_province (hex_coord_t hc) const
        {
            auto const it = provinces_.find(hc);
            if (it == provinces_.end())
                return -1;
            return it->second;
        }

        void init_nations (std::string const & nations_str);
        void init_unit_defs (std::string const & unit_defs_str);

        template <typename Fn1, typename Fn2>
        void init_scenario (std::string const & scenario_str, Fn1 get_map_str, Fn2 get_oob_str)
        {
            assert(!scenario_initialized_);
            parse_scenario_message(scenario_str);
            auto const map_str = get_map_str(scenario_.map);
            init_map(map_str);
            auto const oob_str = get_oob_str(scenario_.order_of_battle);
            init_oob(oob_str);
            init_scenario_impl();
        }

    private:
        void parse_scenario_message (std::string const & scenario_str);
        void init_map (std::string const & map_str);
        void init_oob (std::string const & oob_str);
        void init_scenario_impl ();

        nations_t nations_;
        bool nations_initialized_;

        unit_defs_t unit_defs_;
        bool unit_defs_initialized_;

        scenario_t scenario_;
        map_t map_;
        orders_of_battle_t oob_;
        bool scenario_initialized_;

        std::vector<std::string> hex_strings_;

        int num_provinces_;
        boost::container::flat_multimap<int, hex_coord_t> province_hexes_;
        boost::container::flat_map<hex_coord_t, int> provinces_;
    };

}
