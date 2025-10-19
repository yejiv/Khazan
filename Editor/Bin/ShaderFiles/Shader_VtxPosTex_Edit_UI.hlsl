#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vColor, g_vUVMinMax;
float g_fAlpha;
texture2D g_Texture;


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

// Default Pass Start ------------------------------------------------------------------------------------------

struct VS_DEFAULT_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vLocalPos : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};


VS_DEFAULT_OUT VS_MAIN(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;

    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    
    return Out;
}

struct PS_DEFAULT_IN
{
    float4 vPosition : SV_POSITION;
    float4 vLocalPos : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_PANAL(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_vColor;
    
    return Out;
}

PS_OUT PS_ATLAS_TEX(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vMinUV = { g_vUVMinMax.x, g_vUVMinMax.y };
    float2 vMaxUV = { g_vUVMinMax.z, g_vUVMinMax.w };
    
    In.vTexcoord = vMinUV + (vMaxUV - vMinUV) * In.vTexcoord;
     
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * g_fAlpha;
    return Out;
}

PS_OUT PS_TEX_CLOOR(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vMinUV = { g_vUVMinMax.x, g_vUVMinMax.y };
    float2 vMaxUV = { g_vUVMinMax.z, g_vUVMinMax.w };
    
    In.vTexcoord = vMinUV + (vMaxUV - vMinUV) * In.vTexcoord;
     
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.rgb = (1 - Out.vColor.r) * g_vColor.rgb;
    
    Out.vColor.a = Out.vColor.a * g_fAlpha;
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PANAL();
    }

    pass UI_AtlasTex_1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
  
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_ATLAS_TEX();
    }

    pass UI_Tex_2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
  
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_TEX_CLOOR();
    }
}