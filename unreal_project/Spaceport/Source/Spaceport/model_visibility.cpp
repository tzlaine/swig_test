#include "model_visibility.hpp"

#include "model.hpp"
#include "model_util.hpp"


visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    unit_design_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id.nation_id || allied(gs, nation_id, x.id.nation_id))
        return visibility_kind::owner;
    if (std::ranges::binary_search(
            nation.foreign_designs_seen, x.id, std::ranges::less{})) {
        return visibility_kind::owner;
    }
    if (std::ranges::binary_search(
            nation.foreign_designs_glimpsed, x.id, std::ranges::less{})) {
        return visibility_kind::neutral_or_enemy;
    }
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    unit_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id.nation_id || allied(gs, nation_id, x.id.nation_id))
        return visibility_kind::owner;
    return visibility_kind::neutral_or_enemy;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    fleet_t const & x,
    int)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id.nation_id || allied(gs, nation_id, x.id.nation_id))
        return visibility_kind::owner;
    if (visible_fleet(visible_fleets, x.id)) {
        return visibility_kind::neutral_or_enemy;
    }
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    fleets_t const & x)
{
    // Return always visible here; the fleets in 'x' will be determined
    // visible or not individually.
    return visibility_kind::owner;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    planet_effect_t const & x)
{
    return visibility_kind::owner;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    planet_t const & x,
    int)
{
    nation_t const & nation = gs.nations[nation_id];
    if (std::ranges::binary_search(
            gs.nations[nation_id].systems_visited, x.system_id)) {
        return visibility_kind::neutral_or_enemy;
    }
    // TODO: Need a mechanism for using intel to determine resource amounts.
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    location_object_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (0 <= x.planet_id)
        return visibility_kind::owner;
    for (auto const & fleet_id : x.bases) {
        if (visible_fleet(visible_fleets, fleet_id))
            return visibility_kind::owner;
    }
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    system_location_t const & x)
{
    // Visibility of contents done separately.
    return visibility_kind::owner;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    star_t const & x)
{
    return visibility_kind::owner;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    system_t const & system,
    int system_index)
{
    nation_t const & nation = gs.nations[nation_id];
    if (std::ranges::binary_search(
            gs.nations[nation_id].systems_present_in, system_index)) {
        return visibility_kind::owner;
    }
    if (std::ranges::binary_search(
            gs.nations[nation_id].systems_visited, system_index)) {
        return visibility_kind::owner;
    }
    if (std::ranges::binary_search(
            gs.nations[nation_id].hexes_seen, system.hex_id)) {
        return visibility_kind::neutral_or_enemy;
    }
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    hex_t const & x,
    int)
{
    nation_t const & nation = gs.nations[nation_id];
    if (std::ranges::binary_search(
            nation.hexes_seen, to_index(x.coord, gs.map_width))) {
        return visibility_kind::owner;
    }
    // TODO: What about the province_id?
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    province_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id.nation_id || allied(gs, nation_id, x.id.nation_id))
        return visibility_kind::owner;
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    nation_t const & x,
    int)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id || allied(gs, nation_id, x.id))
        return visibility_kind::owner;
    for (auto * f : visible_fleets) {
        if (f->id.nation_id == x.id)
            return visibility_kind::neutral_or_enemy;
    }
    for (auto id : nation.foreign_designs_seen) {
        if (id.nation_id == x.id)
            return visibility_kind::neutral_or_enemy;
    }
    for (auto id : nation.foreign_designs_glimpsed) {
        if (id.nation_id == x.id)
            return visibility_kind::neutral_or_enemy;
    }
    return visibility_kind::unseen;
}
