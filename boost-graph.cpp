#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

#include <boost/array.hpp>

#if 1
#include <iostream>
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

    struct short_circuiting_visitor : public boost::base_visitor<short_circuiting_visitor>
    {
        typedef boost::on_finish_vertex event_filter;

        struct found_destination {};

        short_circuiting_visitor (int dest) : destination (dest) {}

        template <class Vertex, class Graph>
        void operator() (Vertex u, Graph& g)
        {
            if (static_cast<int>(u) == destination)
                throw found_destination();
        }

        const int destination;
    };

    template <class Graph, class Edge, class Vertex>
    class bfs_visitor
    {
    public:
        class found_destination {}; 
        class reached_depth_limit {};

    private:
        Vertex m_marker;
        Vertex m_stop;
        Vertex m_source;
        Vertex* m_predecessors;
        int m_levels_remaining;
        bool m_level_complete;

    public:
        bfs_visitor (const Vertex& start, const Vertex& stop, Vertex predecessors[], int max_depth) :
            m_marker (start),
            m_stop (stop),
            m_source (start),
            m_predecessors (predecessors),
            m_levels_remaining (max_depth),
            m_level_complete (false)
            {}

        void initialize_vertex (const Vertex& v, const Graph& g)
            {}

        void discover_vertex (const Vertex& v, const Graph& g)
            {
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

    struct vertex_hex_id_t {typedef boost::vertex_property_tag kind;};


#if 0
    /** Returns the path between vertices \a system1_id and \a system2_id of
      * \a graph that travels the shorest distance on starlanes, and the path
      * length.  If system1_id is the same vertex as system2_id, the path has
      * just that system in it, and the path lenth is 0.  If there is no path
      * between the two vertices, then the list is empty and the path length
      * is -1.0 */
    template <class Graph>
    std::pair<std::list<int>, double> ShortestPathImpl(const Graph& graph, int system1_id, int system2_id,
                                                       double linear_distance, const boost::unordered_map<int, int>& id_to_graph_index)
    {
        typedef typename boost::property_map<Graph, vertex_system_id_t>::const_type     ConstSystemIDPropertyMap;
        typedef typename boost::property_map<Graph, boost::vertex_index_t>::const_type  ConstIndexPropertyMap;
        typedef typename boost::property_map<Graph, boost::edge_weight_t>::const_type   ConstEdgeWeightPropertyMap;

        std::pair<std::list<int>, double> retval(std::list<int>(), -1.0);

        ConstSystemIDPropertyMap sys_id_property_map = boost::get(vertex_system_id_t(), graph);

        // convert system IDs to graph indices.  try/catch for invalid input system ids.
        int system1_index, system2_index;
        try {
            system1_index = id_to_graph_index.at(system1_id);
            system2_index = id_to_graph_index.at(system2_id);
        } catch (...) {
            return retval;
        }

        // early exit if systems are the same
        if (system1_id == system2_id) {
            retval.first.push_back(system2_id);
            retval.second = 0.0;    // no jumps needed -> 0 distance
            return retval;
        }

        /* initializing all vertices' predecessors to themselves prevents endless loops when back traversing the tree in the case where
           one of the end systems is system 0, because systems that are not connected to the root system (system2) are not visited
           by the search, and so their predecessors are left unchanged.  Default initialization of the vector may be 0 or undefined
           which could lead to out of bounds errors, or endless loops if a system's default predecessor is 0 (debug mode), and 0's
           predecessor is that system */
        std::vector<int> predecessors(boost::num_vertices(graph));
        std::vector<double> distances(boost::num_vertices(graph));
        for (unsigned int i = 0; i < boost::num_vertices(graph); ++i) {
            predecessors[i] = i;
            distances[i] = -1.0;
        }


        ConstIndexPropertyMap index_map = boost::get(boost::vertex_index, graph);
        ConstEdgeWeightPropertyMap edge_weight_map = boost::get(boost::edge_weight, graph);


        // do the actual path finding using verbose boost magic...
        try {
            boost::dijkstra_shortest_paths(graph, system1_index, &predecessors[0], &distances[0], edge_weight_map, index_map, 
                                           std::less<double>(), std::plus<double>(), std::numeric_limits<int>::max(), 0, 
                                           boost::make_dijkstra_visitor(short_circuiting_visitor(system2_index)));
        } catch (const short_circuiting_visitor::found_destination&) {
            // catching this just means that the destination was found, and so the algorithm was exited early, via exception
        }


        int current_system = system2_index;
        while (predecessors[current_system] != current_system) {
            retval.first.push_front(sys_id_property_map[current_system]);
            current_system = predecessors[current_system];
        }
        retval.second = distances[system2_index];

        if (retval.first.empty()) {
            // there is no path between the specified nodes
            retval.second = -1.0;
            return retval;
        } else {
            // add start system to path, as it wasn't added by traversing predecessors array
            retval.first.push_front(sys_id_property_map[system1_index]);
        }

        return retval;
    }

    /** Returns the path between vertices \a system1_id and \a system2_id of
      * \a graph that takes the fewest number of jumps (edge traversals), and
      * the number of jumps this path takes.  If system1_id is the same vertex
      * as system2_id, the path has just that system in it, and the path lenth
      * is 0.  If there is no path between the two vertices, then the list is
      * empty and the path length is -1 */
    template <class Graph>
    std::pair<std::list<int>, int> LeastJumpsPathImpl(const Graph& graph, int system1_id, int system2_id,
                                                      const boost::unordered_map<int, int>& id_to_graph_index,
                                                      int max_jumps = INT_MAX)
    {
        typedef typename boost::property_map<Graph, vertex_system_id_t>::const_type ConstSystemIDPropertyMap;

        ConstSystemIDPropertyMap sys_id_property_map = boost::get(vertex_system_id_t(), graph);
        std::pair<std::list<int>, int> retval;

        int system1_index = id_to_graph_index.at(system1_id);
        int system2_index = id_to_graph_index.at(system2_id);

        // early exit if systems are the same
        if (system1_id == system2_id) {
            retval.first.push_back(system2_id);
            retval.second = 0;  // no jumps needed
            return retval;
        }

        /* initializing all vertices' predecessors to themselves prevents endless loops when back traversing the tree in the case where
           one of the end systems is system 0, because systems that are not connected to the root system (system2) are not visited
           by the search, and so their predecessors are left unchanged.  Default initialization of the vector may be 0 or undefined
           which could lead to out of bounds errors, or endless loops if a system's default predecessor is 0, (debug mode) and 0's
           predecessor is that system */
        std::vector<int> predecessors(boost::num_vertices(graph));
        for (unsigned int i = 0; i < boost::num_vertices(graph); ++i)
            predecessors[i] = i;


        // do the actual path finding using verbose boost magic...
        typedef bfs_visitor<Graph, typename boost::graph_traits<Graph>::edge_descriptor, int> BFSVisitor;
        try {
            boost::queue<int> buf;
            std::vector<int> colors(boost::num_vertices(graph));

            BFSVisitor bfsVisitor(system1_index, system2_index, &predecessors[0], max_jumps);
            boost::breadth_first_search(graph, system1_index, buf, bfsVisitor, &colors[0]);
        } catch (const typename BFSVisitor::reached_depth_limit&) {
            // catching this means the algorithm explored the neighborhood until max_jumps and didn't find anything
            return std::make_pair(std::list<int>(), -1);
        } catch (const typename BFSVisitor::found_destination&) {
            // catching this just means that the destination was found, and so the algorithm was exited early, via exception
        }


        int current_system = system2_index;
        while (predecessors[current_system] != current_system) {
            retval.first.push_front(sys_id_property_map[current_system]);
            current_system = predecessors[current_system];
        }
        retval.second = retval.first.size() - 1;    // number of jumps is number of systems in path minus one for the starting system

        if (retval.first.empty()) {
            // there is no path between the specified nodes
            retval.second = -1;
        } else {
            // add start system to path, as it wasn't added by traversing predecessors array
            retval.first.push_front(sys_id_property_map[system1_index]);
        }

        return retval;
    }

    template <class Graph>
    std::multimap<double, int> ImmediateNeighborsImpl(const Graph& graph, int system_id,
                                                      const boost::unordered_map<int, int>& id_to_graph_index)
    {
        typedef typename Graph::out_edge_iterator OutEdgeIterator;
        typedef typename boost::property_map<Graph, vertex_system_id_t>::const_type ConstSystemIDPropertyMap;
        typedef typename boost::property_map<Graph, boost::edge_weight_t>::const_type ConstEdgeWeightPropertyMap;

        std::multimap<double, int> retval;
        ConstEdgeWeightPropertyMap edge_weight_map = boost::get(boost::edge_weight, graph);
        ConstSystemIDPropertyMap sys_id_property_map = boost::get(vertex_system_id_t(), graph);
        std::pair<OutEdgeIterator, OutEdgeIterator> edges = boost::out_edges(id_to_graph_index.at(system_id), graph);
        for (OutEdgeIterator it = edges.first; it != edges.second; ++it)
        { retval.insert(std::make_pair(edge_weight_map[*it], sys_id_property_map[boost::target(*it, graph)])); }
        return retval;
    }
#endif

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

}


struct map
{
    unsigned int width;
    unsigned int height;
};

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
{ return hex_coord(0, -1); }

hex_coord hex_below (hex_coord hc)
{ return hex_coord(0, 1); }

hex_coord hex_above_left (hex_coord hc)
{ return hex_coord(hc.x - 1, hc.y + (hc.x % 2 ? -1 : 0)); }

hex_coord hex_above_right (hex_coord hc)
{ return hex_coord(hc.x - 1, hc.y + (hc.x % 2 ?  0 : 1)); }

hex_coord hex_below_left (hex_coord hc)
{ return hex_coord(hc.x + 1, hc.y + (hc.x % 2 ? -1 : 0)); }

hex_coord hex_below_right (hex_coord hc)
{ return hex_coord(hc.x + 1, hc.y + (hc.x % 2 ?  0 : 1)); }


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


void init_graph (graph::graph& g, map m)
{
    graph::hex_id_property_map sys_id_property_map =
        boost::get(graph::vertex_hex_id_t(), g);

    graph::EdgeWeightPropertyMap edge_weight_map =
        boost::get(boost::edge_weight, g);

    {
        unsigned int i = 0;
        for (unsigned int x = 0; x < m.width; ++x) {
            for (unsigned int y = 0; y < m.height; ++y, ++i) {
                unsigned int id = hex_id(hex_coord(x, y));
                boost::add_vertex(g);
                sys_id_property_map[i] = id;
            }
        }
    }

    {
        unsigned int i = 0;
        for (unsigned int x = 0; x < m.width; ++x) {
            for (unsigned int y = 0; y < m.height; ++y, ++i) {
                hex_coord coord(x, y);
                for (hex_direction d = above; d < below; d = hex_direction(d + 1)) {
                    hex_coord adjacent_coord = adjacent_hex_coord(coord, d);
                    if (on_map(adjacent_coord, m)) {
                        std::pair<graph::edge_descriptor, bool> add_edge_result =
                            boost::add_edge(i, hex_id(adjacent_coord), g);
                        edge_weight_map[add_edge_result.first] = 1.0; // TODO
                    }
                }
            }
        }
    }
}

#if 1
int main ()
{
    graph::graph g;
    map m = {150, 24};
    init_graph(g, m);
    std::cerr << "hello, graph!\n";
    return 0;
}
#endif
