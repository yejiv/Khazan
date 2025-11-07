#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vColor, g_fProgressValue;
float g_fAlpha;
float g_fValue;
float g_fDissovle;

texture2D g_Texture;
texture2D g_MaskTexture;

#define PI       3.14159265358979323846
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
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.rgb = (Out.vColor.r) * g_vColor.rgb;
    
    Out.vColor.a = Out.vColor.a * g_vColor.a * g_fAlpha;
    return Out;
}

PS_OUT PS_TEX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
         
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * g_vColor.a * g_fAlpha;
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
    
    float fAlpha = 1.f;
    
    if (In.vTexcoord.x > g_fValue + 0.3)
        discard;
    else if (In.vTexcoord.x > g_fValue)
    {
        fAlpha = (In.vTexcoord.x - g_fValue) / 0.3f;
        fAlpha = saturate(1.0f - fAlpha);
    }
    Out.vColor.a = Out.vColor.a * g_fAlpha * fAlpha;
    return Out;
}

PS_OUT PS_TEX_PROGRESS_LEFTDOWN_GAUGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x > g_fProgressValue.x)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.a = Out.vColor.a * g_fAlpha * 0.5f;
    }
    else
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
  
    return Out;
}

PS_OUT PS_TEX_PROGRESS_LEFTDOWN_GAUGE_TIP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    if (In.vTexcoord.x > g_fProgressValue.x)
    {
        Out.vColor.a = Out.vColor.a * g_fAlpha * 0.5f;
    }
    else if (In.vTexcoord.x > g_fProgressValue.x - 0.1f && g_fProgressValue.x < 1.f)
    {
        float TipStart = g_fProgressValue.x - 0.1f;
       
        float fDelta = saturate((In.vTexcoord.x - TipStart) / (g_fProgressValue.x - TipStart));
        Out.vColor.rgb += 0.5f * fDelta;
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
    else
    {
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
  
    return Out;
}

PS_OUT PS_TEX_PROGRESS_CIRCLE_GAUGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    if(Out.vColor.a < 0.2f)
        discard;
    
    float2 vCenter = { 0.5f, 0.5f };
    float2 vPos = In.vTexcoord - vCenter;

    float fAngle = atan2(vPos.y, vPos.x);

    float fTemp = (fAngle + PI) / (2.0 * PI);
    fTemp = frac(fTemp - 0.25);
    Out.vColor.a = Out.vColor.r;
    if (fTemp > g_fProgressValue.x)
    {
        Out.vColor.rgb = Out.vColor.r * 0.5f;
  
    }
  
    return Out;
}

PS_OUT PS_GET_ITEMINFO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vMaskColor = g_MaskTexture.Sample(ClampSampler, In.vTexcoord);
    
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    

    Out.vColor.rgb = (vMaskColor.r * Out.vColor.rgb) * (In.vTexcoord.x * 0.8f);

    Out.vColor.a = Out.vColor.a * g_vColor.a * (vMaskColor.a * 30.f) * g_fAlpha;
    

    return Out;
}

PS_OUT PS_TEX_PROGRESS_BossHP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    if (In.vTexcoord.x > g_fProgressValue.y)
    {
        Out.vColor.a = Out.vColor.a * g_fAlpha * 0.5f;
    }
    else if (In.vTexcoord.x < g_fProgressValue.y && In.vTexcoord.x > g_fProgressValue.x)
    {
        Out.vColor.rgb = Out.vColor.r + (g_vColor.rgb * 0.3f);
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
    else if (In.vTexcoord.x > g_fProgressValue.x - 0.05f && g_fProgressValue.x < 1.f)
    {
        float TipStart = g_fProgressValue.x - 0.05f;
       
        float fDelta = saturate((In.vTexcoord.x - TipStart) / (g_fProgressValue.x - TipStart));
        if (fDelta > 0.97f)
            Out.vColor.rgb = 1.f;
        else
            Out.vColor.rgb += 0.5f * fDelta;
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
    else
    {
        Out.vColor.a = Out.vColor.a * g_fAlpha;
    }
    return Out;
}

PS_OUT PS_HUD_AMOUNT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    if (In.vTexcoord.x < 0.7f)
    {
        float fAlpha = In.vTexcoord.x / 0.7f;
        fAlpha = clamp(fAlpha, 0.0f, 1.0f);
        
        Out.vColor.a = fAlpha;
        Out.vColor.a *= g_vColor.a;
        Out.vColor.a *= g_fAlpha;
    }
    else
    {
        Out.vColor.a *= g_fAlpha;
    }

    return Out;
}

PS_OUT PS_MASK_DISSOVLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 DissoveleColor = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);
    if (DissoveleColor.r > g_fDissovle)
        discard;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * g_vColor.a * g_fAlpha;

    return Out;
}

PS_OUT PS_MASK_Announce_Talk(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
        
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor.a = Out.vColor.a * g_vColor.a * g_fAlpha;
    Out.vColor.rgb = g_vColor.rgb;
    
    return Out;
}

PS_OUT PS_TUTORIAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
         
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * g_vColor.a * g_fAlpha;
    
    if(In.vTexcoord.y > 0.84)
        Out.vColor.rgb *= 1.5f;
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
        PixelShader = compile ps_5_0 PS_MAINMENU_LIST();
    }

    pass PS_TEX_PROGRESS_LEFTDOWN_GAUGE //7
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_PROGRESS_LEFTDOWN_GAUGE();
    }

    pass PS_TEX_PROGRESS_LEFTDOWN_GAUGE_TIP //8
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_PROGRESS_LEFTDOWN_GAUGE_TIP();
    }

    pass PS_TEX_PROGRESS_CIRCLE_GAUGE //9
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_PROGRESS_CIRCLE_GAUGE();
    }

    pass PS_GET_ITEMINFO //10
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GET_ITEMINFO();
    }
    pass PS_TEX_PROGRESS_BossHP //11
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEX_PROGRESS_BossHP();
    }
    pass PS_HUD_AMOUNT //12
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HUD_AMOUNT();
    }
    pass PS_MASK_DISSOVLE //13
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK_DISSOVLE();
    }

    pass PS_MASK_Announce_Talk //14
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK_Announce_Talk();
    }

    pass PS_TUTORIAL //15
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TUTORIAL();
    }

}