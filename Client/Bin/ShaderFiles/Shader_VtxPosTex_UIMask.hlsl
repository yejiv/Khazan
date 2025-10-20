#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vColor, g_fProgressValue;
float g_fAlpha;
texture2D g_Texture;
texture2D g_MaskTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vLocalPos : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vLocalPos : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MASK_0(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vMaskTex = g_MaskTexture.Sample(ClampSampler, In.vTexcoord);
    
    if(vMaskTex.a < 0.3f)
        discard;
    
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    Out.vColor.a = 1.f - vMaskTex.r;
    return Out;
}

technique11 DefaultTechnique
{
    pass PS_MASK_PASS
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK_0();
    }

}