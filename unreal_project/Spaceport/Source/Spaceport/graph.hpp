#pragma once

#include "hex_operations.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/container/static_vector.hpp>

#define USE_PRINT_GRAPH 0
#if USE_PRINT_GRAPH
#if _MSC_VER
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#include <iostream>
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

    inline graph_t full_local_graph (int radius = max_useful_hex_distance)
    {
        using edge_pair_t = std::pair<int, int>;

        // Edges derived from calling print_local_graph().
        const edge_pair_t graph_edges[] = {
            edge_pair_t{0, 2},
            edge_pair_t{0, 3},
            edge_pair_t{0, 4},
            edge_pair_t{0, 5},
            edge_pair_t{0, 6},
            edge_pair_t{0, 1},
            edge_pair_t{1, 9},
            edge_pair_t{1, 8},
            edge_pair_t{1, 7},
            edge_pair_t{1, 6},
            edge_pair_t{2, 11},
            edge_pair_t{2, 10},
            edge_pair_t{2, 9},
            edge_pair_t{2, 1},
            edge_pair_t{3, 13},
            edge_pair_t{3, 12},
            edge_pair_t{3, 11},
            edge_pair_t{3, 2},
            edge_pair_t{4, 15},
            edge_pair_t{4, 14},
            edge_pair_t{4, 13},
            edge_pair_t{4, 3},
            edge_pair_t{5, 17},
            edge_pair_t{5, 16},
            edge_pair_t{5, 15},
            edge_pair_t{5, 4},
            edge_pair_t{6, 7},
            edge_pair_t{6, 18},
            edge_pair_t{6, 17},
            edge_pair_t{6, 5},
            edge_pair_t{7, 8},
            edge_pair_t{7, 20},
            edge_pair_t{7, 19},
            edge_pair_t{7, 18},
            edge_pair_t{8, 22},
            edge_pair_t{8, 21},
            edge_pair_t{8, 20},
            edge_pair_t{9, 10},
            edge_pair_t{9, 23},
            edge_pair_t{9, 22},
            edge_pair_t{9, 8},
            edge_pair_t{10, 25},
            edge_pair_t{10, 24},
            edge_pair_t{10, 23},
            edge_pair_t{11, 12},
            edge_pair_t{11, 26},
            edge_pair_t{11, 25},
            edge_pair_t{11, 10},
            edge_pair_t{12, 28},
            edge_pair_t{12, 27},
            edge_pair_t{12, 26},
            edge_pair_t{13, 14},
            edge_pair_t{13, 29},
            edge_pair_t{13, 28},
            edge_pair_t{13, 12},
            edge_pair_t{14, 31},
            edge_pair_t{14, 30},
            edge_pair_t{14, 29},
            edge_pair_t{15, 16},
            edge_pair_t{15, 32},
            edge_pair_t{15, 31},
            edge_pair_t{15, 14},
            edge_pair_t{16, 34},
            edge_pair_t{16, 33},
            edge_pair_t{16, 32},
            edge_pair_t{17, 18},
            edge_pair_t{17, 35},
            edge_pair_t{17, 34},
            edge_pair_t{17, 16},
            edge_pair_t{18, 19},
            edge_pair_t{18, 36},
            edge_pair_t{18, 35},
            edge_pair_t{19, 20},
            edge_pair_t{19, 38},
            edge_pair_t{19, 37},
            edge_pair_t{19, 36},
            edge_pair_t{20, 21},
            edge_pair_t{20, 39},
            edge_pair_t{20, 38},
            edge_pair_t{21, 41},
            edge_pair_t{21, 40},
            edge_pair_t{21, 39},
            edge_pair_t{22, 23},
            edge_pair_t{22, 42},
            edge_pair_t{22, 41},
            edge_pair_t{22, 21},
            edge_pair_t{23, 24},
            edge_pair_t{23, 43},
            edge_pair_t{23, 42},
            edge_pair_t{24, 45},
            edge_pair_t{24, 44},
            edge_pair_t{24, 43},
            edge_pair_t{25, 26},
            edge_pair_t{25, 46},
            edge_pair_t{25, 45},
            edge_pair_t{25, 24},
            edge_pair_t{26, 27},
            edge_pair_t{26, 47},
            edge_pair_t{26, 46},
            edge_pair_t{27, 49},
            edge_pair_t{27, 48},
            edge_pair_t{27, 47},
            edge_pair_t{28, 29},
            edge_pair_t{28, 50},
            edge_pair_t{28, 49},
            edge_pair_t{28, 27},
            edge_pair_t{29, 30},
            edge_pair_t{29, 51},
            edge_pair_t{29, 50},
            edge_pair_t{30, 53},
            edge_pair_t{30, 52},
            edge_pair_t{30, 51},
            edge_pair_t{31, 32},
            edge_pair_t{31, 54},
            edge_pair_t{31, 53},
            edge_pair_t{31, 30},
            edge_pair_t{32, 33},
            edge_pair_t{32, 55},
            edge_pair_t{32, 54},
            edge_pair_t{33, 57},
            edge_pair_t{33, 56},
            edge_pair_t{33, 55},
            edge_pair_t{34, 35},
            edge_pair_t{34, 58},
            edge_pair_t{34, 57},
            edge_pair_t{34, 33},
            edge_pair_t{35, 36},
            edge_pair_t{35, 59},
            edge_pair_t{35, 58},
            edge_pair_t{36, 37},
            edge_pair_t{36, 60},
            edge_pair_t{36, 59},
            edge_pair_t{37, 38},
            edge_pair_t{37, 62},
            edge_pair_t{37, 61},
            edge_pair_t{37, 60},
            edge_pair_t{38, 39},
            edge_pair_t{38, 63},
            edge_pair_t{38, 62},
            edge_pair_t{39, 40},
            edge_pair_t{39, 64},
            edge_pair_t{39, 63},
            edge_pair_t{40, 66},
            edge_pair_t{40, 65},
            edge_pair_t{40, 64},
            edge_pair_t{41, 42},
            edge_pair_t{41, 67},
            edge_pair_t{41, 66},
            edge_pair_t{41, 40},
            edge_pair_t{42, 43},
            edge_pair_t{42, 68},
            edge_pair_t{42, 67},
            edge_pair_t{43, 44},
            edge_pair_t{43, 69},
            edge_pair_t{43, 68},
            edge_pair_t{44, 71},
            edge_pair_t{44, 70},
            edge_pair_t{44, 69},
            edge_pair_t{45, 46},
            edge_pair_t{45, 72},
            edge_pair_t{45, 71},
            edge_pair_t{45, 44},
            edge_pair_t{46, 47},
            edge_pair_t{46, 73},
            edge_pair_t{46, 72},
            edge_pair_t{47, 48},
            edge_pair_t{47, 74},
            edge_pair_t{47, 73},
            edge_pair_t{48, 76},
            edge_pair_t{48, 75},
            edge_pair_t{48, 74},
            edge_pair_t{49, 50},
            edge_pair_t{49, 77},
            edge_pair_t{49, 76},
            edge_pair_t{49, 48},
            edge_pair_t{50, 51},
            edge_pair_t{50, 78},
            edge_pair_t{50, 77},
            edge_pair_t{51, 52},
            edge_pair_t{51, 79},
            edge_pair_t{51, 78},
            edge_pair_t{52, 81},
            edge_pair_t{52, 80},
            edge_pair_t{52, 79},
            edge_pair_t{53, 54},
            edge_pair_t{53, 82},
            edge_pair_t{53, 81},
            edge_pair_t{53, 52},
            edge_pair_t{54, 55},
            edge_pair_t{54, 83},
            edge_pair_t{54, 82},
            edge_pair_t{55, 56},
            edge_pair_t{55, 84},
            edge_pair_t{55, 83},
            edge_pair_t{56, 86},
            edge_pair_t{56, 85},
            edge_pair_t{56, 84},
            edge_pair_t{57, 58},
            edge_pair_t{57, 87},
            edge_pair_t{57, 86},
            edge_pair_t{57, 56},
            edge_pair_t{58, 59},
            edge_pair_t{58, 88},
            edge_pair_t{58, 87},
            edge_pair_t{59, 60},
            edge_pair_t{59, 89},
            edge_pair_t{59, 88},
            edge_pair_t{60, 61},
            edge_pair_t{60, 90},
            edge_pair_t{60, 89},
            edge_pair_t{61, 62},
            edge_pair_t{61, 92},
            edge_pair_t{61, 91},
            edge_pair_t{61, 90},
            edge_pair_t{62, 63},
            edge_pair_t{62, 93},
            edge_pair_t{62, 92},
            edge_pair_t{63, 64},
            edge_pair_t{63, 94},
            edge_pair_t{63, 93},
            edge_pair_t{64, 65},
            edge_pair_t{64, 95},
            edge_pair_t{64, 94},
            edge_pair_t{65, 97},
            edge_pair_t{65, 96},
            edge_pair_t{65, 95},
            edge_pair_t{66, 67},
            edge_pair_t{66, 98},
            edge_pair_t{66, 97},
            edge_pair_t{66, 65},
            edge_pair_t{67, 68},
            edge_pair_t{67, 99},
            edge_pair_t{67, 98},
            edge_pair_t{68, 69},
            edge_pair_t{68, 100},
            edge_pair_t{68, 99},
            edge_pair_t{69, 70},
            edge_pair_t{69, 101},
            edge_pair_t{69, 100},
            edge_pair_t{70, 103},
            edge_pair_t{70, 102},
            edge_pair_t{70, 101},
            edge_pair_t{71, 72},
            edge_pair_t{71, 104},
            edge_pair_t{71, 103},
            edge_pair_t{71, 70},
            edge_pair_t{72, 73},
            edge_pair_t{72, 105},
            edge_pair_t{72, 104},
            edge_pair_t{73, 74},
            edge_pair_t{73, 106},
            edge_pair_t{73, 105},
            edge_pair_t{74, 75},
            edge_pair_t{74, 107},
            edge_pair_t{74, 106},
            edge_pair_t{75, 109},
            edge_pair_t{75, 108},
            edge_pair_t{75, 107},
            edge_pair_t{76, 77},
            edge_pair_t{76, 110},
            edge_pair_t{76, 109},
            edge_pair_t{76, 75},
            edge_pair_t{77, 78},
            edge_pair_t{77, 111},
            edge_pair_t{77, 110},
            edge_pair_t{78, 79},
            edge_pair_t{78, 112},
            edge_pair_t{78, 111},
            edge_pair_t{79, 80},
            edge_pair_t{79, 113},
            edge_pair_t{79, 112},
            edge_pair_t{80, 115},
            edge_pair_t{80, 114},
            edge_pair_t{80, 113},
            edge_pair_t{81, 82},
            edge_pair_t{81, 116},
            edge_pair_t{81, 115},
            edge_pair_t{81, 80},
            edge_pair_t{82, 83},
            edge_pair_t{82, 117},
            edge_pair_t{82, 116},
            edge_pair_t{83, 84},
            edge_pair_t{83, 118},
            edge_pair_t{83, 117},
            edge_pair_t{84, 85},
            edge_pair_t{84, 119},
            edge_pair_t{84, 118},
            edge_pair_t{85, 121},
            edge_pair_t{85, 120},
            edge_pair_t{85, 119},
            edge_pair_t{86, 87},
            edge_pair_t{86, 122},
            edge_pair_t{86, 121},
            edge_pair_t{86, 85},
            edge_pair_t{87, 88},
            edge_pair_t{87, 123},
            edge_pair_t{87, 122},
            edge_pair_t{88, 89},
            edge_pair_t{88, 124},
            edge_pair_t{88, 123},
            edge_pair_t{89, 90},
            edge_pair_t{89, 125},
            edge_pair_t{89, 124},
            edge_pair_t{90, 91},
            edge_pair_t{90, 126},
            edge_pair_t{90, 125},
            edge_pair_t{91, 92},
            edge_pair_t{91, 126},
            edge_pair_t{92, 93},
            edge_pair_t{93, 94},
            edge_pair_t{94, 95},
            edge_pair_t{95, 96},
            edge_pair_t{97, 98},
            edge_pair_t{97, 96},
            edge_pair_t{98, 99},
            edge_pair_t{99, 100},
            edge_pair_t{100, 101},
            edge_pair_t{101, 102},
            edge_pair_t{103, 104},
            edge_pair_t{103, 102},
            edge_pair_t{104, 105},
            edge_pair_t{105, 106},
            edge_pair_t{106, 107},
            edge_pair_t{107, 108},
            edge_pair_t{109, 110},
            edge_pair_t{109, 108},
            edge_pair_t{110, 111},
            edge_pair_t{111, 112},
            edge_pair_t{112, 113},
            edge_pair_t{113, 114},
            edge_pair_t{115, 116},
            edge_pair_t{115, 114},
            edge_pair_t{116, 117},
            edge_pair_t{117, 118},
            edge_pair_t{118, 119},
            edge_pair_t{119, 120},
            edge_pair_t{121, 122},
            edge_pair_t{121, 120},
            edge_pair_t{122, 123},
            edge_pair_t{123, 124},
            edge_pair_t{124, 125},
            edge_pair_t{125, 126},
        };

        graph_t retval(
            boost::edges_are_unsorted_multi_pass,
            std::begin(graph_edges), std::end(graph_edges),
            max_neighbors
        );

        return retval;
    }

