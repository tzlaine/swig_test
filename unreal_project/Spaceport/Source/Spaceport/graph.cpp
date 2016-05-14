#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "graph.hpp"
#include "data_utility.hpp"

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>


namespace {

    inline int hex_owner (hex_t const & hex, map_t const & map)
    { return map.provinces[hex.province_id].owner; }

    inline bool neutral_zone (hex_t const & hex, map_t const & map, int nz_nation_id)
    { return hex_owner(hex, map) == nz_nation_id; }

    inline bool supply_source (hex_t const & hex, map_t const & map, int nation_id)
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

    inline bool supply_point (hex_t const & hex, map_t const & map, int nation_id)
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

    struct supply_relevant_contents_t
    {
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
        game_data_t const & game_data
    ) {
        supply_relevant_contents_t retval = {0};

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
        void discover_vertex (const Vertex& v, const Graph& g) {}

        void examine_vertex (const Vertex& v, const Graph& g)
        {
            int d = 0;
            if (source_ != -1)
                d = distances_[source_] + 1;
            distances_[v] = d;
            source_ = v;
        }

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

// TODO: These need to move to a supply-centric header.
enum class grid_type_t {
    capital,
    offmap,
    partial
};

enum class grid_check_time_t {
    during_combat,
    not_during_combat
};

struct supply_grid_t
{
    grid_type_t type;

    std::vector<hex_coord_t> supply_points;
    std::vector<hex_coord_t> hexes_in_supply;

    // TODO
};

std::vector<supply_grid_t> get_supply_grids (
    int nation_id,
    start_data::start_data_t const & start_data,
    game_data_t const & game_data,
    bool during_combat
) {
    std::vector<supply_grid_t> retval;

    auto const team = game_data.team(nation_id, start_data);

    auto const & game_map = game_data.map();

    std::vector<supply_determination_hex_t> supply_determination_hexes(
        game_map.width * game_map.height,
        supply_determination_hex_t{false, false, false, false, {0}}
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
            supply_hex_it->supply_relevant_contents =
                find_supply_relevant_contents(h, nation_id, team, start_data, game_data);
            supply_hex_it->supply_source = supply_source(h, game_map, nation_id);
            if (supply_hex_it->supply_source) {
                supply_sources.push_back(i);
                supply_hex_it->supply_point = true; // TODO: Exception: not if it's a disconencted base in enemy territory.(?)
            } else {
                supply_hex_it->supply_point = supply_point(h, game_map, nation_id);
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

            bool const unblocked_by_contents =
                contents.friendly_units || contents.friendly_ships || contents.friendly_bases;
            if (unblocked_by_contents)
                continue;

            bool const blocked_by_contents = contents.enemy_units || contents.enemy_ships;
            if (blocked_by_contents) {
                hex.impassable = true;
                continue;
            }

            bool blocked_by_adjacent_contents = false;
            bool unblocked_by_adjacent_contents = false;
            for (hex_direction_t d : all_hex_directions) {
                auto const adjacent_coord = adjacent_hex_coord(hc, d);
                hex_index_t const hex_index(adjacent_coord, width);
                auto const adjacent_hex = supply_determination_hexes[hex_index];
                auto const & adjacent_contents = adjacent_hex.supply_relevant_contents;
                unblocked_by_adjacent_contents |=
                    adjacent_contents.friendly_ships;
                blocked_by_adjacent_contents |=
                    adjacent_contents.enemy_ships ||
                    adjacent_contents.enemy_bases;
            }

            if (unblocked_by_adjacent_contents)
                continue;

            if (blocked_by_adjacent_contents)
                hex.impassable = true;
        }
    }

    std::sort(supply_sources.begin(), supply_sources.end());

    std::vector<hex_index_t> supply_point_stack;
    if (!supply_sources.empty()) {
        auto const starting_capital = start_data.starting_national_capital(nation_id); // TODO: Take into account moved (non-offmap) capitals....
        auto const starting_capital_index = starting_capital.to_hex_index(width);
        auto const it = std::lower_bound(
            supply_sources.begin(), supply_sources.end(),
            starting_capital_index
        );
        if (it != supply_sources.end() && *it == starting_capital_index) {
            supply_point_stack.push_back(starting_capital_index);
            supply_sources.erase(it);

            // TODO: Don't start the "capital grid" here if the actual capital
            // planet has been captured.
            retval.push_back(supply_grid_t{grid_type_t::capital});

            auto & grid = retval.back();
            while (!supply_point_stack.empty()) {
                auto const hex_index = supply_point_stack.back();
                supply_point_stack.pop_back();

                graph::graph_t g;
                graph::hex_id_property_map_t hex_id_property_map;
                graph::edge_weight_property_map_t edge_weight_map;
                init_graph(
                    g,
                    hex_id_property_map,
                    edge_weight_map,
                    hex_index.to_hex_coord(width),
                    [&](hex_coord_t hc) {
                        auto const & hex = supply_determination_hexes[hex_index];
                        return !hex.already_assigned_grid && !hex.impassable;
                    },
                    [](hex_coord_t lhs, hex_coord_t rhs) { return 1.0f; },
                    width,
                    height
                );

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
                    if (distances[i] <= max_useful_hex_distance) {
                        hex_id_t const hex_id(hex_id_property_map[i]);
                        grid.hexes_in_supply.push_back(hex_id.to_hex_coord());
                    }
                }
                // TODO: Add provinces in supply?
                
                if (supply_determination_hexes[hex_index].supply_source) {
                    // TODO: Add discovered supply points & sources to stack;
                    // points must go to the bottom of the stack.
                }
            }
        }
    }

#if 0 // Adapt this to start at the offmap area.
    if (!supply_sources.empty()) {
        auto const starting_capital = start_data.starting_national_capital(nation_id); // TODO: Take into account moved (non-offmap) capitals....
        auto const it = std::lower_bound(
            supply_sources.begin(), supply_sources.end(),
            starting_capital
        );
        if (it != supply_sources.end()) {
            hex_index_stack.push_back(starting_capital);
            supply_sources.erase(it);

            retval.push_back(supply_grid_t{grid_type_t::offmap});

            auto & grid = retval.back();
            while (!hex_index_stack.empty()) {
                int const hex_index = hex_index_stack.back();
                hex_index_stack.pop_back();
                // TODO     
            }
        }
    }
#endif

#if 0
    while (!supply_sources.empty()) {
        hex_index_stack.push_back(supply_sources.back());
        supply_sources.pop_back();

        retval.push_back(supply_grid_t{grid_type_t::partial});

        auto & grid = retval.back();
        while (!hex_index_stack.empty()) {
            int const hex_index = hex_index_stack.back();
            hex_index_stack.pop_back();
            // TODO     
        }
    }
#endif

    return retval;
}

#if 0
bool supply_blocked (
    hex_coord_t hc,
    int nation,
    int nations,
    const int* nation_team_membership,
    const supply_check_hex_t* hexes,
    int width,
    int height,
    int neutral_zone_id
) {
    bool retval = true;
    int hex_index_ = to_hex_index(hc, width);
    supply_check_hex_t hex = hexes[hex_index_];

    bool friendly_ships = false;
    bool friendly_units = false;
    bool friendly_base = false;
    bool enemy_ships = false;
    bool enemy_units = false;
    bool enemy_base = false;

    const int team = nation_team_membership[nation];

    find_blocking_contents(
        hex,
        team,
        nations,
        nation_team_membership,
        friendly_ships,
        friendly_units,
        friendly_base,
        enemy_ships,
        enemy_units,
        enemy_base
    );

    if (hex.owner_id == neutral_zone_id) {
        retval = true;
    } else if (friendly_units) { // TODO: friendly units vs. enemy unit checks only applies during combat.
        retval = false;
    } else if (enemy_units) {
        retval = true;
    } else if (friendly_base) {
        retval = false;
    } else {
        bool adjacent_enemy_ships = false;
        bool adjacent_friendly_units = false;
        for (hex_direction_t d : all_hex_directions) {
            hex_coord_t adjacent_coord = adjacent_hex_coord(hc, d);
            if (on_map(adjacent_coord, width, height)) {
                supply_check_hex_t hex = hexes[to_hex_index(adjacent_coord, width)];
                bool friendly_ships = false;
                bool friendly_units = false;
                bool friendly_base = false;
                bool enemy_ships = false;
                bool enemy_units = false;
                bool enemy_base = false;
                find_blocking_contents(
                    hex,
                    team,
                    nations,
                    nation_team_membership,
                    friendly_ships,
                    friendly_units,
                    friendly_base,
                    enemy_ships,
                    enemy_units,
                    enemy_base
                );
                if (enemy_ships || enemy_base)
                    adjacent_enemy_ships = true;
                if (friendly_units)
                    adjacent_friendly_units = true;
            }
        }
        retval = adjacent_enemy_ships && !adjacent_friendly_units;
    }

    return retval;
}

class supply_visitor
{
private:
    int m_nation;
    int m_nations;
    int* m_nation_team_membership;
    int m_grid;
    int m_width;
    int m_height;
    supply_check_hex_t* m_hexes;
    int m_neutral_zone_id;
    int m_max_offmap_border_hexes;
    int* m_offmap_border_hexes;
    int* m_supply;
    boost::unordered_map<int, int>& m_vertex_id_to_hex_id;
    boost::unordered_map<int, int>& m_hex_id_to_vertex_id;
    bool& m_visited_offmap;

public:
    supply_visitor (int nation,
                    int nations,
                    int nation_team_membership[],
                    int grid,
                    int width,
                    int height,
                    supply_check_hex_t hexes[],
                    int neutral_zone_id,
                    int max_offmap_border_hexes,
                    int offmap_border_hexes[],
                    int supply[],
                    boost::unordered_map<int, int>& vertex_id_to_hex_id,
                    boost::unordered_map<int, int>& hex_id_to_vertex_id,
                    bool& visited_offmap) :
        m_nation (nation),
        m_nations (nations),
        m_nation_team_membership (nation_team_membership),
        m_grid (grid),
        m_width (width),
        m_height (height),
        m_hexes (hexes),
        m_neutral_zone_id (neutral_zone_id),
        m_max_offmap_border_hexes (max_offmap_border_hexes),
        m_offmap_border_hexes (offmap_border_hexes),
        m_supply (supply),
        m_vertex_id_to_hex_id (vertex_id_to_hex_id),
        m_hex_id_to_vertex_id (hex_id_to_vertex_id),
        m_visited_offmap (visited_offmap)
        {}

