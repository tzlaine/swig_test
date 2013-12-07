#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

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

    /** Used to short-circuit the use of BFS (breadth-first search) or
      * Dijkstra's algorithm for pathfinding when it finds the desired
      * destination system. */
    struct PathFindingShortCircuitingVisitor : public boost::base_visitor<PathFindingShortCircuitingVisitor>
    {
        typedef boost::on_finish_vertex event_filter;

        struct FoundDestination {}; // exception type thrown when destination is found

        PathFindingShortCircuitingVisitor(int dest_system) : destination_system(dest_system) {}
        template <class Vertex, class Graph>
        void operator()(Vertex u, Graph& g)
        {
            if (static_cast<int>(u) == destination_system)
                throw FoundDestination();
        }
        const int destination_system;
    };

    /** Complete BFS visitor implementing:
      *  - predecessor recording
      *  - short-circuit exit on found match
      *  - maximum search depth 
      */
    template <class Graph, class Edge, class Vertex>
    class BFSVisitorImpl
    {
    public:
        class FoundDestination {}; 
        class ReachedDepthLimit {};

    private:
        Vertex m_marker;
        Vertex m_stop;
        Vertex m_source;
        Vertex * m_predecessors;
        int m_levels_remaining;
        bool m_level_complete;

    public:
        BFSVisitorImpl(const Vertex& start, const Vertex& stop, Vertex predecessors[], int max_depth)
            : m_marker(start),
              m_stop(stop),
              m_source(start),
              m_predecessors(predecessors),
              m_levels_remaining(max_depth),
              m_level_complete(false)
        {}

        void initialize_vertex(const Vertex& v, const Graph& g)
        {}

        void discover_vertex(const Vertex& v, const Graph& g) {
            m_predecessors[static_cast<int>(v)] = m_source;

            if (v == m_stop)
                throw FoundDestination();

            if (m_level_complete) {
                m_marker = v;
                m_level_complete = false;
            }
        }

        void examine_vertex(const Vertex& v, const Graph& g) {
            if (v == m_marker) {
                if (!m_levels_remaining)
                    throw ReachedDepthLimit();
                m_levels_remaining--;
                m_level_complete = true;
            }
            
            m_source = v; // avoid re-calculating source from edge
        }

        void examine_edge(const Edge& e, const Graph& g) {}
        void tree_edge(const Edge& e, const Graph& g) {}    // wait till target is calculated


        void non_tree_edge(const Edge& e, const Graph& g) {}
        void gray_target(const Edge& e, const Graph& g) {}
        void black_target(const Edge& e, const Graph& g) {}
        void finish_vertex(const Vertex& e, const Graph& g) {}
    };

    ////////////////////////////////////////////////////////////////
    // templated implementations of Universe graph search methods //
    ////////////////////////////////////////////////////////////////
    struct vertex_system_id_t {typedef boost::vertex_property_tag kind;}; ///< a system graph property map type


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
                                           boost::make_dijkstra_visitor(PathFindingShortCircuitingVisitor(system2_index)));
        } catch (const PathFindingShortCircuitingVisitor::FoundDestination&) {
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
        typedef BFSVisitorImpl<Graph, typename boost::graph_traits<Graph>::edge_descriptor, int> BFSVisitor;
        try {
            boost::queue<int> buf;
            std::vector<int> colors(boost::num_vertices(graph));

            BFSVisitor bfsVisitor(system1_index, system2_index, &predecessors[0], max_jumps);
            boost::breadth_first_search(graph, system1_index, buf, bfsVisitor, &colors[0]);
        } catch (const typename BFSVisitor::ReachedDepthLimit&) {
            // catching this means the algorithm explored the neighborhood until max_jumps and didn't find anything
            return std::make_pair(std::list<int>(), -1);
        } catch (const typename BFSVisitor::FoundDestination&) {
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

}
using namespace graph;  // to keep GCC 4.2 on OSX happy

/////////////////////////////////////////////
// struct GraphImpl
/////////////////////////////////////////////
struct GraphImpl
{
    typedef boost::property<
        vertex_system_id_t,
        int,
        boost::property<boost::vertex_index_t, int>
    > vertex_property_t;

    typedef boost::property<
        boost::edge_weight_t,
        double
    > edge_property_t;

    // declare main graph types, including properties declared above
    // could add boost::disallow_parallel_edge_tag GraphProperty but it doesn't
    // work for vecS vector-based lists and parallel edges can be avoided while
    // creating the graph by filtering the edges to be added
    typedef boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::undirectedS,
        vertex_property_t,
        edge_property_t
    > SystemGraph;

    // declare property map types for properties declared above
    typedef boost::property_map<SystemGraph, vertex_system_id_t>::const_type        ConstSystemIDPropertyMap;
    typedef boost::property_map<SystemGraph, vertex_system_id_t>::type              SystemIDPropertyMap;
    typedef boost::property_map<SystemGraph, boost::vertex_index_t>::const_type     ConstIndexPropertyMap;
    typedef boost::property_map<SystemGraph, boost::vertex_index_t>::type           IndexPropertyMap;
    typedef boost::property_map<SystemGraph, boost::edge_weight_t>::const_type      ConstEdgeWeightPropertyMap;
    typedef boost::property_map<SystemGraph, boost::edge_weight_t>::type            EdgeWeightPropertyMap;

    typedef boost::graph_traits<GraphImpl::SystemGraph>::edge_descriptor EdgeDescriptor;

    SystemGraph system_graph; ///< a graph in which the systems are vertices and the starlanes are edges
};


#if 0
void Universe::InitializeSystemGraph(int for_empire_id) {
    typedef boost::graph_traits<GraphImpl::SystemGraph>::edge_descriptor EdgeDescriptor;

    for (int i = static_cast<int>(boost::num_vertices(m_graph_impl->system_graph)) - 1; i >= 0; --i) {
        boost::clear_vertex(i, m_graph_impl->system_graph);
        boost::remove_vertex(i, m_graph_impl->system_graph);
    }

    std::vector<int> system_ids = ::Objects().FindObjectIDs<System>();
    //Logger().debugStream() << "InitializeSystemGraph(" << for_empire_id << ") system_ids: (" << system_ids.size() << ")";
    //for (std::vector<int>::const_iterator it = system_ids.begin(); it != system_ids.end(); ++it)
    //    Logger().debugStream() << " ... " << *it;

    GraphImpl::SystemIDPropertyMap sys_id_property_map =
        boost::get(vertex_system_id_t(), m_graph_impl->system_graph);

    GraphImpl::EdgeWeightPropertyMap edge_weight_map =
        boost::get(boost::edge_weight, m_graph_impl->system_graph);

    std::map<int, int> system_id_graph_index_reverse_lookup_map;    // key is system ID, value is index in m_graph_impl->system_graph of system's vertex

    for (int i = 0; i < static_cast<int>(system_ids.size()); ++i) {
        // add a vertex to the graph for this system, and assign it the system's universe ID as a property
        boost::add_vertex(m_graph_impl->system_graph);
        int system_id = system_ids[i];
        sys_id_property_map[i] = system_id;
        // add record of index in m_graph_impl->system_graph of this system
        system_id_graph_index_reverse_lookup_map[system_id] = i;
    }

    m_system_distances.resize(system_ids.size(), system_ids.size());
    for (int i = 0; i < static_cast<int>(system_ids.size()); ++i) {
        int system1_id = system_ids[i];
        TemporaryPtr<const System> system1 = GetEmpireKnownSystem(system1_id, for_empire_id);

        m_system_id_to_graph_index[system1_id] = i;

        // add edges and edge weights
        for (System::const_lane_iterator it = system1->begin_lanes(); it != system1->end_lanes(); ++it) {
            // get id in universe of system at other end of lane
            const int lane_dest_id = it->first;
            // skip null lanes and only add edges in one direction, to avoid
            // duplicating edges ( since this is an undirected graph, A->B
            // duplicates B->A )
            if (lane_dest_id >= system1_id)
                continue;

            // get m_graph_impl->system_graph index for this system
            std::map<int, int>::iterator reverse_lookup_map_it = system_id_graph_index_reverse_lookup_map.find(lane_dest_id);
            if (reverse_lookup_map_it == system_id_graph_index_reverse_lookup_map.end())
                continue;   // couldn't find destination system id in reverse lookup map; don't add to graph
            int lane_dest_graph_index = reverse_lookup_map_it->second;

            std::pair<EdgeDescriptor, bool> add_edge_result =
                boost::add_edge(i, lane_dest_graph_index, m_graph_impl->system_graph);

            if (add_edge_result.second) {                   // if this is a non-duplicate starlane or wormhole
                if (it->second) {                               // if this is a wormhole
                    edge_weight_map[add_edge_result.first] = 0.1;   // arbitrary small distance
                } else {                                        // if this is a starlane
                    TemporaryPtr<const UniverseObject> system2 = GetUniverseObject(it->first);
                    double x_dist = system2->X() - system1->X();
                    double y_dist = system2->Y() - system1->Y();
                    edge_weight_map[add_edge_result.first] = std::sqrt(x_dist*x_dist + y_dist*y_dist);
                    //std::cout << "edge_weight_map " << system1_id << " to " << lane_dest_id << ": " << edge_weight_map[add_edge_result.first] << std::endl;
                }
            }
        }

        // define the straight-line system distances for this system
        for (int j = 0; j < i; ++j) {
            int system2_id = system_ids[j];
            TemporaryPtr<const UniverseObject> system2 = GetUniverseObject(system2_id);
            double x_dist = system2->X() - system1->X();
            double y_dist = system2->Y() - system1->Y();
            m_system_distances[i][j] = std::sqrt(x_dist*x_dist + y_dist*y_dist);
        }
        m_system_distances[i][i] = 0.0;
    }

    m_system_jumps.resize(system_ids.size(), system_ids.size());
    constant_property<EdgeDescriptor, short> jump_weight = { 1 };
    boost::johnson_all_pairs_shortest_paths(m_graph_impl->system_graph, m_system_jumps, boost::weight_map(jump_weight));

    UpdateEmpireVisibilityFilteredSystemGraphs(for_empire_id);
}
#endif


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
    above,
    below,
    above_left,
    above_right,
    below_left,
    below_right
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
        hex_above,
        hex_below,
        hex_above_left,
        hex_above_right,
        hex_below_left,
        hex_below_right
    };
    return fns[hd](hc);
}


