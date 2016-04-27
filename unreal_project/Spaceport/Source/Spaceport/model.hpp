#pragma once

#include "model_data.hpp"
#include <boost/utility.hpp>


struct start_data_t
{
    start_data_t () :
        nations_initialized_ (false),
        unit_defs_initialized_ (false),
        scenario_initialized_ (false)
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

    char const * hex_string (hex_coord_t hc) const;

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
};