    bool examine (int v, graph::graph& g)
        {
            int hex_id_ = m_vertex_id_to_hex_id[v];

            if (0 <= hex_id_) { // off-map is always < 0
                int hex_index_ = hex_id_ / 100 + hex_id_ % 100 * m_width;
                supply_check_hex_t hex = m_hexes[hex_index_];
                int& supply = m_supply[hex_index_];
                if (hex.owner_id == m_nation) {
                    supply = m_grid;
                    if (2 < m_grid && !supply_source(hex, m_nation))
                        supply |= 1 << 24;
                }
                supply |= 1 << (m_nation + 8);
            }

            const int* const offmap_border_hexes_base =
                m_offmap_border_hexes + m_nation * m_max_offmap_border_hexes;
            const int* const offmap_border_hexes_end =
                offmap_border_hexes_base + m_max_offmap_border_hexes;

            if (hex_id_ < 0) { // offmap area
                for (const int* index = offmap_border_hexes_base;
                     index < offmap_border_hexes_end;
                     ++index) {
                    int hex_id = *index;
                    if (hex_id < 0)
                        continue;
                    if (m_hex_id_to_vertex_id.count(hex_id) ||
                        supply_blocked(hex_coord_t{hex_id / 100, hex_id % 100},
                                       m_nation,
                                       m_nations,
                                       m_nation_team_membership,
                                       m_hexes,
                                       m_width,
                                       m_height,
                                       m_neutral_zone_id))
                        continue;
                    int n = add_vertex(
                        g,
                        hex_id,
                        m_vertex_id_to_hex_id,
                        m_hex_id_to_vertex_id
                    );
                    boost::add_edge(v, n, g);
                }
            } else {
                hex_coord_t coord{hex_id_ / 100, hex_id_ % 100};
                for (hex_direction_t d : all_hex_directions) {
                    hex_coord_t adjacent_coord = adjacent_hex_coord(coord, d);
                    if (on_map(adjacent_coord, m_width, m_height)) {
                        int hex_id_ = hex_id(adjacent_coord);
                        if (m_hex_id_to_vertex_id.count(hex_id_) ||
                            supply_blocked(adjacent_coord,
                                           m_nation,
                                           m_nations,
                                           m_nation_team_membership,
                                           m_hexes,
                                           m_width,
                                           m_height,
                                           m_neutral_zone_id)) {
                            continue;
                        }
                        int n = add_vertex(
                            g,
                            hex_id_,
                            m_vertex_id_to_hex_id,
                            m_hex_id_to_vertex_id
                        );
                        boost::add_edge(v, n, g);
                    }
                }

                const int* offmap_hex =
                    std::find(offmap_border_hexes_base, offmap_border_hexes_end, hex_id_);
                if (offmap_hex != offmap_border_hexes_end) {
                    if (!m_hex_id_to_vertex_id.count(-m_nation - 1)) {
                        int n = add_vertex(
                            g,
                            -m_nation - 1,
                            m_vertex_id_to_hex_id,
                            m_hex_id_to_vertex_id
                        );
                        boost::add_edge(v, n, g);
                    }
                }
            }

            return true;
        }

