#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vColor, g_fProgressValue;
float g_fAlpha;
texture2D g_Texture;

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


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor = 1.f;
    return Out;
}

PS_OUT PS_TEX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
         
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * g_fAlpha;
    return Out;
}

PS_OUT PS_TEX_CLOOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
         
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.rgb = (1 - Out.vColor.r) * g_vColor.rgb;
    
    Out.vColor.a = Out.vColor.a * g_fAlpha;
    return Out;
}

PS_OUT PS_TEX_PROGRESS_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x > g_fProgressValue.y)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.a = Out.vColor.a * g_fAlpha * 0.5f;
    }
    else if (In.vTexcoord.x < g_fProgressValue.y && In.vTexcoord.x > g_fProgressValue.x)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.rgb = Out.vColor.r + (g_vColor.rgb * 0.3f);
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
    else
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
    return Out;
}

PS_OUT PS_TEX_PROGRESS_LEFTDOWN_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x > g_fProgressValue.x)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.rgb = (1 - Out.vColor.r) * g_vColor.rgb;
        Out.vColor.a = Out.vColor.a * g_fAlpha * 0.5f;
    }
    else
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.rgb = (1 - Out.vColor.r) * g_vColor.rgb;
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
  
    return Out;
}

PS_OUT PS_TEX_PROGRESS_BOTTOMDOWN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (g_fProgressValue.x == 1.f)
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);       
    else if (In.vTexcoord.y < 1.f - g_fProgressValue.x)
        discard;
    else
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.rgb *= 0.5f;
    }
    Out.vColor.a = Out.vColor.a * g_fAlpha;
    return Out;
}

PS_OUT PS_MAINMENU_LIST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
         
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * g_fAlpha;
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass HP_PASS
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX();
    }

    pass COMBATSPIRIT_PASS
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_CLOOR();
    }

    pass PS_PROGRESS_PASS_LEFTDOWN //3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_PROGRESS_HP();
    }

    pass PS_PROGRESS_PASS_LEFTDOWN_COLOR //4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_PROGRESS_LEFTDOWN_COLOR();
    }

    pass PS_PROGRESS_PASS_BOTTOMDOWN//5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_PROGRESS_BOTTOMDOWN();
    }

    pass PS_MAINMENU_LIST //6
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_PROGRESS_BOTTOMDOWN();
    }
}