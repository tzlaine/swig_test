#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <map>
#include <string>

#include <numeric> // TODO
#include <iostream>


#ifdef BOOST_NO_EXCEPTIONS
#include <boost/exception/diagnostic_information.hpp>
namespace boost {
    void throw_exception (std::exception const & e)
    {
        // TODO: Send up to Unity app?
        std::cerr << boost::diagnostic_information(e);
    }
}
#endif

#if defined(BUILD_LIBRARY) && defined(_MSC_VER)
#define GRAPH_ALGO_API __declspec(dllexport)
#else
#define GRAPH_ALGO_API
#endif

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

    const int invalid_hex_id = -1000;

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

    template <class Graph, class Edge, class Vertex>
    class bfs_visitor
    {
    private:
        Vertex m_marker;
        Vertex m_stop;
        Vertex m_source;
        Vertex* m_predecessors;
        int m_levels_remaining;
        bool m_level_complete;

    public:
        bfs_visitor (const Vertex& start,
                     const Vertex& stop,
                     Vertex* predecessors,
                     int max_depth) :
            m_marker (start),
            m_stop (stop),
            m_source (start),
            m_predecessors (predecessors),
            m_levels_remaining (max_depth),
            m_level_complete (false)
            {}

        void initialize_vertex (const Vertex& v, const Graph& g) {}

        void discover_vertex (const Vertex& v, const Graph& g)
            {
                if (m_predecessors)
                    m_predecessors[static_cast<int>(v)] = m_source;

                if (v == m_stop)
                    throw found_destination();

                if (m_level_complete) {
                    m_marker = v;
                    m_level_complete = false;
                }
            }

        void examine_vertex (const Vertex& v, const Graph& g)
            {
                if (v == m_marker) {
                    if (!m_levels_remaining)
                        throw reached_depth_limit();
                    m_levels_remaining--;
                    m_level_complete = true;
                }

                m_source = v; // avoid re-calculating source from edge
            }

        void examine_edge (const Edge& e, const Graph& g) {}
        void tree_edge (const Edge& e, const Graph& g) {}
        void non_tree_edge (const Edge& e, const Graph& g) {}
        void gray_target (const Edge& e, const Graph& g) {}
        void black_target (const Edge& e, const Graph& g) {}
        void finish_vertex (const Vertex& e, const Graph& g) {}
    };

    template <class Graph, class Edge, class Vertex>
    class supply_visitor
    {
    private:
        int m_nation;
        int m_grid;
        int* m_supply;
        hex_id_property_map m_hex_id_property_map;
        bfs_visitor<Graph, Edge, Vertex> m_bfs_visitor;

    public:
        supply_visitor (int nation,
                        int grid,
                        int supply[],
                        hex_id_property_map hex_id_property_map_,
                        bfs_visitor<Graph, Edge, Vertex> bfs_visitor_) :
            m_nation (nation),
            m_grid (grid),
            m_supply (supply),
            m_hex_id_property_map (hex_id_property_map_),
            m_bfs_visitor (bfs_visitor_)
            {}

        void initialize_vertex (const Vertex& v, const Graph& g) {}

        void discover_vertex (const Vertex& v, const Graph& g)
            {
                m_bfs_visitor.discover_vertex(v, g);
            }

        void examine_vertex (const Vertex& v, const Graph& g)
            {
                m_bfs_visitor.examine_vertex(v, g);
            }

        void examine_edge (const Edge& e, const Graph& g) {}
        void tree_edge (const Edge& e, const Graph& g) {}
        void non_tree_edge (const Edge& e, const Graph& g) {}
        void gray_target (const Edge& e, const Graph& g) {}
        void black_target (const Edge& e, const Graph& g) {}
        void finish_vertex (const Vertex& e, const Graph& g) {}
    };

}


struct hex_coord
{
    hex_coord () : x (1000), y (1000) {}
    hex_coord (unsigned int x, unsigned int y) : x (x), y (y) {}

    unsigned int x;
    unsigned int y;

    static hex_coord invalid;
};

hex_coord hex_coord::invalid;

bool operator== (hex_coord lhs, hex_coord rhs)
{ return lhs.x == rhs.x && lhs.y == rhs.y; }

bool operator!= (hex_coord lhs, hex_coord rhs)
{ return !(lhs == rhs); }


struct map
{
    enum feature_ {
        none,
        bats,
        sb,
        min,
        maj,
        capital
    };

    struct hex
    {
        hex () :
            coord (),
            owner (0),
            feature (none),
            num_neutral_zone_borders (0),
            neutral_zone_bordering ()
            {}

        hex_coord coord;
        unsigned int owner;
        feature_ feature;
        unsigned int num_neutral_zone_borders;
        unsigned int neutral_zone_bordering[3];
    };

