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
            float4 color : COLOR0;
            float gradient : TEXCOORD4;
        };

        v2f vert (appdata_base v)
        {
            v2f o;
            o.position = mul(UNITY_MATRIX_MVP, v.vertex);
            o.color = _primary_color;
            o.gradient = v.vertex.y;//(v.vertex.x == 0 && v.vertex.y == 0) ? 0 : 1; // TODO
            return o;
        }

        float4 frag (v2f i) : COLOR
        {
            float weight =
                clamp(i.gradient - (1 - _border_thickness), 0, 1) / _border_thickness;
            return lerp(_primary_color, _secondary_color, weight);
        }

    ENDCG } }

    FallBack "Diffuse"
}
