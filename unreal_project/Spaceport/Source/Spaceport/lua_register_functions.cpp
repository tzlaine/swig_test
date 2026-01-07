#include "lua.hpp"

#include "hex_operations.hpp"
#include "rng.hpp"


void detail::lua_register_functions(sol::state & l)
{
    // hex_operations.hpp
    l["cpp_to_index"] = &to_index;
    l["cpp_from_index"] = &from_index;
    l["cpp_on_map"] = &on_map;
    l["hex_above_right"] = &hex_above_right;
    l["hex_above"] = &hex_above;
    l["hex_above_left"] = &hex_above_left;
    l["hex_below_left"] = &hex_below_left;
    l["hex_below"] = &hex_below;
    l["hex_below_right"] = &hex_below_right;

    // rng.hpp
    l["random_int"] = [](int lo, int hi) { return random_int(lo, hi); };
    l["random_double"] = [](double lo, double hi) {
        return random_double(lo, hi);
    };
    l["random_unit_double"] = [] { return random_unit_double(); };
}
