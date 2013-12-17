// -*- javascript -*-
Shader "Custom/hex_surface"
{
    Properties {
        _primary_colors ("Primary Colors", 2D) = "white" {}
    }

    SubShader { Pass { CGPROGRAM

        #pragma vertex vert
        #pragma fragment frag
        #include "UnityCG.cginc"

        sampler2D _primary_colors;

        struct v2f
        {
            float4 position : SV_POSITION;
            float owner : TEXCOORD5;
        };

        v2f vert (appdata_full v)
        {
            v2f o;

            o.position = mul(UNITY_MATRIX_MVP, v.vertex);

            // Pass through encoded owner ID, scaled for sampling.
            o.owner = v.texcoord1.x * 4.0;

            return o;
        }

        float4 frag (v2f i) : COLOR
        {
            float4 primary_color = tex2D(_primary_colors, float2(i.owner, 0.5));
            return primary_color;
        }

    ENDCG } }

    FallBack "Diffuse"
}