#if USE_PRINT_GRAPH
    inline void print_local_graph (int radius = max_useful_hex_distance)
    {
        assert(radius <= max_useful_hex_distance);

        hex_coord_t const hc{20, 20};
        auto const width = 50;
        auto const height = 50;
        auto const neighbors = adjacent_hex_coords(hc, width, height, radius);

        using offset_t = int16_t;
        offset_t * offset_indices = (offset_t *)alloca(width * height * sizeof(offset_t));
        std::fill(offset_indices, offset_indices + width * height, offset_t{-1});

        using edge_pair_t = std::pair<int, int>;
        boost::container::static_vector<edge_pair_t, 6 * max_neighbors> edges;

        {
            offset_t i = 0;
            for (auto const neighbor : neighbors) {
                offset_indices[hex_index_t(neighbor, width)] = i++;
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
                            }
                        }

                        // Connect to the next CW hex.
                        adjacent_hc = adjacent_hex_coord(current, --adjacent_d);
                        if (on_map(adjacent_hc, width, height)) {
                            int const other_vertex =
                                offset_indices[hex_index_t(adjacent_hc, width)];
                            if (other_vertex != -1) {
                                edges.push_back(edge_pair_t(current_vertex, other_vertex));
                            }
                        }

                        // Connect to the next CW hex.
                        adjacent_hc = adjacent_hex_coord(current, --adjacent_d);
                        if (on_map(adjacent_hc, width, height)) {
                            int const other_vertex =
                                offset_indices[hex_index_t(adjacent_hc, width)];
                            if (other_vertex != -1) {
                                edges.push_back(edge_pair_t(current_vertex, other_vertex));
                            }
                        }

                        if (k == 0) {
                            adjacent_hc = adjacent_hex_coord(current, --adjacent_d);

                            if (on_map(adjacent_hc, width, height)) {
                                int const other_vertex =
                                    offset_indices[hex_index_t(adjacent_hc, width)];
                                if (other_vertex != -1) {
                                    edges.push_back(edge_pair_t(current_vertex, other_vertex));
                                }
                            }
                        }
                    }
                }
            }
        }

        std::cout << "constexpr const edge_pair_t graph_edges[] = {\n";
        for (auto edge : edges) {
            std::cout << "    edge_pair_t{" << edge.first << ", " << edge.second << "},\n";
        }
        std::cout << "}\n";
    }
#endif

}
