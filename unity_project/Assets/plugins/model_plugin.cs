using System;
using System.Runtime.InteropServices;


[StructLayout(LayoutKind.Sequential)]
public struct ga_hex_t
{
    public int a;
    public float b;
};

/*
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
*/

[StructLayout(LayoutKind.Sequential)]
public struct nations_t
{
     // TODO
};

public class model_plugin
{
    [DllImport("model_plugin")]
    public static extern int test (int i);

    public static float test_2 (float[] floats)
    {
        return test_2(floats.Length, floats);
    }

    [DllImport("model_plugin")]
    private static extern float test_2 (
        int n,
        [In] float[] floats
    );

    public static float test_3 (ga_hex_t[] hexes)
    {
        return test_3(hexes.Length, hexes);
    }

    [DllImport("model_plugin")]
    private static extern float test_3 (
        int n,
        [In] ga_hex_t[] hexes
    );

/* TODO
    [DllImport("model_plugin")]
    private static extern int get_nations_blob ([In] uint[] size, [In] IntPtr[] bytes);
*/

    [DllImport("model_plugin")]
    private static extern int init_model (string nations_str, string map_str, string oob_str);

/*
    public const int max_offmap_border_hexes = 13;

    [DllImport("model_plugin")]
    public static extern IntPtr determine_supply (
        int w, int h,
        [In] supply_check_hex_t[] hexes,
        int neutral_zone_id,
        int nations,
        [In] int[] nation_team_membership,
        [In] int[] capitols,
        int max_offmap_border_hexes,
        [In] int[] offmap_border_hexes
    );
*/

};
