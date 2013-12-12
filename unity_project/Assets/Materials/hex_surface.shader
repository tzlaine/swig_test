// -*- javascript -*-
Shader "Custom/hex_shader"
{
    Properties {
        _primary_color ("Primary Color", Color) = (1,1,1,1)
        _secondary_color ("Secondary Color", Color) = (0,0,0,1)
        _border_thickness ("Border Thickness", Range(0,1)) = 0.1
    }

    SubShader { Pass { CGPROGRAM

        #pragma vertex vert
        #pragma fragment frag
        #include "UnityCG.cginc"

        float4 _primary_color;
        float4 _secondary_color;
        float _border_thickness;

        struct v2f
        {
            float4 position : SV_POSITION;
            float gradient : TEXCOORD4;
        };

        v2f vert (appdata_full v)
        {
            v2f o;
            int vertex_index = v.vertex.z;
            float4 vertex = float4(v.vertex.xy, 0, 1);
            o.position = mul(UNITY_MATRIX_MVP, vertex);
            o.gradient = v.texcoord1.x;
            return o;
        }

        float4 frag (v2f i) : COLOR
        {
            float weight =
                saturate(i.gradient - (1 - _border_thickness)) / _border_thickness;
            weight = pow(weight, 0.75);
            return lerp(_primary_color, _secondary_color, weight);
        }

    ENDCG } }

    FallBack "Diffuse"
}
