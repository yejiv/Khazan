#include "Engine_Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_Texture;
float4      g_vColor;

float       g_numCols, g_numRows;
float       g_FrameIdx;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
            
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);    
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    float Width = 1.0f / g_numCols;
    float Height = 1.0f / g_numRows;
    
    float startU = (g_FrameIdx % g_numCols) * Width;
    float startV = floor(g_FrameIdx / g_numCols) * Height;
    float2 finalUV = float2(startU, startV) + (In.vTexcoord * float2(Width, Height));
    Out.vTexcoord = finalUV;
    
    Out.vTexcoord = In.vTexcoord;
    
    return Out;     
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    
};

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    //Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    vector vMask = g_Texture.Sample(PointSampler, In.vTexcoord);
    vector vFinalColor = float4(g_vColor.xyz, min(vMask.r, g_vColor.a));

    float vDestAlpha = max(max(vMask.r, vMask.g), vMask.b);
    
    vFinalColor.a = 1.f * vDestAlpha;
    
    Out.vColor = vFinalColor;
    
   // Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLEND();
    }
}
