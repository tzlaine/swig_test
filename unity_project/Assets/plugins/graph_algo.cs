using System;
using System.Runtime.InteropServices;


[StructLayout(LayoutKind.Sequential)]
public struct ga_hex_t
{
    public int a;
    public float b;
}

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
}
