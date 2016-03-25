#include <model.hpp>
#include "model_data.hpp"

#include <boost/container/flat_map.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <map>
#include <queue>
#include <string>

#define LOG 1
#if LOG
#include <fstream>
#endif


#define CATCH_AND_RETURN(value)                         \
    catch (std::exception const & e) {                  \
        /* TODO: Send up to Unity app? */               \
        std::cerr << boost::diagnostic_information(e);  \
        return value;                                   \
    }

namespace pb = google::protobuf;

namespace {

    template <class Key, class Value>
    struct constant_property
    { Value m_value; };

}

namespace boost {

    template <class Key, class Value>
    struct property_traits<constant_property<Key, Value> >
    {
        typedef Value value_type;
        typedef Key key_type;
        typedef readable_property_map_tag category;
    };

    template <class Key, class Value>
    const Value& get(const constant_property<Key, Value>& pmap, const Key&)
    { return pmap.m_value; }

}


namespace graph {

    struct vertex_hex_id_t {typedef boost::vertex_property_tag kind;};

    typedef boost::property<
        vertex_hex_id_t,
        int,
        boost::property<boost::vertex_index_t, int>
    > vertex_property_t;

    typedef boost::property<
        boost::edge_weight_t,
        double
    > edge_property_t;

    typedef boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::undirectedS,
        vertex_property_t,
        edge_property_t
    > graph;

    typedef boost::property_map<graph, vertex_hex_id_t>::const_type const_hex_id_property_map;
    typedef boost::property_map<graph, vertex_hex_id_t>::type hex_id_property_map;

    typedef boost::property_map<graph, boost::edge_weight_t>::const_type ConstEdgeWeightPropertyMap; // TODO
    typedef boost::property_map<graph, boost::edge_weight_t>::type EdgeWeightPropertyMap;

    typedef boost::graph_traits<graph>::edge_descriptor edge_descriptor;

    struct found_destination {}; 
    struct reached_depth_limit {};

    template <class Graph, class Visitor>
    bool bfs (Graph g, Visitor v, int start_hex_id)
    {
        try {
            boost::queue<int> buf;
            std::vector<int> colors(boost::num_vertices(g));
            boost::breadth_first_search(g, start_hex_id, buf, v, &colors[0]);
        } catch (const reached_depth_limit&) {
            // The algorithm didn't find hex2_id before reaching max_dist.
            return false;
        } catch (const found_destination&) {
            // hex2_id was found and we exited early by throwing.
        }
        return true;
    }

    enum vertex_color {
        white,
        gray,
        black
    };

    // bfs_init (G, s)
    //   for each vertex u in V[G]      initialize vertex u 
    //     color[u] := WHITE 
    //     d[u] := infinity 
    //     p[u] := u 
    //   end for

    template <typename D>
    void bfs_init (std::size_t n,
                   std::vector<vertex_color>& color,
                   std::vector<D>& d,
                   std::vector<int>& p)
    {
        color = std::vector<vertex_color>(n, white);
        d = std::vector<D>(n, (std::numeric_limits<D>::max)());
        p.resize(n);
        std::generate(p.begin(), p.end(), [] () {
            static int i = -1;
            return ++i;
        });
    }

    // bfs_ (G, s)
    //   color[s] := GRAY 
    //   d[s] := 0 
    //   ENQUEUE(Q, s)                  discover vertex s
    //   while (Q != Ø) 
    //     u := DEQUEUE(Q)              examine vertex u
    //     for each vertex v in Adj[u]  examine edge (u,v)
    //       if (color[v] = WHITE)      (u,v) is a tree edge
    //         color[v] := GRAY 
    //         d[v] := d[u] + 1  
    //         p[v] := u  
    //         ENQUEUE(Q, v)            discover vertex v
    //       else                       (u,v) is a non-tree edge
    //         if (color[v] = GRAY) 
    //           ...                    (u,v) has a gray target
    //         else 
    //           ...                    (u,v) has a black target
    //     end for
    //     color[u] := BLACK            finish vertex u
    //   end while
    //   return (d, p)

