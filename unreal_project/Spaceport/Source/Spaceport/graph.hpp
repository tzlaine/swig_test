#pragma once

#include "hex_operations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/container/static_vector.hpp>

#if _MSC_VER
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif


namespace graph {

    struct vertex_data_t
    {
        hex_id_t hex_id;
    };

    struct edge_data_t
    {
        edge_data_t () : weight (0) {}
        edge_data_t (float weight) : weight (weight) {}

        float weight;
    };

    using graph_t = boost::compressed_sparse_row_graph<
        boost::bidirectionalS,
        vertex_data_t,
        edge_data_t,
        boost::no_property,
        int
    >;

    template <typename ValidFn, typename WeightFn>
    graph_t local_graph (
        hex_coord_t hc,
        ValidFn valid,
        WeightFn weight,
        int width,
        int height,
        int radius = max_useful_hex_distance
    ) {
        assert(on_map(hc, width, height));
        assert(valid(hc));
        assert(0 < width);
        assert(0 < height);
        assert(0 < radius);
        assert(radius <= max_useful_hex_distance);

        auto const neighbors = adjacent_hex_coords(hc, width, height, radius);

        using offset_t = int16_t;
        offset_t * offset_indices = (offset_t *)alloca(width * height * sizeof(offset_t));
        std::fill(offset_indices, offset_indices + width * height, offset_t{-1});

        using edge_pair_t = std::pair<int, int>;
        boost::container::static_vector<edge_pair_t, 6 * max_neighbors> edges;
        boost::container::static_vector<edge_data_t, 6 * max_neighbors> edge_data;

        boost::container::static_vector<hex_id_t, max_neighbors> vertex_hex_ids;

        {
            offset_t i = 0;
            for (auto const neighbor : neighbors) {
                if (valid(neighbor)) {
                    offset_indices[hex_index_t(neighbor, width)] = i++;
                    vertex_hex_ids.push_back(hex_id_t(neighbor));
                }
            }
        }

        {
            // connect hc to r=1 hexes
            for (hex_direction_t const d : all_hex_directions) {
                auto const adjacent_hc = adjacent_hex_coord(hc, d);
                if (!on_map(adjacent_hc, width, height))
                    continue;
                int const other_vertex = offset_indices[hex_index_t(adjacent_hc, width)];
                if (other_vertex == -1)
                    continue;

                edges.push_back(edge_pair_t(0, other_vertex));
                edge_data_t const edge_weight{weight(hc, adjacent_hc)};
                edge_data.push_back(edge_weight);
            }

            auto current = hc;
            // Each outermost loop iteration J connects each hex at r=J to the
            // other adjacent hexes at r=J, and to all adjacent ones at r=J+1.  At
            // r=N-1, we skip connecting to the next row.
            for (int j = 1; j <= radius; ++j) {
                current = adjacent_hex_coord(current, hex_direction_t::below);
                for (hex_direction_t const d : all_hex_directions) {
                    for (int k = 0; k < j; ++k) {
                        // Step forward.
                        current = adjacent_hex_coord(current, d);
                        if (!on_map(current, width, height))
                            continue;
                        int const current_vertex =
                            offset_indices[hex_index_t(current, width)];
                        if (current_vertex == -1)
                            continue;

                        // Connect to the hex directly ahead.
                        auto adjacent_d = d;
                        auto adjacent_hc = adjacent_hex_coord(current, adjacent_d);
                        if (on_map(adjacent_hc, width, height)) {
                            int const other_vertex =
                                offset_indices[hex_index_t(adjacent_hc, width)];
                            if (other_vertex != -1) {
                                edges.push_back(edge_pair_t(current_vertex, other_vertex));
                                edge_data_t const edge_weight{weight(current, adjacent_hc)};
                                edge_data.push_back(edge_weight);
                            }
                        }

                        // Connect to the next CW hex.
                        adjacent_hc = adjacent_hex_coord(current, --adjacent_d);
                        if (on_map(adjacent_hc, width, height)) {
                            int const other_vertex =
                                offset_indices[hex_index_t(adjacent_hc, width)];
                            if (other_vertex != -1) {
                                edges.push_back(edge_pair_t(current_vertex, other_vertex));
                                edge_data_t const edge_weight{weight(current, adjacent_hc)};
                                edge_data.push_back(edge_weight);
                            }
                        }

                        // Connect to the next CW hex.
                        adjacent_hc = adjacent_hex_coord(current, --adjacent_d);
                        if (on_map(adjacent_hc, width, height)) {
                            int const other_vertex =
                                offset_indices[hex_index_t(adjacent_hc, width)];
                            if (other_vertex != -1) {
                                edges.push_back(edge_pair_t(current_vertex, other_vertex));
                                edge_data_t const edge_weight{weight(current, adjacent_hc)};
                                edge_data.push_back(edge_weight);
                            }
                        }

                        if (k == 0) {
                            adjacent_hc = adjacent_hex_coord(current, --adjacent_d);

                            if (on_map(adjacent_hc, width, height)) {
                                int const other_vertex =
                                    offset_indices[hex_index_t(adjacent_hc, width)];
                                if (other_vertex != -1) {
                                    edges.push_back(edge_pair_t(current_vertex, other_vertex));
                                    edge_data_t const edge_weight{weight(current, adjacent_hc)};
                                    edge_data.push_back(edge_weight);
                                }
                            }
                        }
                    }
                }
            }
        }

        graph::graph_t retval(
            boost::edges_are_unsorted_multi_pass,
            edges.begin(), edges.end(),
            edge_data.begin(),
            vertex_hex_ids.size()
        );

        {
            int i = 0;
            for (auto const hex_id : vertex_hex_ids) {
                retval[i++].hex_id = hex_id;
            }
        }

        return retval;
    }

}
