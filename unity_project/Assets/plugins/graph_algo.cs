using System;
using System.Runtime.InteropServices;


[StructLayout(LayoutKind.Sequential)]
public struct ga_hex_t
{
    public int a;
    public float b;
};

[StructLayout(LayoutKind.Sequential)]
public struct supply_check_hex_t
{
    public int owner_id;
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
    public int presence;
    public int borders_offmap;
};

public class graph_algorithms
{
    [DllImport("graph_algorithms")]
    public static extern int test (int i);

    public static float test_2 (float[] floats)
    {
        return test_2(floats.Length, floats);
    }

    [DllImport("graph_algorithms")]
    private static extern float test_2 (
        int n,
        [In] float[] floats
    );

    public static float test_3 (ga_hex_t[] hexes)
    {
        return test_3(hexes.Length, hexes);
    }

    [DllImport("graph_algorithms")]
    private static extern float test_3 (
        int n,
        [In] ga_hex_t[] hexes
    );

    [DllImport("graph_algorithms")]
    public static extern IntPtr determine_supply (
        int w, int h,
        [In] supply_check_hex_t[] hexes,
        int neutral_zone_id,
        int nations,
        [In] int[] nation_team_membership,
        [In] int[] capitols,
        [In] int[] nation_offmap_areas
    );
};