    struct province
    {
        std::vector<unsigned int> hexes;
    };

    map () :
        width (0),
        height (0)
        {}

    unsigned int width;
    unsigned int height;

    // TODO: Move this out into its own data file.  Contents of capital hexes
    // should be placed there as well.
    std::vector<std::pair<unsigned int, std::string>> nations;
    boost::unordered_map<unsigned int, std::vector<province>> provinces; // key is owner ID
    std::vector<hex> hexes;
};


enum hex_direction {
    above_right,
    above,
    above_left,
    below_left,
    below,
    below_right,
    hex_directions
};


hex_coord hex_above (hex_coord hc)
{ return hex_coord(hc.x, hc.y - 1); }

hex_coord hex_below (hex_coord hc)
{ return hex_coord(hc.x, hc.y + 1); }

hex_coord hex_above_left (hex_coord hc)
{ return hex_coord(hc.x - 1, hc.y + (hc.x % 2 ? 0 : -1)); }

hex_coord hex_below_left (hex_coord hc)
{ return hex_coord(hc.x - 1, hc.y + (hc.x % 2 ? 1 : 0)); }

hex_coord hex_above_right (hex_coord hc)
{ return hex_coord(hc.x + 1, hc.y + (hc.x % 2 ? 0 : -1)); }

hex_coord hex_below_right (hex_coord hc)
{ return hex_coord(hc.x + 1, hc.y + (hc.x % 2 ? 1 : 0)); }


hex_coord adjacent_hex_coord (hex_coord hc, hex_direction hd)
{
    typedef hex_coord (*fn) (hex_coord hc);
    fn fns[6] = {
        hex_above_right,
        hex_above,
        hex_above_left,
        hex_below_left,
        hex_below,
        hex_below_right
    };
    return fns[hd](hc);
}


bool on_map (hex_coord hc, map m)
{ return hc.x < m.width && hc.y < m.height; }

bool on_map (hex_coord hc, unsigned int width, unsigned int height)
{ return hc.x < width && hc.y < height; }


unsigned int hex_index (hex_coord hc, unsigned int width)
{ return hc.x + hc.y * width; }


unsigned int hex_id (hex_coord hc)
{ return hc.x * 100 + hc.y; }


// Static container for hex ids within R=2 of a central hex.
struct neighbors
{
    typedef boost::array<hex_coord, 19> data_type;
    typedef data_type::const_iterator iterator;

    neighbors () : size (0) {}

    std::size_t size;
    data_type hexes;
};

neighbors::iterator begin (neighbors n)
{ return n.hexes.begin(); }

neighbors::iterator end (neighbors n)
{ return n.hexes.begin() + n.size; }


