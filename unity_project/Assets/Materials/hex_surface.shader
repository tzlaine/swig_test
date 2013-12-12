// -*- javascript -*-
Shader "Custom/hex_shader"
{
    Properties {
        _border_thickness ("Border Thickness", Range(0,1)) = 0.1
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
            float2 gradient_owner : TEXCOORD4;
        };

        v2f vert (appdata_full v)
        {
            v2f o;

            int vertex_index = v.vertex.z;

            float4 vertex = float4(v.vertex.xy, 0, 1);
            o.position = mul(UNITY_MATRIX_MVP, vertex);
            o.gradient_owner.x = v.texcoord1.x; // Hex edge gradient.

            // Pass through encoded owner ID, scaled for sampling.
            o.gradient_owner.y = v.color.r * 4.0;

            return o;
        }

        float4 frag (v2f i) : COLOR
        {
            float4 primary_color =
                tex2D(_primary_colors, float2(i.gradient_owner.y, 0.5));
            float4 secondary_color =
                tex2D(_secondary_colors, float2(i.gradient_owner.y, 0.5));
            float weight =
                saturate(i.gradient_owner.x - (1 - _border_thickness)) / _border_thickness;
            weight = pow(weight, 0.75);
            return lerp(primary_color, secondary_color, weight);
        }

    ENDCG } }

    FallBack "Diffuse"
}