    // precondition: bfs_init(color, d, p)
    template <typename Graph, typename D, typename Visitor>
    void bfs_ (Graph& g,
               std::vector<vertex_color>& color,
               std::vector<D>& d,
               std::vector<int>& p,
               int start_vertex,
               Visitor visitor
    ) {
        color[start_vertex] = gray;
        d[start_vertex] = 0;
        std::queue<int> queue;
        queue.push(start_vertex);
        while (!queue.empty()) {
            int u = queue.front();
            queue.pop();
           if (!visitor.examine(u, g))
               continue;
            typedef typename Graph::adjacency_iterator iterator;
            std::pair<iterator, iterator> adj = adjacent_vertices(u, g);
            for (iterator it = adj.first; it != adj.second; ++it) {
                int v = *it;
                if (color[v] == white) {
                    color[v] = gray;
                    std::pair<D, bool> dist = visitor.distance(d[u], u, v);
                    if (!dist.second)
                        continue;
                    d[v] = dist.first;
                    p[v] = u;
                    queue.push(v);
                } else if (color[v] == gray) {
                    // TODO: Revisit, a la Djikstra/A*?
                }
            }
            color[u] = black;
        }
    }

}

hex_coord_t invalid_hex_coord{1000, 1000};

bool operator== (hex_coord_t lhs, hex_coord_t rhs)
{ return lhs.x == rhs.x && lhs.y == rhs.y; }

bool operator!= (hex_coord_t lhs, hex_coord_t rhs)
{ return !(lhs == rhs); }

#if LOG
std::ostream& operator<< (std::ostream& os, hex_coord_t hc)
{
    return os << (hc.x + 1 < 10 ? "0" : "") << (hc.x + 1) << (hc.y + 1 < 10 ? "0" : "") << (hc.y + 1);
}

//std::ostream& operator<< (std::ostream& os, hex_coord_t hc)
//{ return os << '(' << (hc.x + 1) << ',' << (hc.y + 1) << ')'; }
#endif

enum class hex_direction_t {
    above_right,
    above,
    above_left,
    below_left,
    below,
    below_right,
    hex_directions
};

hex_direction_t& operator++(hex_direction_t& d)
{
    assert(d != hex_direction_t::hex_directions);
    const int n = static_cast<int>(hex_direction_t::hex_directions);
    int d_int = static_cast<int>(d);
    d_int = (d_int + 1) % n;
    d = hex_direction_t(d_int);
    return d;
}

static const hex_direction_t all_hex_directions[] = {
    hex_direction_t::above_right,
    hex_direction_t::above,
    hex_direction_t::above_left,
    hex_direction_t::below_left,
    hex_direction_t::below,
    hex_direction_t::below_right
};


hex_coord_t hex_above (hex_coord_t hc)
{ return hex_coord_t{hc.x, hc.y - 1}; }

hex_coord_t hex_below (hex_coord_t hc)
{ return hex_coord_t{hc.x, hc.y + 1}; }

hex_coord_t hex_above_left (hex_coord_t hc)
{ return hex_coord_t{hc.x - 1, hc.y + (hc.x % 2 ? 0 : -1)}; }

hex_coord_t hex_below_left (hex_coord_t hc)
{ return hex_coord_t{hc.x - 1, hc.y + (hc.x % 2 ? 1 : 0)}; }

hex_coord_t hex_above_right (hex_coord_t hc)
{ return hex_coord_t{hc.x + 1, hc.y + (hc.x % 2 ? 0 : -1)}; }

hex_coord_t hex_below_right (hex_coord_t hc)
{ return hex_coord_t{hc.x + 1, hc.y + (hc.x % 2 ? 1 : 0)}; }


hex_coord_t adjacent_hex_coord (hex_coord_t hc, hex_direction_t hd)
{
    switch (hd) {
#define CASE(x) case hex_direction_t::x: return hex_##x(hc)
    CASE(above_right);
    CASE(above);
    CASE(above_left);
    CASE(below_left);
    CASE(below);
    CASE(below_right);
#undef CASE
    default:
    case hex_direction_t::hex_directions:
        return invalid_hex_coord;
    };
}


bool on_map (hex_coord_t hc, map_t m)
{ return hc.x < m.width && hc.y < m.height; }

bool on_map (hex_coord_t hc, int width, int height)
{ return hc.x < width && hc.y < height; }


int hex_index (hex_coord_t hc, int width)
{ return hc.x + hc.y * width; }


int hex_id (hex_coord_t hc)
{ return hc.x * 100 + hc.y; }


// Static container for hex ids within R=2 of a central hex.
struct neighbors_t
{
    typedef boost::array<hex_coord_t, 19> data_type;
    typedef data_type::const_iterator iterator;

    neighbors_t () : size (0) {}

    std::size_t size;
    data_type hexes;
};

neighbors_t::iterator begin (neighbors_t n)
{ return n.hexes.begin(); }

neighbors_t::iterator end (neighbors_t n)
{ return n.hexes.begin() + n.size; }


