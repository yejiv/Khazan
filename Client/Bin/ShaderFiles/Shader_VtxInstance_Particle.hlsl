#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_MaskTexture;

float4 g_vSourceColor = float4(1.f, 1.f, 1.f, 1.f);

float g_RunningTime = 1.f;
float2 g_ScrollSpeed = 0.f;


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : TEXCOORD1;
    float bDead : TEXCOORD2;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
    float bDead : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    float4 vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
    Out.bDead = In.bDead;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
    float bDead : TEXCOORD3;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    
};

PS_OUT PS_MAIN(PS_IN In)
{
    //if (In.bDead == true)
    //    discard;
    
    PS_OUT Out = (PS_OUT) 0;
    
    //float2 fEffectOffset = float2(g_RunningTime * g_ScrollSpeed.x, g_RunningTime * g_ScrollSpeed.y);
    //float2 fScrolledEffectUV = In.vTexcoord + fEffectOffset;
    //
    //vector vEffectTexture = g_DiffuseTexture.Sample(PointSampler, fScrolledEffectUV);
    //
    //vector vFinalColor = float4(g_vSourceColor.xyz, min(vEffectTexture.r, g_vSourceColor.a));
    //
    //vFinalColor.a = 1.f * vEffectTexture.r;
    //
    //float safeProgress = saturate(g_RunningTime); // 0 ~ 1   그니까 0 이면 -2. 1이면 0이 되어야함
    //float maskOffset = (safeProgress * 2.0f) - 2.0f; // -1 ~ 1
    //float2 maskUV = float2(In.vTexcoord.x, In.vTexcoord.y + maskOffset);
    //float maskValue = g_MaskTexture.Sample(ClampSampler, maskUV).r;
    //vFinalColor.a = vFinalColor.a * maskValue;
    //
    //float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    //
    //vFinalColor.a -= fDecreaseAlpha;
    //Out.vColor = vFinalColor;
    
    Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
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
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    //pass AlphaBlend
    //{
    //    SetRasterizerState(RS_Cull_None);
    //    SetDepthStencilState(DSS_Default, 0);
    //    SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    GeometryShader = NULL;
    //    PixelShader = compile ps_5_0 PS_MAIN();
    //}
}
