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

    struct out_edge_container_tag_t {};
    struct edge_list_container_tag_t {};

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
    struct container_gen< ::graph::out_edge_container_tag_t, ValueType>
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
    struct parallel_edge_traits< ::graph::out_edge_container_tag_t>
    {
        typedef allow_parallel_edge_tag type;
    };


    // Specializations for custom vertex containers.
    template <class ValueType>
    struct container_gen< ::graph::edge_list_container_tag_t, ValueType>
    {
        typedef boost::container::static_vector<ValueType, max_neighbors * 6> type;
    };

    template <class ValueType>
    std::pair<typename boost::container::static_vector<ValueType, max_neighbors * 6>::iterator, bool>
    push (boost::container::static_vector<ValueType, max_neighbors * 6> & c, ValueType const & v)
    {
        c.push_back(v);
        return std::make_pair(boost::prior(c.end()), true);
    }

    template <class ValueType>
    void erase (boost::container::static_vector<ValueType, max_neighbors * 6> & c, ValueType const & x)
    {
        c.erase(std::remove(c.begin(), c.end(), x), c.end());
    }

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
        out_edge_container_tag_t,
        boost::vecS,
        boost::undirectedS,
        vertex_property_t,
        edge_property_t,
        edge_list_container_tag_t
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
        hex_coord_t hc,
        int width,
        int height,
        int radius = max_useful_hex_distance
    ) {
        assert(on_map(hc, width, height));
        assert(0 < width);
        assert(0 < height);
        assert(0 < radius);
        assert(radius <= max_useful_hex_distance);

        hex_id_property_map = boost::get(vertex_hex_id_tag_t(), g);
        edge_weight_map = boost::get(boost::edge_weight, g);

        auto const neighbors = adjacent_hex_coords(hc, width, height, radius);
        std::vector<int> offset_indices(width * height, -1); // TODO: calloc()?
        {
            int i = 0;
            for (auto const neighbor : neighbors) {
                offset_indices[to_hex_index(neighbor, width)] = i++;
            }
        }

        {
            int i = 0;
            for (auto const neighbor : neighbors) {
                int id = to_hex_id(neighbor);
                boost::add_vertex(g);
                hex_id_property_map[i] = id;
            }
        }

        {
            // connect hc to r=1 hexes
            for (hex_direction_t const d : all_hex_directions) {
                auto const adjacent_hc = adjacent_hex_coord(hc, d);
                auto const other_vertex = offset_indices[to_hex_index(adjacent_hc, width)];
                assert(other_vertex != -1);
                std::pair<edge_descriptor_t, bool> const add_edge_result =
                    boost::add_edge(0, other_vertex, g);
                edge_weight_map[add_edge_result.first] = 1.0f;
            }

            int i = 1;
            auto current = hc;
            // Each outermost loop iteration J connects each hex at r=J to the
            // other hexes at r=J, and to all adjacent ones at r=J+1.  At
            // r=N-1, we skip connecting to the next row.
            for (int j = 0; j < radius; ++j) {
                current = adjacent_hex_coord(current, hex_direction_t::below);
                for (hex_direction_t const d : all_hex_directions) {
                    for (int k = 0; k <= j; ++k, ++i) {
                        // Step forward.
                        current = adjacent_hex_coord(current, d);

                        // Connect to the hex directly ahead.
                        auto adjacent_d = d;
                        auto adjacent_hc = adjacent_hex_coord(current, adjacent_d);
                        {
                            auto const other_vertex = offset_indices[to_hex_index(adjacent_hc, width)];
                            if (other_vertex != -1) {
                                std::pair<edge_descriptor_t, bool> const add_edge_result =
                                    boost::add_edge(i, other_vertex, g);
                                edge_weight_map[add_edge_result.first] = 1.0f;
                            }
                        }

                        // Connect to the next CW hex.
                        adjacent_hc = adjacent_hex_coord(current, --adjacent_d);
                        {
                            auto const other_vertex = offset_indices[to_hex_index(adjacent_hc, width)];
                            if (other_vertex != -1) {
                                std::pair<edge_descriptor_t, bool> const add_edge_result =
                                    boost::add_edge(i, other_vertex, g);
                                edge_weight_map[add_edge_result.first] = 1.0f;
                            }
                        }

                        // Connect to the next CW hex.
                        adjacent_hc = adjacent_hex_coord(current, --adjacent_d);
                        {
                            auto const other_vertex = offset_indices[to_hex_index(adjacent_hc, width)];
                            if (other_vertex != -1) {
                                std::pair<edge_descriptor_t, bool> const add_edge_result =
                                    boost::add_edge(i, other_vertex, g);
                                edge_weight_map[add_edge_result.first] = 1.0f;
                            }
                        }

                        if (k == 0) {
                            adjacent_hc = adjacent_hex_coord(current, --adjacent_d);
                            {
                                auto const other_vertex = offset_indices[to_hex_index(adjacent_hc, width)];
                                if (other_vertex != -1) {
                                    std::pair<edge_descriptor_t, bool> const add_edge_result =
                                        boost::add_edge(i, other_vertex, g);
                                    edge_weight_map[add_edge_result.first] = 1.0f;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}
