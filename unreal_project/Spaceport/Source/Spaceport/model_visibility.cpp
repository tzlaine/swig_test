#include "model_visibility.hpp"

#include "effects.hpp"
#include "model.hpp"


visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    unit_design_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id.nation_id || allied(gs, nation_id, x.id.nation_id))
        return visibility_kind::owner;
    // TODO: Keep these sorted?
    if (std::ranges::find(nation.foreign_designs_seen, x.id) !=
        nation.foreign_designs_seen.end()) {
        return visibility_kind::owner;
    }
    if (std::ranges::find(nation.foreign_designs_glimpsed, x.id) !=
        nation.foreign_designs_seen.end()) {
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
    if (!transitory(x))
        return visibility_kind::owner;
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    std::vector<fleet_t const *> const & visible_fleets,
    int nation_id,
    planet_t const & x,
    int)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.owner || allied(gs, nation_id, x.owner))
        return visibility_kind::owner;
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
    system_t const & x,
    int system_index)
{
    nation_t const & nation = gs.nations[nation_id];
    // TODO: Keep sorted?
    int const hex_index = to_index(x.coord, gs.map_width);
    if (std::ranges::find(nation.systems_visited, system_index) !=
        nation.systems_visited.end()) {
        return visibility_kind::owner;
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
    // TODO: Keep sorted?
    if (std::ranges::find(nation.hexes_seen, to_index(x.coord, gs.map_width)) !=
        nation.hexes_seen.end()) {
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
    // TODO: This should return visibility_kind::neutral_or_enemy if nation_id
    // can see any of their fleets.
    return visibility_kind::unseen;
}
