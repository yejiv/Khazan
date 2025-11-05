#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_LightViewMatrix, g_LightProjMatrix;
float g_Splits[4];

Texture2D g_DiffuseTexture, g_NormalTexture, g_SpecularTexture;

// Î∞ïÏ???òÅ?ù¥ ?ûÑ?ãúÎ°? Ï∂îÍ???ï¥?Üì?ùå
texture2D g_EmissiveTexture;


bool g_isDiffuse = false;
bool g_isNormal = false;
bool g_isEmissive = false;
bool g_isSpecular = false;


/* ∏µ® ¿¸√º ª¿±‚¡ÿ(x) */
/* ∆Ø¡§ ∏ﬁΩ√ø° øµ«‚§∑∏£ ¡÷¥¬ ª¿µÈ */
matrix g_BoneMatrices[512];

float g_fEmissiveIntensity = 1.f;
bool g_isEnableEmissive, g_isEnableBloom;

// Outline
float g_fOutlineSize = 0.001f;
float3 g_vOutlineColor = { 1.f, 0.f, 1.f };

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    uint4  vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
    //  float4 vPosition : SV_POSITION;
    //  float4 vNormal : NORMAL;
    //  float2 vTexcoord : TEXCOORD0;
    //  float4 vWorldPos : TEXCOORD1;
    //  float4 vProjPos : TEXCOORD2;
};

/* ?†ï?†ê?âê?ù¥?çî : ?†ï?†ê ?úÑÏπòÏùò ?ä§?éò?ù¥?ä§ Î≥??ôò(Î°úÏª¨ -> ?õî?ìú -> Î∑? -> ?à¨?òÅ). */ 
/*          : ?†ï?†ê?ùò Íµ¨ÏÑ±?ùÑ Î≥?Í≤?.(in:3Í∞?, out:2Í∞? or 5Í∞?) */
/*          : ?†ï?†ê ?ã®?úÑ(?†ï?†ê ?ïò?Çò?ãπ VS_MAIN?ïúÎ≤àÌò∏Ï∂?) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    /* ?†ï?†ê?ùò Î°úÏª¨?úÑÏπ? * ?õî?ìú * Î∑? * ?à¨?òÅ */ 
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix =
        g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;    
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
      /* ?†ï?†ê?ùò Î°úÏª¨?úÑÏπ? * ?õî?ìú * Î∑? * ?à¨?òÅ */ 
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix =
        g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_LightViewMatrix);
    matWVP = mul(matWV, g_LightProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct VS_OUT_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float fDepth : TEXCOORD0;
};

VS_OUT_OUTLINE VS_MAIN_OUTLINE(VS_IN In)
{
    VS_OUT_OUTLINE Out;
      /* ?†ï?†ê?ùò Î°úÏª¨?úÑÏπ? * ?õî?ìú * Î∑? * ?à¨?òÅ */ 
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix =
        g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    
    vPosition.xyz += In.vNormal * g_fOutlineSize;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.fDepth = Out.vPosition.z / Out.vPosition.w;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vWorld : SV_TARGET3;
    float4 vSpecular : SV_TARGET4;
    float4 vEmissive : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;

    return Out;
}

PS_OUT PS_MAIN_NONPICK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = vector(0.f, 0.f, 0.f, 0.f);
    return Out;
}

struct PS_IN_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float fDepth : TEXCOORD0;
};

struct PS_OUT_OUTLINE
{
    float4 vOutline : SV_TARGET0;
};

PS_OUT_OUTLINE PS_MAIN_OUTLINE(PS_IN_OUTLINE In)
{
    PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE) 0;
    
    Out.vOutline.rgb = g_vOutlineColor;
    Out.vOutline.a = In.fDepth;
    
    return Out;
}

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    // NonBlend Í∞ùÏ≤¥
    
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    //  Out.vSpecular = vMtrlSpecular;
    
    if (true == g_isEnableEmissive)
        Out.vEmissive.rgb = Out.vDiffuse.rgb * g_fEmissiveIntensity; // Î∞ùÍ∏∞ Í∞ïÎèÑ
    
    if (true == g_isEnableBloom)
        Out.vEmissive.a = 1.f;
    else
        Out.vEmissive.a = 0.f;
    
    return Out;
}

struct PS_OUT_EMISSIVE
{
    float4 vPostScene : SV_TARGET0;
    float4 vEmissive : SV_TARGET1;
};