bool on_map (hex_coord hc, unsigned int map_width, unsigned int map_height)
{ return hc.x < map_width && hc.y < map_height; }


unsigned int hex_id (hex_coord hc)
{ return hc.x * 100 + hc.y; }


void init_graph (GraphImpl& graph, unsigned int map_width, unsigned int map_height)
{
    GraphImpl::SystemIDPropertyMap sys_id_property_map =
        boost::get(vertex_system_id_t(), graph.system_graph);

    GraphImpl::EdgeWeightPropertyMap edge_weight_map =
        boost::get(boost::edge_weight, graph.system_graph);

    {
        unsigned int i = 0;
        for (unsigned int x = 0; x < map_width; ++x) {
            for (unsigned int y = 0; y < map_height; ++y, ++i) {
                unsigned int id = hex_id(hex_coord(x, y));
                boost::add_vertex(graph.system_graph);
                sys_id_property_map[i] = id;
            }
        }
    }

    {
        unsigned int i = 0;
        for (unsigned int x = 0; x < map_width; ++x) {
            for (unsigned int y = 0; y < map_height; ++y, ++i) {
                hex_coord coord(x, y);
                for (hex_direction d = hex_direction(0); d < hex_direction(6); d = hex_direction(d + 1)) {
                    hex_coord adjacent_coord = adjacent_hex_coord(coord, d);
                    if (on_map(adjacent_coord, map_width, map_height)) {
                        std::pair<GraphImpl::EdgeDescriptor, bool> add_edge_result =
                            boost::add_edge(i, hex_id(adjacent_coord), graph.system_graph);
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
    GraphImpl graph;
    init_graph(graph, 150, 24);
    std::cerr << "hello, graph!\n";
    return 0;
}
#endif
