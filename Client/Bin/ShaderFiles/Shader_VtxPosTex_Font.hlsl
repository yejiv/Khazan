#include "Engine_Shader_Defines.hlsli"
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D   g_FontTexture : register(t0);
float       g_fAlpha;

cbuffer CBData : register(b0)
{
    float4x4 g_mWVP;
    float4 g_vColor; 
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vUV : TEXCOORD0;    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;    
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
        
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_mWVP);
    Out.vUV = In.vUV;
  
    return Out;     
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;   
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;    
    float4 tex = g_FontTexture.Sample(DefaultSampler, In.vUV);
    float alpha = tex.r * g_vColor.a;
    float3 color = g_vColor.rgb * alpha;
    
    Out.vColor.rgb = color;
    
    Out.vColor.a = alpha * g_fAlpha;
    return Out;    
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();   
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}
