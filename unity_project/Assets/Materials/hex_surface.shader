// -*- javascript -*-
Shader "Custom/hex_shader"
{
    Properties {
        _border_thickness ("Border Thickness", float) = 0.1
        _primary_colors ("Primary Colors", 2D) = "white" {}
        _secondary_colors ("Secondary Colors", 2D) = "white" {}
    }

    SubShader { Pass { CGPROGRAM

        #pragma vertex vert
        #pragma fragment frag
        #include "UnityCG.cginc"

        float _border_thickness;
        sampler2D _primary_colors;
        sampler2D _secondary_colors;

        struct v2f
        {
            float4 position : SV_POSITION;
            float gradient : TEXCOORD4;
            float owner : TEXCOORD5;
            float thickness : TEXCOORD6;
        };

        v2f vert (appdata_full v)
        {
            v2f o;

            int edge_index = v.vertex.z;
            float adjacency = 2.0;
            if (edge_index == 0)
                adjacency = v.color.g * 2;
            else if (edge_index == 1)
                adjacency = v.color.b * 2;
            else if (edge_index == 2)
                adjacency = v.color.a * 2;
            else if (edge_index == 3)
                adjacency = v.tangent.x * 2;
            else if (edge_index == 4)
                adjacency = v.tangent.y * 2;
            else if (edge_index == 5)
                adjacency = v.tangent.z * 2;
  
            if (adjacency < 0.5)
                o.thickness = _border_thickness / 3.0;
            else if (1.5 < adjacency)
                o.thickness = _border_thickness * 1.5;
            else
                o.thickness = _border_thickness;

            float4 vertex = float4(v.vertex.xy, 0, 1);
            o.position = mul(UNITY_MATRIX_MVP, vertex);
            o.gradient = v.texcoord1.x; // Hex edge gradient.

            // Pass through encoded owner ID, scaled for sampling.
            o.owner = v.color.r * 4.0;

            return o;
        }

        float4 frag (v2f i) : COLOR
        {
            float4 primary_color = tex2D(_primary_colors, float2(i.owner, 0.5));
            float4 secondary_color = tex2D(_secondary_colors, float2(i.owner, 0.5));
            float weight = saturate(i.gradient - (1 - i.thickness)) / i.thickness;
            weight = pow(weight, 0.75);
            return lerp(primary_color, secondary_color, weight);
        }

    ENDCG } }

    FallBack "Diffuse"
}
