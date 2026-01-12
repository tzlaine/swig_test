#include "lua.hpp"

#include "game_data_formatters.hpp"
#include "game_data_metadata.hpp"
#include "model_util.hpp"


void detail::lua_register_4(sol::state & l)
{
    sol::usertype<cost_t> cost_t_type = l.new_usertype<cost_t>(
        "cost_t",
        sol::no_constructor,
        sol::meta_function::to_string,
        [](cost_t const & x) { return std::format("{}", x); },
        sol::meta_function::addition,
        [](cost_t const & l, cost_t const & r) { return l + r; });
    cost_t_type["new"] = [] {
        return cost_t{
            .money_cost = 0,
            .metal_cost = 0,
            .energy_cost = 0,
            .fuel_minerals_cost = 0,
            .fuel_cost = 0,
            .water_cost = 0,
            .food_cost = 0};
    };
    detail::metadata<cost_t>::foreach_member(
        [&](auto meta) { cost_t_type[meta.name_] = meta.ptr_; });

    REGISTER_GAME_DATA_TYPE(l, resource_t);
    REGISTER_GAME_DATA_TYPE(l, day_update_t);
    REGISTER_GAME_DATA_TYPE(l, month_update_t);
    REGISTER_GAME_DATA_TYPE(l, year_update_t);
}
