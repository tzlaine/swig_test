#pragma once

#if defined(BUILD_LIBRARY) && defined(_MSC_VER)
#define GRAPH_ALGO_API __declspec(dllexport)
#else
#define GRAPH_ALGO_API
#endif

extern "C" {

    struct supply_check_hex_t
    {
        int owner_id;

        // These values are all encoded such that nation N has an X present if
        // (X & (1 << N)).
        int ship;
        int nonship_unit;
        int base_with_fighters;
        int planet;
        int SB;
        int BATS;
        int MB;
        int convoy;
        int supply_tug;
    };

    struct ga_hex_t
    {
        int a;
        float b;
    };

    GRAPH_ALGO_API
    int test (int i);

    GRAPH_ALGO_API
    float test_2 (int n, float floats[]);

    GRAPH_ALGO_API
    float test_3 (int n, ga_hex_t* hexes);

    // Returns an int for each hex, containing a grid ID in the first 8 bits
    // (0 is no grid, 1 is main capital grid, 2 is main offmap grid, anything
    // else is a partial grid).  Bits 8-23 contain the nations supplying this
    // hex (a '1' in bit N indicates that the nation with ID N-8 is supplying
    // it).  Bit 24 contains a flag indicating supplies must be paid for by
    // the hex's owner to supply ships in this hex (meaning the hex is in a
    // partial supply grid, and does not include a free-supply feature like a
    // SB, BATS, or planet).
    GRAPH_ALGO_API
    int* determine_supply (
        int w, int h,
        supply_check_hex_t hexes[],
        int neutral_zone_id,
        int nations,
        int nation_team_membership[],
        int capitals[],
        int max_offmap_border_hexes,
        int offmap_border_hexes[]
    );
}