    std::pair<float, bool> distance (float d, int u, int v)
        {
            const float epsilon = 1.0e-3;
            std::pair<float, bool> retval(d + 1.0f, true);
            int hex_id = m_vertex_id_to_hex_id[v];
            if (hex_id < 0 && true /* TODO: offmap has a base/planet */) {
                retval.first = 0;
            } else {
                hex_coord_t hc{hex_id / 100, hex_id % 100};
                if (supply_source(m_hexes[to_hex_index(hc, m_width)], m_nation))
                    retval.first = 0;
            }
            retval.second = retval.first < 6.0 + epsilon;
            return retval;
        }
};

int next_supply_source (int source,
                        int size,
                        int supply[],
                        supply_check_hex_t hexes[])
{
    for (; source < size; ++source) {
        if (supply_source(hexes[source], -1) && !supply[source])
            break;
    }
    return source;
}
#endif

#if 0
// Returns an int for each hex, containing a grid ID in the first 8 bits
// (0 is no grid, 1 is main capital grid, 2 is main offmap grid, anything
// else is a partial grid).  Bits 8-23 contain the nations supplying this
// hex (a '1' in bit N indicates that the nation with ID N-8 is supplying
// it).  Bit 24 contains a flag indicating supplies must be paid for by
// the hex's owner to supply ships in this hex (meaning the hex is in a
// partial supply grid, and does not include a free-supply feature like a
// SB, BATS, or planet).
int* determine_supply (
    int w, int h,
    supply_check_hex_t hexes[],
    int neutral_zone_id,
    int nations,
    int nation_team_membership[],
    int capitals[],
    int max_offmap_border_hexes,
    int offmap_border_hexes[])
{
    g_supply_data.supply.resize(w * h);

    graph::graph g;

    std::size_t n = w * h;
    for (int i = 0; i < nations; ++i) {
        if (offmap_border_hexes[i * max_offmap_border_hexes] != -1)
            ++n;
    }

    std::vector<graph::vertex_color> colors;
    std::vector<float> distances;
    std::vector<int> predecessors;

    graph::bfs_init(n, colors, distances, predecessors);

    boost::unordered_map<int, int> vertex_id_to_hex_id;
    boost::unordered_map<int, int> hex_id_to_vertex_id;

    auto find_grid = [&] (
        int nation,
        int grid,
        int hex_id,
        bool& visited_offmap
    ) {
        int n = add_vertex(g, hex_id, vertex_id_to_hex_id, hex_id_to_vertex_id);

        supply_visitor visitor(nation,
                               nations,
                               nation_team_membership,
                               grid,
                               w,
                               h,
                               hexes,
                               neutral_zone_id,
                               max_offmap_border_hexes,
                               offmap_border_hexes,
                               &g_supply_data.supply[0],
                               vertex_id_to_hex_id,
                               hex_id_to_vertex_id,
                               visited_offmap);
        graph::bfs_(g, colors, distances, predecessors, n, visitor);
    };

#if LOG
    {
        std::ofstream ofs("model_log.txt");
        boost::property_tree::ptree pt;

        pt.put("w", w);
        pt.put("h", h);

        for (int i = 0; i < w * h; ++i) {
            const std::string hex = "hexes." + boost::lexical_cast<std::string>(i);
            pt.put(hex + ".owner_id", hexes[i].owner_id);
            pt.put(hex + ".ship", hexes[i].ship);
            pt.put(hex + ".nonship_unit", hexes[i].nonship_unit);
            pt.put(hex + ".base_with_fighters", hexes[i].base_with_fighters);
            pt.put(hex + ".planet", hexes[i].planet);
            pt.put(hex + ".SB", hexes[i].SB);
            pt.put(hex + ".BATS", hexes[i].BATS);
            pt.put(hex + ".MB", hexes[i].MB);
            pt.put(hex + ".convoy", hexes[i].convoy);
            pt.put(hex + ".supply_tug", hexes[i].supply_tug);
        }

        pt.put("neutral_zone_id", neutral_zone_id);
        pt.put("nations", nations);

        for (int i = 0; i < nations; ++i) {
            pt.put("nation_team_membership." + boost::lexical_cast<std::string>(i),
                   nation_team_membership[i]);
        }

        for (int i = 0; i < nations; ++i) {
            pt.put("capitals." + boost::lexical_cast<std::string>(i),
                   capitals[i]);
        }

        pt.put("max_offmap_border_hexes", max_offmap_border_hexes);

        for (int i = 0; i < nations * max_offmap_border_hexes; ++i) {
            pt.put("offmap_border_hexes." + boost::lexical_cast<std::string>(i),
                   offmap_border_hexes[i]);
        }

        boost::property_tree::json_parser::write_json(ofs, pt);
    }
#endif

    for (int nation = 0; nation < nations; ++nation) {
        if (capitals[nation] == -1)
            continue;

#if LOG
        std::cerr << "\n"
                  << "****************************************\n"
                  << "* Nation " << nation << ":\n"
                  << "****************************************\n\n";
#endif

        bool visited_offmap = false; // TODO: Remove.
        find_grid(
            nation,
            1,
            capitals[nation],
            visited_offmap
        );

        if (!hex_id_to_vertex_id.count(-nation - 1)) {
            find_grid(
                nation,
                2,
                -nation - 1,
                visited_offmap
            );
        }
    }

    int source = next_supply_source(
        0,
        w * h,
        &g_supply_data.supply[0],
        hexes
    );

#if LOG
    std::cerr << "\n"
              << "next_supply_source()=" << source << "\n";
#endif

    std::vector<int> grid_ids(nations, 2);
    while (source < w * h) {
#if LOG
        std::cerr << "\n"
                  << "****************************************\n"
                  << "* Nation " << hexes[source].owner_id << " (possible supplemental grid):\n"
                  << "****************************************\n\n";
#endif

        // TODO: This seems to be producing some weird grids.  They're all
        // single-hex and do not appear to be in a valid range in the
        // y-dimension.
        bool dont_care = false;
        find_grid(
            hexes[source].owner_id,
            ++grid_ids[hexes[source].owner_id],
            source,
            dont_care
        );

        source = next_supply_source(
            source + 1,
            w * h,
            &g_supply_data.supply[0],
            hexes
        );
    }

    return &g_supply_data.supply[0];

}
#endif

#if 0
void test_determine_supply ()
{
    std::ifstream ifs("model_log.txt");
    boost::property_tree::ptree pt;
 
    boost::property_tree::json_parser::read_json(ifs, pt);
 
    const int w = pt.get<int>("w");
    const int h = pt.get<int>("h");
 
    std::vector<supply_check_hex_t> hexes(w * h);
    for (int i = 0; i < w * h; ++i) {
        const std::string hex = "hexes." + boost::lexical_cast<std::string>(i);
        hexes[i].owner_id = pt.get<int>(hex + ".owner_id");
        hexes[i].ship = pt.get<int>(hex + ".ship");
        hexes[i].nonship_unit = pt.get<int>(hex + ".nonship_unit");
        hexes[i].base_with_fighters = pt.get<int>(hex + ".base_with_fighters");
        hexes[i].planet = pt.get<int>(hex + ".planet");
        hexes[i].SB = pt.get<int>(hex + ".SB");
        hexes[i].BATS = pt.get<int>(hex + ".BATS");
        hexes[i].MB = pt.get<int>(hex + ".MB");
        hexes[i].convoy = pt.get<int>(hex + ".convoy");
        hexes[i].supply_tug = pt.get<int>(hex + ".supply_tug");
    }
 
    const int neutral_zone_id = pt.get<int>("neutral_zone_id");
    const int nations = pt.get<int>("nations");
 
    std::vector<int> nation_team_membership(nations);
    for (int i = 0; i < nations; ++i) {
        nation_team_membership[i] =
            pt.get<int>("nation_team_membership." + boost::lexical_cast<std::string>(i));
    }
 
    std::vector<int> capitals(nations);
    for (int i = 0; i < nations; ++i) {
        capitals[i] = pt.get<int>("capitals." + boost::lexical_cast<std::string>(i));
    }
 
    const int max_offmap_border_hexes = pt.get<int>("max_offmap_border_hexes");
 
    std::vector<int> offmap_border_hexes(nations * max_offmap_border_hexes);
    for (int i = 0; i < nations * max_offmap_border_hexes; ++i) {
        offmap_border_hexes[i] =
            pt.get<int>("offmap_border_hexes." + boost::lexical_cast<std::string>(i));
    }

    determine_supply(
        w,
        h,
        &hexes[0],
        neutral_zone_id,
        nations,
        &nation_team_membership[0],
        &capitals[0],
        max_offmap_border_hexes,
        &offmap_border_hexes[0]
    );
}
#endif
