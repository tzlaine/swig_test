// -*- javascript -*-
Shader "Custom/hex_surface"
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
            float falloff : TEXCOORD7;
        };

        v2f vert (appdata_full v)
        {
            v2f o;

            float adjacency = v.vertex.z;
            if (adjacency < 0.5) {
                o.thickness = _border_thickness / 3.0;
                o.falloff = 0.5;
            } else if (1.5 < adjacency) {
                o.thickness = _border_thickness;
                o.falloff = 0.25;
            } else {
                o.thickness = _border_thickness / 1.5;
                o.falloff = 0.25;
            }

            float4 vertex = float4(v.vertex.xy, 0, 1);
            o.position = mul(UNITY_MATRIX_MVP, vertex);
            o.gradient = v.texcoord1.x; // Hex edge gradient.

            // Pass through encoded owner ID, scaled for sampling.
            o.owner = v.texcoord1.y * 4.0;

            return o;
        }

        float4 frag (v2f i) : COLOR
        {
            float4 primary_color = tex2D(_primary_colors, float2(i.owner, 0.5));
            float4 secondary_color = tex2D(_secondary_colors, float2(i.owner, 0.5));
            float weight = saturate(i.gradient - (1 - i.thickness)) / i.thickness;
            weight = pow(weight, i.falloff);
            return lerp(primary_color, secondary_color, weight);
        }

    ENDCG } }

    FallBack "Diffuse"
}
