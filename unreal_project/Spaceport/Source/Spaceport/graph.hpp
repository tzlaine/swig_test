#pragma once

#include "hex_operations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

#include <queue>


namespace detail {

    template <class Key, class Value>
    struct constant_property
    { Value m_value; };

}

namespace boost {

    template <class Key, class Value>
    struct property_traits< ::detail::constant_property<Key, Value> >
    {
        typedef Value value_type;
        typedef Key key_type;
        typedef readable_property_map_tag category;
    };

    template <class Key, class Value>
    Value const & get(const ::detail::constant_property<Key, Value>& pmap, Key const &)
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

    typedef boost::property_map<graph, boost::edge_weight_t>::const_type const_edge_weight_property_map; // todo
    typedef boost::property_map<graph, boost::edge_weight_t>::type edge_weight_property_map;

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
        } catch (reached_depth_limit const &) {
            // The algorithm didn't find hex2_id before reaching max_dist.
            return false;
        } catch (found_destination const &) {
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

    template <typename EdgeFn, typename WeightFn>
    void init_graph (graph& g,
                     hex_id_property_map& hex_id_property_map,
                     edge_weight_property_map& edge_weight_map,
                     int width,
                     int height,
                     EdgeFn make_edge,
                     WeightFn weight)
    {
        hex_id_property_map = boost::get(vertex_hex_id_t(), g);
        edge_weight_map = boost::get(boost::edge_weight, g);

        {
            int i = 0;
            for (int x = 0; x < width; ++x) {
                for (int y = 0; y < height; ++y, ++i) {
                    int id = to_hex_id(hex_coord_t{x, y});
                    boost::add_vertex(g);
                    hex_id_property_map[i] = id;
                }
            }
        }

        {
            int i = 0;
            for (int x = 0; x < width; ++x) {
                for (int y = 0; y < height; ++y, ++i) {
                    hex_coord_t const hc{x, y};
                    for (hex_direction_t d = hex_direction_t::above; d < hex_direction_t::below; ++d) {
                        hex_coord_t const adjacent_hc = adjacent_hex_coord(hc, d);
                        if (on_map(adjacent_hc, width, height)) {
                            int const index = to_hex_index(adjacent_hc, width);
                            if (!make_edge(i, index))
                                continue;
                            std::pair<edge_descriptor, bool> add_edge_result =
                                boost::add_edge(i, index, g);
                            edge_weight_map[add_edge_result.first] = weight(i, index);
                        }
                    }
                }
            }
        }
    }

}
