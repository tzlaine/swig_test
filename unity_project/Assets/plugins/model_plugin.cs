using System;
using System.Runtime.InteropServices;
using Message;


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

    // Used in all the Protobuf blob getters.
    private static int[] size_array = new int[1];
    private static IntPtr[] bytes_array = new IntPtr[1];

/* TODO
    [DllImport("model_plugin")]
    private static extern int get_nations_blob ([In] int[] size, [In] IntPtr[] bytes);
*/

    [DllImport("model_plugin")]
    private static extern int get_nations ([In] IntPtr[] bytes, [In] int[] size);

    public static nations_t get_nations ()
    {
        if (get_nations(bytes_array, size_array) == 0)
            return null;

        byte[] copied_bytes = new byte[size_array[0]];
        System.Runtime.InteropServices.Marshal.Copy(bytes_array[0], copied_bytes, 0, size_array[0]);

/* TODO (for perf, maybe move this function into a DLL compiled with "unsafe" flag?
        System.IO.UnmanagedMemoryStream ums;
        unsafe {
            ums = new System.IO.UnmanagedMemoryStream((Byte*)bytes_array[0], size_array[0]);
        }
*/
        return nations_t.Parser.ParseFrom(copied_bytes);
    }

    [DllImport("model_plugin")]
    private static extern int get_map ([In] IntPtr[] bytes, [In] int[] size);

    [DllImport("model_plugin")]
    private static extern int get_oob ([In] IntPtr[] bytes, [In] int[] size);

    [DllImport("model_plugin")]
    public static extern int init_nations (string nations_str);

    [DllImport("model_plugin")]
    public static extern int init_model (string map_str, string oob_str);

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
