#pragma once

#include "hex_operations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/container/static_vector.hpp>

#include <queue>


namespace graph {

    struct edge_container_tag_t {};

    template <class Key, class Value>
    struct constant_property_t
    { Value m_value; };

}

namespace boost {

    template <class Key, class Value>
    struct property_traits< ::graph::constant_property_t<Key, Value> >
    {
        typedef Value value_type;
        typedef Key key_type;
        typedef readable_property_map_tag category;
    };

    template <class Key, class Value>
    Value const & get( ::graph::constant_property_t<Key, Value> const & pmap, Key const &)
    { return pmap.m_value; }

    // Specializations for custom out-edge containers.
    template <class ValueType>
    struct container_gen< ::graph::edge_container_tag_t, ValueType>
    {
        typedef boost::container::static_vector<ValueType, 6> type;
    };

    template <class ValueType>
    std::pair<typename boost::container::static_vector<ValueType, 6>::iterator, bool>
    push (boost::container::static_vector<ValueType, 6> & c, ValueType const & v)
    {
        c.push_back(v);
        return std::make_pair(boost::prior(c.end()), true);
    }

    template <class ValueType>
    void erase (boost::container::static_vector<ValueType, 6> & c, ValueType const & x)
    {
        c.erase(std::remove(c.begin(), c.end(), x), c.end());
    }

    template <>
    struct parallel_edge_traits< ::graph::edge_container_tag_t>
    {
        typedef allow_parallel_edge_tag type;
    };
}


namespace graph {

    struct vertex_hex_id_tag_t { typedef boost::vertex_property_tag kind; };

    typedef boost::property<
        vertex_hex_id_tag_t,
        int,
        boost::property<boost::vertex_index_t, int>
    > vertex_property_t;

    typedef boost::property<
        boost::edge_weight_t,
        float
    > edge_property_t;

    typedef boost::adjacency_list<
        edge_container_tag_t,
        boost::vecS,
        boost::undirectedS,
        vertex_property_t,
        edge_property_t,
        boost::vecS
    > graph_t;

    typedef boost::property_map<graph_t, vertex_hex_id_tag_t>::const_type const_hex_id_property_map_t;
    typedef boost::property_map<graph_t, vertex_hex_id_tag_t>::type hex_id_property_map_t;

    typedef boost::property_map<graph_t, boost::edge_weight_t>::const_type const_edge_weight_property_map_t; // todo
    typedef boost::property_map<graph_t, boost::edge_weight_t>::type edge_weight_property_map_t;

    typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor_t;


    inline void init_graph (
        graph_t & g,
        hex_id_property_map_t & hex_id_property_map,
        edge_weight_property_map_t & edge_weight_map,
        int width,
        int height
    ) {
        hex_id_property_map = boost::get(vertex_hex_id_tag_t(), g);
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
                            std::pair<edge_descriptor_t, bool> const add_edge_result =
                                boost::add_edge(i, index, g);
                            edge_weight_map[add_edge_result.first] = 1.0f;
                        }
                    }
                }
            }
        }
    }

}
