#include "model_visibility.hpp"

#include "effects.hpp"
#include "model.hpp"


// TODO: Maybe combine the logic in visibility_of() with the serialization
// code itself.

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, unit_design_t const & x)
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

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, unit_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id.nation_id || allied(gs, nation_id, x.id.nation_id))
        return visibility_kind::owner;
    // TODO: Return visibility_kind::neutral_or_enemy iff this unit's fleet is
    // visible.
    return visibility_kind::unseen;
}

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, fleet_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id.nation_id || allied(gs, nation_id, x.id.nation_id))
        return visibility_kind::owner;
    // TODO: Need to refactor the visibility grid from the repl graph to use
    // the same logic here.
    return visibility_kind::unseen;
}

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, fleets_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    // TODO: Need to refactor the visibility grid from the repl graph to use
    // the same logic here (same logic as for fleet_t above).
    return visibility_kind::unseen;
}

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, planet_effect_t const & x)
{
    if (!transitory(x))
        return visibility_kind::owner;
    return visibility_kind::unseen;
}

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, planet_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.owner || allied(gs, nation_id, x.owner))
        return visibility_kind::owner;
    // TODO: Need a mechanism for using intel to determine resource amounts.
    return visibility_kind::neutral_or_enemy;
}

visibility_kind visibility_of(
    game_state_t const & gs, int nation_id, location_object_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    // TODO
    return visibility_kind::unseen;
}

visibility_kind visibility_of(
    game_state_t const & gs, int nation_id, system_location_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    // TODO, but but in-system fleets should be harder to detect, due to the
    // star, stations and planets
    return visibility_kind::unseen;
}

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, star_t const & x)
{
    return visibility_kind::owner;
}

visibility_kind visibility_of(
    game_state_t const & gs,
    int nation_id,
    system_t const & x,
    int system_index)
{
    nation_t const & nation = gs.nations[nation_id];
    // TODO: Keep sorted?
    if (std::ranges::find(nation.systems_seen, system_index) !=
        nation.systems_seen.end()) {
        return visibility_kind::owner;
    }
    return visibility_kind::unseen;
}

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, hex_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    // TODO: Keep sorted?
    if (std::ranges::find(nation.hexes_seen, to_index(x.coord, gs.map_width)) !=
        nation.hexes_seen.end()) {
        return visibility_kind::owner;
    }
    return visibility_kind::unseen;
}

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, province_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id.nation_id || allied(gs, nation_id, x.id.nation_id))
        return visibility_kind::owner;
    // TODO
    return visibility_kind::unseen;
}

visibility_kind
visibility_of(game_state_t const & gs, int nation_id, nation_t const & x)
{
    nation_t const & nation = gs.nations[nation_id];
    if (nation_id == x.id || allied(gs, nation_id, x.id))
        return visibility_kind::owner;
    return visibility_kind::unseen;
}

unit_design_t
view_of(game_state_t const & gs, int nation_id, unit_design_t const & x)
{
    return x; // TODO
}

unit_t view_of(game_state_t const & gs, int nation_id, unit_t const & x)
{
    return x; // TODO
}

fleet_t view_of(game_state_t const & gs, int nation_id, fleet_t const & x)
{
    return x; // TODO
}

fleets_t view_of(game_state_t const & gs, int nation_id, fleets_t const & x)
{
    return x; // TODO
}

planet_effect_t
view_of(game_state_t const & gs, int nation_id, planet_effect_t const & x)
{
    return x; // TODO
}

planet_t view_of(game_state_t const & gs, int nation_id, planet_t const & x)
{
    return x; // TODO
}

location_object_t
view_of(game_state_t const & gs, int nation_id, location_object_t const & x)
{
    return x; // TODO
}

system_location_t
view_of(game_state_t const & gs, int nation_id, system_location_t const & x)
{
    return x; // TODO
}

star_t view_of(game_state_t const & gs, int nation_id, star_t const & x)
{
    return x; // TODO
}

system_t view_of(game_state_t const & gs, int nation_id, system_t const & x)
{
    return x; // TODO
}

hex_t view_of(game_state_t const & gs, int nation_id, hex_t const & x)
{
    return x; // TODO
}

province_t view_of(game_state_t const & gs, int nation_id, province_t const & x)
{
    return x; // TODO
}

nation_t view_of(game_state_t const & gs, int nation_id, nation_t const & x)
{
    return x; // TODO
}