neighbors_t adjacent_hex_coords (hex_coord_t hc, map_t m, int r = 1)
{
    assert(r == 1 || r == 2);
    neighbors_t retval;
    if (on_map(hc, m)) {
        retval.hexes[retval.size++] = hc;
        for (hex_direction_t d : all_hex_directions) {
            hex_coord_t r1 = adjacent_hex_coord(hc, d);
            if (on_map(r1, m)) {
                retval.hexes[retval.size++] = r1;

                {
                    hex_coord_t r2 = adjacent_hex_coord(r1, d);
                    if (on_map(r2, m))
                        retval.hexes[retval.size++] = r2;
                }

                {
                    auto d2 = d;
                    ++d2;
                    hex_coord_t r2 = adjacent_hex_coord(r1, d2);
                    if (on_map(r2, m))
                        retval.hexes[retval.size++] = r2;
                }
            }
        }
    }
    return retval;
}

template <typename EdgeFn, typename WeightFn>
void init_graph (graph::graph& g,
                 graph::hex_id_property_map& hex_id_property_map,
                 graph::EdgeWeightPropertyMap& edge_weight_map,
                 int width,
                 int height,
                 EdgeFn make_edge,
                 WeightFn weight)
{
    hex_id_property_map = boost::get(graph::vertex_hex_id_t(), g);
    edge_weight_map = boost::get(boost::edge_weight, g);

    {
        int i = 0;
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y, ++i) {
                int id = hex_id(hex_coord_t{x, y});
                boost::add_vertex(g);
                hex_id_property_map[i] = id;
            }
        }
    }

    {
        int i = 0;
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y, ++i) {
                hex_coord_t coord{x, y};
                for (hex_direction_t d = hex_direction_t::above; d < hex_direction_t::below; ++d) {
                    hex_coord_t adjacent_coord = adjacent_hex_coord(coord, d);
                    if (on_map(adjacent_coord, width, height)) {
                        int index = hex_index(adjacent_coord, width);
                        if (!make_edge(i, index))
                            continue;
                        std::pair<graph::edge_descriptor, bool> add_edge_result =
                            boost::add_edge(i, index, g);
                        edge_weight_map[add_edge_result.first] = weight(i, index);
                    }
                }
            }
        }
    }
}

hex_coord_t hex_string_to_hex_coord (std::string str)
{
    if (str[0] == '0')
        str.substr(1, -1);
    if (str[0] == '0')
        str.substr(1, -1);
    if (str[0] == '0')
        str.substr(1, -1);
    const int hex_id = boost::lexical_cast<int>(str);
    int hex_x = hex_id / 100 - 1;
    int hex_y = hex_id % 100 - 1;
    return hex_coord_t{hex_x, hex_y};
}

feature_t feature_string_to_feature (std::string str)
{
    feature_t retval = feature_t::none;
    if (str == "BATS")
        retval = feature_t::bats;
    else if (str == "SB")
        retval = feature_t::sb;
    else if (str == "MIN")
        retval = feature_t::min;
    else if (str == "MAJ")
        retval = feature_t::maj;
    else if (str != "capital" && str != "none")
        boost::throw_exception(std::runtime_error("Invalid hex feature \"" + str + "\" found in map.json"));
    return retval;
}


