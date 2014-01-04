// -*- javascript -*-
Shader "Custom/counter"
{
    Properties {
        _alpha_front ("alpha front", 2D) = "white" {}
        _alpha_back ("alpha back", 2D) = "white" {}
        _bravo_front ("bravo front", 2D) = "white" {}
        _bravo_back ("bravo back", 2D) = "white" {}
        _bases_front ("bases front", 2D) = "white" {}
        _bases_back ("bases back", 2D) = "white" {}
        _counters_front ("counters front", 2D) = "white" {}
    }

    SubShader { Pass {

        CGPROGRAM

        #pragma vertex vert
        #pragma fragment frag
        #include "UnityCG.cginc"

        sampler2D _alpha_front;
        sampler2D _alpha_back;
        sampler2D _bravo_front;
        sampler2D _bravo_back;
        sampler2D _bases_front;
        sampler2D _bases_back;
        sampler2D _counters_front;

        struct v2f
        {
            float4 position : SV_POSITION;
            float2 uv : TEXCOORD0;
            float texture_index : TEXCOORD6;
        };

        v2f vert (appdata_full v)
        {
            v2f o;

            o.position = mul(UNITY_MATRIX_MVP, v.vertex);

            o.uv = v.texcoord.xy;
            o.texture_index = v.texcoord1.x;

            return o;
        }

        float4 frag (v2f i) : COLOR
        {
            //return tex2D(_bravo_front, i.uv);
            if (i.texture_index < 0.5) {
                return tex2D(_alpha_front, i.uv);
            } else if (i.texture_index < 1.5) {
                return tex2D(_alpha_back, i.uv);
            } else if (i.texture_index < 2.5) {
                return tex2D(_bravo_front, i.uv);
            } else if (i.texture_index < 3.5) {
                return tex2D(_bravo_back, i.uv);
            } else if (i.texture_index < 4.5) {
                return tex2D(_bases_front, i.uv);
            } else if (i.texture_index < 5.5) {
                return tex2D(_bases_back, i.uv);
            } else { // if (i.texture_index < 6.5)
                return tex2D(_counters_front, i.uv);
            }
        }

        ENDCG

    } }

    FallBack "Diffuse"
}
