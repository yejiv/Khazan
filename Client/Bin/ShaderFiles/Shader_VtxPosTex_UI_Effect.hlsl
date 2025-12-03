#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vColor, g_fProgressValue;
float g_fAlpha;
float g_fValue;

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

struct PS_OUT_EMISSIVE
{
    float4 vPostScene : SV_TARGET0;
    float4 vEmissive : SV_TARGET1; 
};

PS_OUT_EMISSIVE PS_ROCKON(PS_IN In)
{
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;

    float4 vMaskColor = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vPostScene = g_Texture.Sample(DefaultSampler, In.vTexcoord);

    Out.vPostScene.a = vMaskColor.r;
    
    Out.vPostScene.rgb = Out.vPostScene.rgb;
    
    if (Out.vPostScene.a >= 0.9f)
    {
        Out.vEmissive.rgb = Out.vPostScene.rgb * 1.5f;
        Out.vEmissive.a = 1.f;
    }
    else
        Out.vEmissive.a = 0.f;

    return Out;
}

PS_OUT_EMISSIVE PS_BRUTALBG(PS_IN In)
{
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;

    Out.vPostScene = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vPostScene.rgb = 1.f - Out.vPostScene.rgb;
    if (Out.vPostScene.a >= 0.1f)
    {
        Out.vPostScene.rgb = 0.f;
        Out.vPostScene.a = 1.f - Out.vPostScene.a + 0.5f;
        Out.vPostScene.a = clamp(Out.vPostScene.a, 0.5f, 1.f);
    }
    
    Out.vPostScene.a = Out.vPostScene.a * g_vColor.a;
    return Out;
}

PS_OUT_EMISSIVE PS_BRUTAL_PROGRESS(PS_IN In)
{
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;
    
    Out.vPostScene = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    if (Out.vPostScene.a < 0.2f)
        discard;
    
    float2 vCenter = { 0.5f, 0.5f };
    float2 vPos = In.vTexcoord - vCenter;

    float fAngle = atan2(vPos.y, vPos.x);

    float fTemp = (fAngle + PI) / (2.0 * PI);
    fTemp = frac(fTemp - 0.25);
    Out.vPostScene.a = Out.vPostScene.r;
    if (fTemp > g_fProgressValue.x)
    {
        Out.vPostScene.rgb = Out.vPostScene.r * 0.5f;
        Out.vPostScene.gb = Out.vPostScene.r * 0.1f;
  
    }
    else
    {
        Out.vPostScene.gb = Out.vPostScene.r * 0.1f;      
    }

    return Out;
}

PS_OUT_EMISSIVE PS_BRUTALPoint(PS_IN In)
{
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;

    Out.vPostScene = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vPostScene.a = Out.vPostScene.r;
    Out.vPostScene.a = Out.vPostScene.a;
    
    if (Out.vEmissive.r > 0.9f)
    {
        Out.vEmissive.rgb = Out.vPostScene.rgb;
        Out.vEmissive = 1.f;
    }
    else
        Out.vEmissive = 0.f;
 
    return Out;
 
}
PS_OUT_EMISSIVE PS_BRUTALPointBG(PS_IN In)
{
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;

    Out.vPostScene = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vPostScene.rgb = Out.vPostScene.r * g_vColor.rgb;
    Out.vPostScene.a = Out.vPostScene.a * g_vColor.a;
    return Out;
}

PS_OUT PS_SLOT_SMOKE(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;

    float2 nUV = uv + float2(g_fValue * 0.1f, -g_fValue * 0.1f);
    float2 noise = g_Texture.Sample(DefaultSampler, nUV).rg;

    // -0.5 ~ +0.5
    float2 n = noise - 0.5f;

    // 기본 왜곡
    float2 distort = n * 0.01f;

    float waveU = sin(uv.y * 10 + g_fValue * 3.0f);
    float waveV = cos(uv.x * 12 + g_fValue * 2.5f);

    distort.x += waveU * 0.01f;
    distort.y += waveV * 0.01f;

    float2 finalUV = uv + distort;
    finalUV = saturate(finalUV);

    float4 tex = g_Texture.Sample(DefaultSampler, finalUV);

    // 밝기 계산 (0~1)
    float luminance = dot(tex.rgb, float3(0.299f, 0.587f, 0.114f));


    Out.vColor.rgb = g_vColor; // 네가 지정한 단일 색
    Out.vColor.a = luminance * g_vColor.a * g_fAlpha; // 텍스처의 밝기를 알파로

    return Out;
}

PS_OUT PS_SLOT_SKILL_FX(PS_IN In)
{
    PS_OUT Out;

    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.r * g_vColor.a * g_fAlpha; // 텍스처의 밝기를 알파로

    return Out;
}

PS_OUT PS_CURSOR_FX(PS_IN In)
{
    PS_OUT Out;

    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.a <= 0.2f)
        discard;
    
    Out.vColor.rgb = g_vColor.rgb;
    Out.vColor.a = Out.vColor.a * g_vColor.a * g_fAlpha; // 텍스처의 밝기를 알파로
    
    return Out;
}


PS_OUT PS_SKILL_BG_SMOKE(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;
    float speed = 0.03f;

    uv.x = frac(uv.x - g_fValue * speed);
    uv.y += sin(uv.x * 3.0 + g_fValue * 1.5f) * 0.01f;

    Out.vColor = g_Texture.Sample(DefaultSampler, uv);

    Out.vColor.a = Out.vColor.a * g_vColor.a * g_fAlpha; // 텍스처의 밝기를 알파로
    
    return Out;
}


PS_OUT PS_SKILL_SELETE(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;

    uv.y = frac(uv.y + g_fValue * 0.25f);
    uv.x += sin(uv.y * 8.0 + g_fValue * 2.0) * 0.02;
    float alpha = g_Texture.Sample(DefaultSampler, uv).r;

    float2 center = float2(0.5f, 0.5f);
    float dist = distance(In.vTexcoord, center);
    float radius = 0.45f;
    float soft = 0.05f;
    float circleMask = smoothstep(radius, radius - soft, dist);
    float edgeFade = smoothstep(0.0f, radius, radius - dist);

    alpha *= circleMask;
    alpha *= edgeFade;

    Out.vColor.rgb = g_vColor.rgb;
    Out.vColor.a = alpha * 1.7f * g_vColor.a * g_fAlpha;
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
        PixelShader = compile ps_5_0 PS_ROCKON();
    }

    pass PS_BRUTALBG
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRUTALBG();
    }

    pass PS_BRUTAL_PROGRESS //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRUTAL_PROGRESS();
    }

    pass PS_BRUTALPoint //3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRUTALPoint();
    }

    pass PS_BRUTALPointBG //4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRUTALPointBG();
    }

    pass PS_SLOT_SMOKE //5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLOT_SMOKE();
    }

    pass PS_SLOT_SKILL_FX //6
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLOT_SKILL_FX();
    }

    pass PS_CURSOR_FX //7
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CURSOR_FX();
    }
    pass PS_SKILL_BG_SMOKE //8
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKILL_BG_SMOKE();
    }
    pass PS_SKILL_SELETE //9
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKILL_SELETE();
    }


}