#if 0 // Resurect as a validation function.
map_t FromProtobuf (const message::map_t& map_msg, const nations_t& nations)
{
    map_t retval;

#if 0
    retval.width = pt.get<int>("width");
    retval.height = pt.get<int>("height");

    retval.hexes.resize(retval.width * retval.height);

    const boost::property_tree::ptree& zones = pt.get_child("zones");
    for (const auto& zone : zones) {
        if (zone.first == "NZ") {
            boost::unordered_map<int, feature_t> planets;
            const boost::property_tree::ptree& planets_ = zone.second.get_child("planets");
            for (const auto& planet : planets_) {
                planets[hex_id(hex_string_to_hex_coord(planet.first))] =
                    feature_string_to_feature(planet.second.data());
            }

            const boost::property_tree::ptree& hexes = zone.second.get_child("hexes");
            for (const auto& hex : hexes) {
                hex_coord_t hc = hex_string_to_hex_coord(hex.second.data());

                assert(hc.x + hc.y * retval.width < retval.hexes.size());
                hex_t& map_hex = retval.hexes[hc.x + hc.y * retval.width];

                if (map_hex.coord != invalid_hex_coord)
                    boost::throw_exception(std::runtime_error("Duplicate definition of hex " + hex.second.data()));

                map_hex.coord = hc;
                map_hex.owner = 0;
                map_hex.feature = planets[hex_id(hc)];
            }
        } else {
            auto nations_it = nations.find(zone.first);
            if (nations_it == nations.end()) {
                boost::throw_exception(
                    std::runtime_error("Unknown owner nation '" + zone.first + "' encountered in map data")
                );
            }
            const int nation_id = nations_it->second.nation_id;

            const boost::property_tree::ptree& provinces = zone.second.get_child("provinces");
            for (const auto& province : provinces) {
                province_t map_province;
                for (const auto& hex : province.second) {
                    hex_coord_t hc = hex_string_to_hex_coord(hex.first);
                    map_province.hexes.push_back(hex_id(hc));

                    assert(hc.x + hc.y * retval.width < retval.hexes.size());
                    hex_t& map_hex = retval.hexes[hc.x + hc.y * retval.width];

                    if (map_hex.coord != invalid_hex_coord)
                        boost::throw_exception(std::runtime_error("Duplicate definition of hex " + hex.first));

                    map_hex.coord = hc;
                    map_hex.owner = nation_id;
                    map_hex.feature = feature_string_to_feature(hex.second.data());
                }
                retval.provinces[nation_id].push_back(map_province);
#if 0
                boost::property_tree::json_parser::write_json(std::cout, province.second);
#endif
            }
        }
    }

    for (std::size_t i = 0; i < retval.hexes.size(); ++i) {
        if (retval.hexes[i].coord == invalid_hex_coord) {
            int hex_x = i % retval.width + 1;
            int hex_y = i / retval.width + 1;
            std::string hex_str = boost::lexical_cast<std::string>(hex_x * 100 + hex_y);
            if (hex_str.size() == 3u)
                hex_str = '0' + hex_str;
            boost::throw_exception(std::runtime_error("Hex " + hex_str + " not defined in map.json"));
        }
    }

    // TODO: Fill in hex_t::*neutral_zone_border* values.
#endif

    return retval;
}
#endif

int hex_coord_to_graph_id (hex_coord_t hc)
{ return (hc.x + 1) * 100 + hc.y + 1; }

struct supply_data
{
    std::vector<int> supply;
} g_supply_data;

bool neutral (supply_check_hex_t h, int nz_id)
{ return h.owner_id == nz_id; }

bool supply_point (supply_check_hex_t h, int nation)
{
    const int flag = 1 << nation;
    return
        h.planet & flag ||
        h.SB & flag ||
        h.BATS & flag ||
        h.MB & flag ||
        h.convoy & flag ||
        h.supply_tug & flag;
}

bool supply_source (supply_check_hex_t h, int nation)
{
    const int flag = 1 << nation;
    return nation == -1 ?
        h.planet || h.SB || h.BATS :
        h.planet & flag || h.SB & flag || h.BATS & flag;
}

int add_vertex (graph::graph& g,
                int v,
                boost::unordered_map<int, int>& vertex_id_to_hex_id,
                boost::unordered_map<int, int>& hex_id_to_vertex_id)
{
    int n = boost::num_vertices(g);
    vertex_id_to_hex_id[n] = v;
    hex_id_to_vertex_id[v] = n;
#if LOG
    std::cerr << "add_vertex(";
    if (v < 0)
        std::cerr << v;
    else
        std::cerr << hex_coord_t{v / 100, v % 100};
    std::cerr << ") as vertex " << n << '\n';
#endif
    boost::add_vertex(g);
    return n;
}

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
    int hex_index_ = hex_index(hc, width);
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
                supply_check_hex_t hex = hexes[hex_index(adjacent_coord, width)];
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
                if (supply_source(m_hexes[hex_index(hc, m_width)], m_nation))
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

struct model_state_t
{
    model_state_t () : initialized (false) {}

    bool initialized;
    nations_t nations;
    map_t m;
    orders_of_battle_t oob;
    graph::graph g;
    graph::hex_id_property_map hex_id_property_map;
    graph::EdgeWeightPropertyMap edge_weight_map;
};
model_state_t g_model_state;

