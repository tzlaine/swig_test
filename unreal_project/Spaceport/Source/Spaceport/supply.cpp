#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "supply.hpp"

#include "data_utility.hpp"
#include "graph.hpp"

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>


namespace {

    struct supply_relevant_contents_t
    {
        bool neutral_zone;
        int friendly_ships;
        int friendly_units;
        int friendly_bases;
        int enemy_ships;
        int enemy_units;
        int enemy_bases;
    };

    struct supply_determination_hex_t
    {
        bool already_assigned_grid;
        bool supply_source;
        bool supply_point;
        bool impassable;
        supply_relevant_contents_t supply_relevant_contents;
    };

    supply_relevant_contents_t find_supply_relevant_contents (
        hex_t const & hex,
        int nation_id,
        team_t const * team,
        start_data::start_data_t const & start_data,
        game_data_t const & game_data,
        int nz_nation_id
    ) {
        supply_relevant_contents_t retval = {0};

        retval.neutral_zone = neutral_zone(hex, game_data.map(), nz_nation_id);

        for (auto const & zone : hex.zones) {
            for (auto const & fixture : zone.fixtures) {
                if (fixture.type == hex_zone_fixture_t::type_t::type_base &&
                    (6 <= fixture.base.fighters || 6 <= fixture.base.pfs)) {
                    if (team && part_of_team(fixture.base.owner, *team) ||
                        fixture.base.owner == nation_id) {
                        ++retval.friendly_bases;
                    } else if (team && game_data.at_war_with(team->name, fixture.base.owner) ||
                               fixture.base.owner != nation_id) {
                        ++retval.enemy_bases;
                    }
                }
            }
        }

        for (auto const & pair : hex.fleets.fleets) {
            int const fleet_nation_id = pair.first;
            auto const & fleet = pair.second;
            for (auto const & unit : fleet.units) {
                auto const ship = is_ship(unit, start_data);
                if (team && part_of_team(unit.owner, *team) || fleet_nation_id == nation_id) {
                    if (ship)
                        ++retval.friendly_ships;
                    else
                        ++retval.friendly_units;
                } else if (team && game_data.at_war_with(team->name, unit.owner) ||
                           fleet_nation_id != nation_id) {
                    if (ship)
                        ++retval.friendly_ships;
                    else
                        ++retval.friendly_units;
                }
            }
        }

        return retval;
    }

    template <typename Graph, typename Edge, typename Vertex, std::size_t N>
    struct bfs_visitor_t
    {
        bfs_visitor_t (std::array<int, N> & distances) :
            distances_ (distances),
            source_ (-1)
        {}

        void initialize_vertex (const Vertex& v, const Graph& g) {}

        void discover_vertex (const Vertex& v, const Graph& g)
        {
            int d = 0;
            if (source_ != -1)
                d = distances_[source_] + 1;
            distances_[v] = d;
        }

        void examine_vertex (const Vertex& v, const Graph& g)
        { source_ = v; }

        void examine_edge (const Edge& e, const Graph& g) {}
        void tree_edge (const Edge& e, const Graph& g) {}

        void non_tree_edge (const Edge& e, const Graph& g) {}
        void gray_target (const Edge& e, const Graph& g) {}
        void black_target (const Edge& e, const Graph& g) {}
        void finish_vertex (const Vertex& e, const Graph& g) {}

        std::array<int, N> & distances_;
        int source_;
    };

}

bool supply_source (int nation_id, hex_t const & hex, map_t const & map)
{
    for (auto const & zone : hex.zones) {
        for (auto const & fixture : zone.fixtures) {
            if (fixture.type == hex_zone_fixture_t::type_t::type_planet) {
                if (fixture.planet.owner == nation_id)
                    return true;
            } else if (fixture.base.owner == nation_id &&
                       (is_bats(fixture.base) || is_sb(fixture.base))) {
                return true;
            }
        }
    }

    return false;
}

