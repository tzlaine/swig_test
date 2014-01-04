// -*- javascript -*-
Shader "Custom/counter_old"
{
    Properties {
        _counters ("Colors", 2D) = "white" {}
        _uvs ("UVs", Vector) = (0, 0, 1, 1)
    }

    SubShader { Pass {

        CGPROGRAM

        #pragma vertex vert
        #pragma fragment frag
        #include "UnityCG.cginc"

        sampler2D _counters;
        float4 _uvs;

        struct v2f
        {
            float4 position : SV_POSITION;
            float2 uv : TEXCOORD5;
        };

        v2f vert (appdata_full v)
        {
            v2f o;

            float4 vertex = float4(v.vertex.xy, 0, 1);
            o.position = mul(UNITY_MATRIX_MVP, vertex);

            o.uv = _uvs.xy + (_uvs.zw - _uvs.xy) * v.texcoord;
            o.uv = float2(1.0, 1.0) - o.uv;

            return o;
        }

        float4 frag (v2f i) : COLOR
        {
            return tex2D(_counters, i.uv);
        }

        ENDCG

    } }

    FallBack "Diffuse"
}
