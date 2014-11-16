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

    // These values are all encoded such that nation N has an X present if
    // (X & (1 << N)).
    public int ship;
    public int nonship_unit;
    public int base_with_fighters;
    public int planet;
    public int SB;
    public int BATS;
    public int MB;
    public int convoy;
    public int supply_tug;
};

public class graph_algorithms
{
/*
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
*/

    public const int max_offmap_border_hexes = 13;

//    [DllImport("graph_algorithms")]
//    public static extern IntPtr determine_supply (
    public static IntPtr determine_supply (
        int w, int h,
        [In] supply_check_hex_t[] hexes,
        int neutral_zone_id,
        int nations,
        [In] int[] nation_team_membership,
        [In] int[] capitols,
        int max_offmap_border_hexes,
        [In] int[] offmap_border_hexes
    ) { return new System.IntPtr(); }
};