PS_OUT_EMISSIVE PS_MAIN_DEBUG_EMISSIVE(PS_IN In)
{
    // NonLight, Blend Test
    
    PS_OUT_EMISSIVE Out = (PS_OUT_EMISSIVE) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;

    //  Out.vPostScene = vMtrlDiffuse;
    
    // =============== NonLight ===============
    
    // PostSceneÎß? Í∏∞Î°ù
    //  Out.vPostScene = vMtrlDiffuse;
    //  // (?Éù?ûµ Í∞??ä• -> ?úÑ?óê?Ñú 0 Ï¥àÍ∏∞?ôî)
    //  Out.vEmissive = 0.f; 
    
    // EmissiveÎß? Í∏∞Î°ù
    // (?Éù?ûµ Í∞??ä• -> ?úÑ?óê?Ñú 0 Ï¥àÍ∏∞?ôî)
    //  Out.vPostScene = 0.f;
    //  Out.vPostScene.rgb = vMtrlDiffuse * 3.f; // Intensity
    //  Out.vPostScene.a = 1.f;
    
    Out.vEmissive.rgb = vMtrlDiffuse * 3.f; // Intensity
    Out.vEmissive.a = 1.f;
    
    // ?ëò ?ã§ Í∏∞Î°ù?ïòÍ∏?
    //  Out.vPostScene = vMtrlDiffuse;
    //  Out.vEmissive.rgb = vMtrlDiffuse * 3.f; // Intensity
    //  Out.vEmissive.a = 1.f;
    
    // =============== Blend ===============
    
    // PostSceneÎß? Í∏∞Î°ù
    //  Out.vPostScene = float4(vMtrlDiffuse.rgb, 0.5f);
    //  // (?Éù?ûµ Í∞??ä• -> ?úÑ?óê?Ñú 0 Ï¥àÍ∏∞?ôî)
    //  Out.vEmissive = 0.f; 
    
    //  EmissiveÎß? Í∏∞Î°ù
    //  (?Éù?ûµ Í∞??ä• -> ?úÑ?óê?Ñú 0 Ï¥àÍ∏∞?ôî)
    //  Out.vPostScene = 0.f;
    //  Out.vEmissive.rgb = vMtrlDiffuse * 3.f; // Intensity
    //  Out.vEmissive.a = 1.f;
    
    //  ?ëò ?ã§ Í∏∞Î°ù?ïòÍ∏?
    //  Out.vPostScene = float4(vMtrlDiffuse.rgb, 0.2f);
    //  Out.vEmissive.rgb = vMtrlDiffuse * 3.f; // Intensity
    //  Out.vEmissive.a = 1.f;
    
    return Out;
}

/* ?ï†?ïÑÎ≤ÑÏ???ùò ?ã¨?îåÏª¨Îü¨ Î∑? */
PS_OUT PS_SIMPLE_COLOR_VIEW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = (1.f, 1.f, 1.f, 1.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    
    return Out;
}

PS_OUT PS_BLADENEXUS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (true == g_isNormal)
    {
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
        vMtrlNormal = float4(normalize(vMtrlNormal.xyz) * 2.f - 1.f, 0.f);
    }
    
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (true == g_isEmissive)
    {
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (true == g_isSpecular)
    {
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
        vMtrlSpecular.a = 1.f;
    }

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vMtrlNormal);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    //Out.vSpecular = vMtrlSpecular;
    //Out.vEmissive = vMtrlEmissive;
    Out.vEmissive = vMtrlSpecular;

    return Out;
}

technique11 DefaultTechnique
{
    /* ?äπ?†ï ?å®?ä§Î•? ?ù¥?ö©?ï¥?Ñú ?†ê?†ï?ùÑ Í∑∏Î†§?Éà?ã§. */
    /* ?ïò?Çò?ùò Î™®Îç∏?ùÑ Í∑∏Î†§?Éà?ã§. */ 
    /* Î™®Îç∏?ùò ?ÉÅ?ô©?óê ?î∞?ùº ?ã§Î•? ?âê?ù¥?î© Í∏∞Î≤ï ?Ñ∏?ä∏(Î™ÖÏïî + Î¶ºÎùº?ù¥?ä∏ + ?ä§?éô?Åò?ü¨ + ?Ö∏Î©?Îß? + ssao )Î•? Î®πÏó¨Ï£ºÍ∏∞?úÑ?ï¥?Ñú */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonPick
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONPICK();
    }

    pass Cascade // Depth Only
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = NULL;
    }

    pass Outline
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_DepthTestOnly, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }

    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass DebugNonLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG_EMISSIVE();
    }

    pass DebugBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG_EMISSIVE();
    }

    /* 7Î≤? : ?ï†?ïÑÎ≤ÑÏ???ùò ?ã¨?îåÏª¨Îü¨ Î∑? */
    pass SimpleColorView
    {

        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SIMPLE_COLOR_VIEW();
    }

    // ±Õ∞À ∆–Ω∫        ( 8π¯ )
    pass BladeNexusPass
    {

        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLADENEXUS();
    }

}
