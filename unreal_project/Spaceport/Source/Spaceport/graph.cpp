#ifndef BUILD_FOR_TEST
#include "Spaceport.h"
#endif

#include "graph.hpp"
#include "data_utility.hpp"


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
            // TODO: I don't think A is actually the supply mission!
            if (is_convoy(unit) || unit.tug_mission == tug_mission_t::A)
                return true;
        }

        return false;
    }

    struct blocking_contents_t
    {
        int friendly_ships;
        int friendly_units;
        int friendly_bases;
        int enemy_ships;
        int enemy_units;
        int enemy_bases;
    };

    blocking_contents_t blocking_contents (hex_t const & hex, team_t const & team)
    {
        blocking_contents_t retval = {0};

        for (auto const & zone : hex.zones) {
            for (auto const & fixture : zone.fixtures) {
                if (fixture.type == hex_zone_fixture_t::type_t::type_base) {
                    // TODO: Must this be a certain number of fighters or PFs to count?
                    if (fixture.base.fighters != 0 || fixture.base.pfs != 0) {
                        // TODO: if allied, increment friendly
                        ++retval.friendly_bases;
                        // TODO: if enemy, increment enemy
                    }
                }
            }
        }

        for (auto const nation_id : team.nations) {
            auto it = hex.fleets.fleets.find(nation_id);
            if (it == hex.fleets.fleets.end())
                continue;

            auto const & fleet = it->second;
            for (auto const & unit : fleet.units) {
                // TODO
            }
        }

#if 0
        for (int n = 0; n < nations; ++n) {
            const int flag = 1 << n;
            if (nation_team_membership[n] == team) {
                friendly_ships |= bool(hex.ship & flag);
                friendly_units |= friendly_ships || hex.nonship_unit & flag;
                friendly_base |= bool(hex.base_with_fighters & flag);
            } else {
                enemy_ships |= bool(hex.ship & flag);
                enemy_units |= friendly_ships || hex.nonship_unit & flag;
                enemy_base |= bool(hex.base_with_fighters & flag);
            }
        }
#endif

        return retval;
    }

}

#if 0
void find_blocking_contents (
    const supply_check_hex_t & hex,
    int team,
    int nations,
    const int* nation_team_membership,
    bool & friendly_ships,
    bool & friendly_units,
    bool & friendly_base,
    bool & enemy_ships,
    bool & enemy_units,
    bool & enemy_base
) {
    for (int n = 0; n < nations; ++n) {
        const int flag = 1 << n;
        if (nation_team_membership[n] == team) {
            friendly_ships |= bool(hex.ship & flag);
            friendly_units |= friendly_ships || hex.nonship_unit & flag;
            friendly_base |= bool(hex.base_with_fighters & flag);
        } else {
            enemy_ships |= bool(hex.ship & flag);
            enemy_units |= friendly_ships || hex.nonship_unit & flag;
            enemy_base |= bool(hex.base_with_fighters & flag);
        }
    }
}

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
    } else if (friendly_units) {
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
