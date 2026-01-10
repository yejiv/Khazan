#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_FontTexture : register(t0);
Texture2D g_MaskTexture : register(t1);;
Texture2D g_DissovleTexture : register(t2);;

float g_fAlpha;
float g_fTexAspect;

float g_fDissovle;
float g_fDissovleTexAspect;

float4 g_ShadowColor = { 0.f, 0.f, 0.f, 0.6f };
float2 g_ShadowOffset = { 1.f / 1920.f, 1.f / 1080.f };

cbuffer CBData : register(b0)
{
    float4x4 g_mWVP;
    float4 g_vColor;
    float4 g_TextRect;
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
    VS_OUT Out = (VS_OUT) 0;
        
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_mWVP);
    Out.vUV = In.vUV;
  
    return Out;
}

VS_OUT VS_WORLD(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
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
   
    float4 shadowTex = g_FontTexture.Sample(DefaultSampler, In.vUV + g_ShadowOffset);
    float shadowAlpha = shadowTex.r * g_ShadowColor.a;
    float3 shadowColor = g_ShadowColor.rgb * shadowAlpha;
    
    float4 tex = g_FontTexture.Sample(DefaultSampler, In.vUV);
    float alpha = tex.r * g_vColor.a;
    float3 color = g_vColor.rgb * alpha;
    
    float3 finalColor = shadowColor + color;
    
    Out.vColor.rgb = finalColor;
    Out.vColor.a = alpha;
    
    return Out;
}

PS_OUT PS_MAP_MASK_PASS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float mask = g_FontTexture.Sample(DefaultSampler, In.vUV).r;
    float2 uvFill = (In.vPosition.xy - g_TextRect.xy) / g_TextRect.zw;

    // clamp / frac 可记
    // uvFill = saturate(uvFill);   // 裹困 观=0/1
    // uvFill = frac(uvFill);       // tile

    float rectAspect = g_TextRect.z / g_TextRect.w;
    
    if (rectAspect > g_fTexAspect)
    {
        float scale = g_fTexAspect / rectAspect;
        uvFill.x = uvFill.x * scale + (1 - scale) * 0.5;
    }
    else
    {
        float scale = rectAspect / g_fTexAspect;
        uvFill.y = uvFill.y * scale + (1 - scale) * 0.5;
    }

    float3 fillRGB = g_MaskTexture.Sample(DefaultSampler, uvFill).rgb;

    if (fillRGB.r < 0.99f)
        fillRGB *= 0.95f;

    Out.vColor.rgb = fillRGB * g_vColor.rgb * mask;
    Out.vColor.a = mask * g_vColor.a * g_fAlpha;

    return Out;
}

PS_OUT PS_MAP_DISSOLVE_PASS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float mask = g_FontTexture.Sample(DefaultSampler, In.vUV).r;
    float2 uvFill = (In.vPosition.xy - g_TextRect.xy) / g_TextRect.zw;
    float2 DissoveleuvFill = (In.vPosition.xy - g_TextRect.xy) / g_TextRect.zw;

    float rectAspect = g_TextRect.z / g_TextRect.w;

    //叼凉宏 贸府
    if (rectAspect > g_fDissovleTexAspect)
    {
        float scale = g_fDissovleTexAspect / rectAspect;
        DissoveleuvFill.x = DissoveleuvFill.x * scale + (1 - scale) * 0.5;
    }
    else
    {
        float scale = rectAspect / g_fDissovleTexAspect;
        DissoveleuvFill.y = DissoveleuvFill.y * scale + (1 - scale) * 0.5;
    }
    
    float3 DissoveleRGB = g_DissovleTexture.Sample(DefaultSampler, uvFill).rgb;

    if (DissoveleRGB.r > g_fDissovle)
        discard;
    
    //咆胶贸 贸府
    if (rectAspect > g_fTexAspect)
    {
        float scale = g_fTexAspect / rectAspect;
        uvFill.x = uvFill.x * scale + (1 - scale) * 0.5;
    }
    else
    {
        float scale = rectAspect / g_fTexAspect;
        uvFill.y = uvFill.y * scale + (1 - scale) * 0.5;
    }

    float3 fillRGB = g_MaskTexture.Sample(DefaultSampler, uvFill).rgb;

    if (fillRGB.r < 0.99f)
        fillRGB *= 0.95f;

    Out.vColor.rgb = fillRGB * g_vColor.rgb * mask;
    Out.vColor.a = mask * g_vColor.a * g_fAlpha;

    return Out;
}

PS_OUT PS_WORLD_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    float4 shadowTex = g_FontTexture.Sample(DefaultSampler, In.vUV + g_ShadowOffset);
    float shadowAlpha = shadowTex.r * g_ShadowColor.a;
    float3 shadowColor = g_ShadowColor.rgb * shadowAlpha;
    
    float4 tex = g_FontTexture.Sample(DefaultSampler, In.vUV);
    float alpha = tex.r * g_vColor.a;
    float3 color = g_vColor.rgb * alpha;
    
    float3 finalColor = shadowColor + color;
    
    Out.vColor.rgb = finalColor;
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

    pass PS_MAP_MASK_PASS
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP_MASK_PASS();
    }

    pass PS_MAP_DISSOLVE_PASS
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP_DISSOLVE_PASS();
    }

    pass PS_WORLD_DEFAULT_PASS //3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_WORLD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WORLD_MAIN();
    }

}