extern "C" {

    MODEL_API
    int test (int i) try
    {
        return i + 42;
    } CATCH_AND_RETURN(0);

    MODEL_API
    float test_2 (int n, float floats[]) try
    {
        return 6;

    } CATCH_AND_RETURN(0.0f);

    MODEL_API
    float test_3 (int n, ga_hex_t* hexes) try
    {
        float retval = 0;
        for (int i = 0; i < n; ++i) {
            retval += hexes[i].a + hexes[i].b;
        }
        return retval;

    } CATCH_AND_RETURN(0.0f);

    MODEL_API
    int init_model (const char* nations_str, const char* map_str, const char* oob_str) try
    {
        if (g_model_state.initialized)
            boost::throw_exception(std::runtime_error("Attempted to duplicate-initialize model"));

        {
            message::nations_t nations_msg;
            pb::io::ArrayInputStream is(nations_str, strlen(nations_str));
            if (!pb::TextFormat::Parse(&is, &nations_msg))
                boost::throw_exception(std::runtime_error("Missing starting nations data"));
            g_model_state.nations = FromProtobuf(nations_msg);
            // TODO: Validate and fill in nation_ids.
        }

        if (0) { // TODO
            message::map_t map_msg;
            pb::io::ArrayInputStream is(map_str, strlen(map_str));
            if (!pb::TextFormat::Parse(&is, &map_msg))
                boost::throw_exception(std::runtime_error("Missing starting map data"));
            g_model_state.m = FromProtobuf(map_msg);
            // TODO: Validate and fill in nation_ids.
        }

        if (0) { // TODO
            message::orders_of_battle_t oob_msg;
            pb::io::ArrayInputStream is(oob_str, strlen(oob_str));
            if (!pb::TextFormat::Parse(&is, &oob_msg))
                boost::throw_exception(std::runtime_error("Missing starting order of battle data"));
            g_model_state.oob = FromProtobuf(oob_msg);
            // TODO: Validate and fill in nation_ids.
        }

#if 0
        init_graph(
            g_model_state.g,
            g_model_state.hex_id_property_map,
            g_model_state.edge_weight_map,
            g_model_state.m.width,
            g_model_state.m.height,
            [] (int id1, int id2) {return true;},
            [] (int id1, int id2) {return 1.0;}
        );
#endif

        g_model_state.initialized = true;

        return 1;
    } CATCH_AND_RETURN(0);

    MODEL_API
    int reset_model () try
    {
        g_model_state = model_state_t();
        return 1;

    } CATCH_AND_RETURN(0);

    MODEL_API
    int save_model (const char* filename) try
    {
        if (!g_model_state.initialized)
            boost::throw_exception(std::runtime_error("Attempted to save an uninitialized model"));

        std::ofstream ofs(filename);
        if (!ofs)
            return 0;

        message::model_t model;

        *model.mutable_nations() = ToProtobuf(g_model_state.nations);
        *model.mutable_map() = ToProtobuf(g_model_state.m);

        pb::io::OstreamOutputStream os(&ofs);
        return static_cast<int>(pb::TextFormat::Print(model, &os));

    } CATCH_AND_RETURN(0);

    MODEL_API
    int load_model (const char* filename) try
    {
        if (g_model_state.initialized)
            boost::throw_exception(std::runtime_error("Attempted to load over an initialized model"));

        std::ifstream ifs(filename);
        if (!ifs)
            return 0;

        message::model_t model;
        pb::io::IstreamInputStream is(&ifs);
        if (!pb::TextFormat::Parse(&is, &model))
            return 0;

        {
            if (!model.has_nations())
                boost::throw_exception(std::runtime_error("Missing saved nations data"));
            g_model_state.nations = FromProtobuf(model.nations());
        }

        {
            if (!model.has_map())
                boost::throw_exception(std::runtime_error("Missing saved map data"));
            g_model_state.m = FromProtobuf(model.map());
        }

        init_graph(
            g_model_state.g,
            g_model_state.hex_id_property_map,
            g_model_state.edge_weight_map,
            g_model_state.m.width,
            g_model_state.m.height,
            [] (int id1, int id2) {return true;},
            [] (int id1, int id2) {return 1.0;}
        );

        g_model_state.initialized = true;

        return 1;

    } CATCH_AND_RETURN(0);

    // Returns an int for each hex, containing a grid ID in the first 8 bits
    // (0 is no grid, 1 is main capital grid, 2 is main offmap grid, anything
    // else is a partial grid).  Bits 8-23 contain the nations supplying this
    // hex (a '1' in bit N indicates that the nation with ID N-8 is supplying
    // it).  Bit 24 contains a flag indicating supplies must be paid for by
    // the hex's owner to supply ships in this hex (meaning the hex is in a
    // partial supply grid, and does not include a free-supply feature like a
    // SB, BATS, or planet).
    MODEL_API
    int* determine_supply (int w, int h,
                           supply_check_hex_t hexes[],
                           int neutral_zone_id,
                           int nations,
                           int nation_team_membership[],
                           int capitals[],
                           int max_offmap_border_hexes,
                           int offmap_border_hexes[]) try
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

    } CATCH_AND_RETURN(nullptr);

}

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