neighbors adjacent_hex_coords (hex_coord hc, map m, unsigned int r = 1)
{
    assert(r == 1 || r == 2);
    neighbors retval;
    if (on_map(hc, m)) {
        retval.hexes[retval.size++] = hc;        
        for (hex_direction d = above_right;
             d < hex_directions;
             d = hex_direction(d + 1)) {
            hex_coord r1 = adjacent_hex_coord(hc, d);
            if (on_map(r1, m)) {
                retval.hexes[retval.size++] = r1;

                {
                    hex_coord r2 = adjacent_hex_coord(r1, d);
                    if (on_map(r2, m))
                        retval.hexes[retval.size++] = r2;
                }

                {
                    hex_coord r2 = adjacent_hex_coord(r1, hex_direction(d + 1));
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
                 unsigned int width,
                 unsigned int height,
                 EdgeFn make_edge,
                 WeightFn weight)
{
    hex_id_property_map = boost::get(graph::vertex_hex_id_t(), g);
    edge_weight_map = boost::get(boost::edge_weight, g);

    {
        unsigned int i = 0;
        for (unsigned int x = 0; x < width; ++x) {
            for (unsigned int y = 0; y < height; ++y, ++i) {
                unsigned int id = hex_id(hex_coord(x, y));
                boost::add_vertex(g);
                hex_id_property_map[i] = id;
            }
        }
    }

    {
        unsigned int i = 0;
        for (unsigned int x = 0; x < width; ++x) {
            for (unsigned int y = 0; y < height; ++y, ++i) {
                hex_coord coord(x, y);
                for (hex_direction d = above; d < below; d = hex_direction(d + 1)) {
                    hex_coord adjacent_coord = adjacent_hex_coord(coord, d);
                    if (on_map(adjacent_coord, width, height)) {
                        unsigned int index = hex_index(adjacent_coord, width);
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

hex_coord hex_string_to_hex_coord (std::string str)
{
    if (str[0] == '0')
        str.substr(1, -1);
    if (str[0] == '0')
        str.substr(1, -1);
    if (str[0] == '0')
        str.substr(1, -1);
    const unsigned int hex_id = boost::lexical_cast<unsigned int>(str);
    unsigned int hex_x = hex_id / 100 - 1;
    unsigned int hex_y = hex_id % 100 - 1;
    return hex_coord(hex_x, hex_y);
}

map::feature_ feature_string_to_feature (std::string str)
{
    map::feature_ retval = map::none;
    if (str == "BATS")
        retval = map::bats;
    else if (str == "SB")
        retval = map::sb;
    else if (str == "MIN")
        retval = map::min;
    else if (str == "MAJ")
        retval = map::maj;
    else if (str != "capital" && str != "none")
        boost::throw_exception(std::runtime_error("Invalid hex feature \"" + str + "\" found in map.json"));
    return retval;
}

map read_map ()
{
    map retval;

    boost::property_tree::ptree pt;
    {
        std::ifstream ifs("default_map.json");
        boost::property_tree::json_parser::read_json(ifs, pt);
    }

    retval.width = pt.get<unsigned int>("width");
    retval.height = pt.get<unsigned int>("height");

    retval.hexes.resize(retval.width * retval.height);

    boost::property_tree::ptree zones = pt.get_child("zones");
    for (auto zone : zones) {
        if (zone.first == "NZ") {
            boost::unordered_map<unsigned int, map::feature_> planets;
            boost::property_tree::ptree planets_ = zone.second.get_child("planets");
            for (auto planet : planets_) {
                planets[hex_id(hex_string_to_hex_coord(planet.first))] =
                    feature_string_to_feature(planet.second.data());
            }

            boost::property_tree::ptree hexes = zone.second.get_child("hexes");
            for (auto hex : hexes) {
                hex_coord hc = hex_string_to_hex_coord(hex.second.data());

                assert(hc.x + hc.y * retval.width < retval.hexes.size());
                map::hex& map_hex = retval.hexes[hc.x + hc.y * retval.width];

                if (map_hex.coord != hex_coord())
                    boost::throw_exception(std::runtime_error("Duplicate definition of hex " + hex.second.data()));

                map_hex.coord = hc;
                map_hex.owner = 0;
                map_hex.feature = planets[hex_id(hc)];
            }
        } else {
            unsigned int nation_id = retval.nations.size() + 1; // 0 reserved for neutral zone
            retval.nations.push_back(std::make_pair(nation_id, zone.first));

            boost::property_tree::ptree provinces = zone.second.get_child("provinces");
            for (auto province : provinces) {
                map::province map_province;
                for (auto hex : province.second) {
                    hex_coord hc = hex_string_to_hex_coord(hex.first);
                    map_province.hexes.push_back(hex_id(hc));

                    assert(hc.x + hc.y * retval.width < retval.hexes.size());
                    map::hex& map_hex = retval.hexes[hc.x + hc.y * retval.width];

                    if (map_hex.coord != hex_coord())
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
        if (retval.hexes[i].coord == hex_coord()) {
            unsigned int hex_x = i % retval.width + 1;
            unsigned int hex_y = i / retval.width + 1;
            std::string hex_str = boost::lexical_cast<std::string>(hex_x * 100 + hex_y);
            if (hex_str.size() == 3u)
                hex_str = '0' + hex_str;
            boost::throw_exception(std::runtime_error("Hex " + hex_str + " not defined in map.json"));
        }
    }

    // TODO: Fill in map::hex::*neutral_zone_border* values.

    return retval;
}

void validate_map ()
{
    const map m = read_map();
    graph::graph g;
    graph::hex_id_property_map hex_id_property_map;
    graph::EdgeWeightPropertyMap edge_weight_map;
    init_graph(
        g,
        hex_id_property_map,
        edge_weight_map,
        m.width,
        m.height,
        [] (unsigned int id1, unsigned int id2) {return true;},
        [] (unsigned int id1, unsigned int id2) {return 1.0;}
    );
    std::cerr << "map looks good!\n";
}

int hex_coord_to_graph_id (hex_coord hc)
{ return (hc.x + 1) * 100 + hc.y + 1; }

struct ga_hex_t
{
    int a;
    float b;
};

struct supply_data
{
    std::vector<int> supply;
} g_supply_data;

struct supply_check_hex_t
{
    int owner_id;
    // The presence values for each team are encoded as:
    // 1 << 0: ship
    // 1 << 1: non-ship unit
    // 1 << 2: base w/fighters/PFs
    // 1 << 3: planet
    // 1 << 4: SB
    // 1 << 5: BATS
    // 1 << 6: MB
    // 1 << 7: convoy
    // 1 << 8: supply tug
    // Team N is in bits 1 << (N * 9 + 0) through 1 << (N * 9 + 8).
    int presence;
    int borders_offmap;
};

bool neutral (supply_check_hex_t h, unsigned int nz_id)
{ return h.owner_id == nz_id; }

int next_supply_source (int nation,
                        int point,
                        int size,
                        int supply[],
                        supply_check_hex_t hexes[])
{
    for (; point < size; ++point) {
        if (hexes[point].owner_id == nation) {
            int presence = hexes[point].presence;
            if (presence | ((1 << 3) | (1 << 4) | (1 << 5)) && supply[point])
                break;
        }
    }
    return point;
}

extern "C" {

    GRAPH_ALGO_API
    int test (int i)
    {
        return i + 42;
    }

    GRAPH_ALGO_API
    float test_2 (int n, float floats[])
    {
        return std::accumulate(floats, floats + n, 0.0f);
    }

    GRAPH_ALGO_API
    float test_3 (int n, ga_hex_t* hexes)
    {
        float retval = 0;
        for (int i = 0; i < n; ++i) {
            retval += hexes[i].a + hexes[i].b;
        }
        return retval;
    }

    // Returns an int for each hex, containing a grid ID in the first 8 bits
    // (0 is no grid, 1 is main capital grid, 2 is main offmap grid, anything
    // else is a partial grid).  Bits 8-23 contain the nations supplying this
    // hex (a '1' in bit N indicates that the nation with ID N-8 is supplying
    // it).  Bit 17 contains a flag indicating supplies must be paid for by
    // the hex's owner to supply ships in this hex (meaning the hex is in a
    // partial supply grid, and does not include a free-supply feature like a
    // SB, BATS, or planet).
    GRAPH_ALGO_API
    int* determine_supply (int w, int h,
                           supply_check_hex_t hexes[],
                           int neutral_zone_id,
                           int nations,
                           int nation_team_membership[],
                           int capitals[],
                           int nation_offmap_areas[])
    {
        g_supply_data.supply.resize(w * h);

        graph::graph g;
        graph::hex_id_property_map hex_id_property_map;
        graph::EdgeWeightPropertyMap edge_weight_map;

        init_graph(
            g,
            hex_id_property_map,
            edge_weight_map,
            w,
            h,
            [=] (unsigned int id1, unsigned int id2) {
                return
                    !neutral(hexes[id1], neutral_zone_id) &&
                    !neutral(hexes[id2], neutral_zone_id);
            },
            [=] (unsigned int id1, unsigned int id2) {return 1.0;}
        );

        std::vector<int> offmap_area_ids(nations, -1);
        for (int i = 0; i < nations; ++i) {
            if (nation_offmap_areas[i] != -1) {
                offmap_area_ids[nation_offmap_areas[i]] = boost::num_vertices(g);
                boost::add_vertex(g);
            }
        }

        for (int i = 0; i < w * h; ++i) {
            if (hexes[i].borders_offmap != -1 && !neutral(hexes[i], neutral_zone_id)) {
                std::pair<graph::edge_descriptor, bool> add_edge_result =
                    boost::add_edge(i, hexes[i].borders_offmap, g);
                edge_weight_map[add_edge_result.first] = 1.0;
            }
        }

        for (int i = 0; i < nations; ++i) {
            if (capitals[i] == -1)
                continue;

            typedef graph::bfs_visitor<
                graph::graph,
                boost::graph_traits<graph::graph>::edge_descriptor,
                int
            > bfs_visitor;
            typedef graph::supply_visitor<
                graph::graph,
                boost::graph_traits<graph::graph>::edge_descriptor,
                int
            > supply_visitor;

            bfs_visitor bfs_visitor_(0, graph::invalid_hex_id, 0, 6);

            {
                supply_visitor visitor(i,
                                       1,
                                       &g_supply_data.supply[0],
                                       hex_id_property_map,
                                       bfs_visitor_);
                graph::bfs(g, visitor, 0);
            }

            if (offmap_area_ids[nation_offmap_areas[i]] != -1) {
                supply_visitor visitor(i,
                                       2,
                                       &g_supply_data.supply[0],
                                       hex_id_property_map,
                                       bfs_visitor_);
                graph::bfs(g, visitor, offmap_area_ids[nation_offmap_areas[i]]);
            }

            int source = next_supply_source(
                i,
                0,
                w * h,
                &g_supply_data.supply[0],
                hexes
            );
            int grid = 3;
            while (source < w * h) {
                supply_visitor visitor(i,
                                       grid++,
                                       &g_supply_data.supply[0],
                                       hex_id_property_map,
                                       bfs_visitor_);
                graph::bfs(g, visitor, source);

                source = next_supply_source(
                    i,
                    0,
                    w * h,
                    &g_supply_data.supply[0],
                    hexes
                );
            }
        }

        return &g_supply_data.supply[0];
    }

}

#ifndef BUILD_LIBRARY
int main ()
{
    validate_map();
    return 0;
}
#endif