bool supply_point (int nation_id, hex_t const & hex, map_t const & map)
{
    for (auto const & zone : hex.zones) {
        for (auto const & fixture : zone.fixtures) {
            if (fixture.type == hex_zone_fixture_t::type_t::type_planet) {
                if (fixture.planet.owner == nation_id)
                    return true;
            } else if (fixture.base.owner == nation_id) {
                return true;
            }
        }
    }

    auto it = hex.fleets.fleets.find(nation_id);
    if (it == hex.fleets.fleets.end())
        return false;

    auto const & fleet = it->second;
    for (auto const & unit : fleet.units) {
        if (is_convoy(unit) || unit.tug_mission == tug_mission_t::D_supply)
            return true;
    }

    return false;
}

std::vector<supply_grid_t> find_supply_grids (
    int nation_id,
    start_data::start_data_t const & start_data,
    game_data_t const & game_data
) {
    std::vector<supply_grid_t> retval;

    auto const team = game_data.team(nation_id, start_data);

    auto const & game_map = game_data.map();

    auto const nz_nation_id = start_data.nation_id("NZ"_name);

    std::vector<supply_determination_hex_t> supply_determination_hexes(
        game_map.width * game_map.height,
        supply_determination_hex_t{false, false, false, false, {false}}
    );

    std::vector<hex_index_t> supply_sources;

    {
        auto game_hex_it = game_map.hexes.begin();
        auto supply_hex_it = supply_determination_hexes.begin();
        auto const supply_hex_end = supply_determination_hexes.end();
        hex_index_t i(0);
        for (; supply_hex_it != supply_hex_end;
             ++game_hex_it, ++supply_hex_it, i = hex_index_t(i + 1)) {
            auto const & h = *game_hex_it;
            supply_hex_it->supply_relevant_contents = find_supply_relevant_contents(
                h,
                nation_id,
                team,
                start_data,
                game_data,
                nz_nation_id
            );
            supply_hex_it->supply_source = supply_source(nation_id, h, game_map);
            if (supply_hex_it->supply_source) {
                supply_sources.push_back(i);
                supply_hex_it->supply_point = true; // TODO: Exception: not if it's a disconencted base in enemy territory.(?)
            } else {
                supply_hex_it->supply_point = supply_point(nation_id, h, game_map);
            }
        }
    }

    auto const width = start_data.map().width;
    auto const height = start_data.map().height;

    {
        hex_index_t i(0);
        for (auto & hex : supply_determination_hexes) {
            auto const hc = i.to_hex_coord(width);
            i = hex_index_t(i + 1);

            auto const & contents = hex.supply_relevant_contents;

            if (contents.neutral_zone) {
                hex.impassable = true;
                continue;
            }

            bool const unblocked_by_contents =
                contents.friendly_units || contents.friendly_ships || contents.friendly_bases;
            if (unblocked_by_contents)
                continue;

            bool const blocked_by_contents =
                contents.enemy_units || contents.enemy_ships || contents.enemy_bases;
            if (blocked_by_contents) {
                hex.impassable = true;
                continue;
            }

            bool blocked_by_adjacent_contents = false;
            bool unblocked_by_adjacent_contents = false;
            for (hex_direction_t d : all_hex_directions) {
                auto const adjacent_coord = adjacent_hex_coord(hc, d);
                if (on_map(adjacent_coord, width, height)) {
                    hex_index_t const hex_index(adjacent_coord, width);
                    auto const adjacent_hex = supply_determination_hexes[hex_index];
                    auto const & adjacent_contents = adjacent_hex.supply_relevant_contents;
                    unblocked_by_adjacent_contents |=
                        adjacent_contents.friendly_ships;
                    blocked_by_adjacent_contents |=
                        adjacent_contents.enemy_ships ||
                        adjacent_contents.enemy_bases;
                }
            }

            if (unblocked_by_adjacent_contents)
                continue;

            if (blocked_by_adjacent_contents)
                hex.impassable = true;
        }
    }

    std::sort(supply_sources.begin(), supply_sources.end());

    std::vector<hex_index_t> supply_source_stack;
    std::vector<hex_index_t> supply_point_stack;
    if (!supply_sources.empty()) {
        auto const starting_capital = start_data.starting_national_capital(nation_id); // TODO: Take into account moved (non-offmap) capitals....
        auto const starting_capital_index = starting_capital.to_hex_index(width);
        auto const it = std::lower_bound(
            supply_sources.begin(), supply_sources.end(),
            starting_capital_index
        );
        if (it != supply_sources.end() && *it == starting_capital_index) {
            supply_source_stack.push_back(starting_capital_index);
            supply_sources.erase(it);

            // TODO: Don't start the "capital grid" here if the actual capital
            // planet has been captured.
            retval.push_back(supply_grid_t{grid_type_t::capital});
            auto & grid = retval.back();

            auto const capital_hc = starting_capital_index.to_hex_coord(width);

            while (!supply_source_stack.empty() || !supply_point_stack.empty()) {
                auto const starting_hex_index = supply_source_stack.empty() ?
                    supply_point_stack.back() : supply_source_stack.back();
                if (supply_source_stack.empty())
                    supply_point_stack.pop_back();
                else
                    supply_source_stack.pop_back();

                auto const starting_hc = starting_hex_index.to_hex_coord(width);

                graph::graph_t g;
                graph::hex_id_property_map_t hex_id_property_map;
                graph::edge_weight_property_map_t edge_weight_map;
                init_graph(
                    g,
                    hex_id_property_map,
                    edge_weight_map,
                    starting_hc,
                    [&](hex_coord_t hc) {
                        auto const & hex = supply_determination_hexes[hex_index_t(hc, width)];
                        return !hex.already_assigned_grid && !hex.impassable;
                    },
                    [](hex_coord_t lhs, hex_coord_t rhs) { return 1.0f; },
                    width,
                    height
                );

                grid.supply_points.push_back(starting_hc);
                grid.hexes_in_supply.push_back(starting_hc);
                auto & starting_hex = supply_determination_hexes[hex_index_t(starting_hc, width)];
                starting_hex.already_assigned_grid = true;

                boost::queue<int> buf;
                std::array<int, max_neighbors> distances = {{0}};
                std::array<int, max_neighbors> colors = {{0}};

                using visitor_t = bfs_visitor_t<
                    graph::graph_t,
                    typename boost::graph_traits<graph::graph_t>::edge_descriptor,
                    int,
                    max_neighbors
                >;

                boost::breadth_first_search(
                    g,
                    vertex(0, g),
                    buf,
                    visitor_t{distances},
                    &colors[0]
                );

                auto const vertices = (int)num_vertices(g);
                for (int i = 1; i < vertices; ++i) {
                    if (0 < distances[i] && distances[i] <= max_useful_hex_distance) {
                        hex_id_t const hex_id(hex_id_property_map[i]);
                        auto const hex_index = hex_id.to_hex_index(width);
                        auto & hex = supply_determination_hexes[hex_index];

                        bool enqueued = false;

                        // If the source of this BFS is a source (as opposed
                        // to just a point), then extend the grid to all
                        // sources and points found.
                        if (supply_determination_hexes[starting_hex_index].supply_source) {
                            if (hex.supply_source) {
                                auto it = std::lower_bound(
                                    supply_source_stack.begin(), supply_source_stack.end(),
                                    hex_index
                                );
                                if (it == supply_source_stack.end() || *it != hex_index)
                                    supply_source_stack.insert(it, hex_index);
                                enqueued = true;
                            } else if (hex.supply_point) {
                                auto it = std::lower_bound(
                                    supply_point_stack.begin(), supply_point_stack.end(),
                                    hex_index
                                );
                                if (it == supply_point_stack.end() || *it != hex_index)
                                    supply_point_stack.insert(it, hex_index);
                                enqueued = true;
                            }
                        }

                        if (!enqueued) {
                            grid.hexes_in_supply.push_back(hex_id.to_hex_coord());
                            hex.already_assigned_grid = true;
                        }
                    }
                }

                // TODO: Add provinces in supply?
            }
        }
    }

    // TODO: Reuse the above, and start at the offmap area.

    while (!supply_sources.empty()) {
        supply_sources.pop_back();

        // TODO: Reuse the above, and start here.
    }

    return retval;
}
