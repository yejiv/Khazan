#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vSourceColor = float4(1.f, 1.f, 1.f, 1.f);

float2 g_ScrollSpeed = 0.f; //Texture scroll
bool g_MaskScrollYDir;
bool g_MaskScrollInv;
bool g_IsDisolve = false;

float g_MaskScrollSpeed;

float4 g_vCamPosition;

float g_EdgeWidth;
float4 g_EdgeColor;

texture2D g_DiffuseTexture;
texture2D g_MaskTexture;
texture2D g_DisolveTexture;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    row_major float4x4 TransformMatrix : WORLD;
    
    float2 vLifeTime : TEXCOORD1;
    float bDead : TEXCOORD2;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
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
    float4 vNormal = normalize(mul(float4(In.vNormal, 0.f), In.TransformMatrix));
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
    Out.bDead = In.bDead;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
    float bDead : TEXCOORD3;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vEmissiveColor : SV_TARGET1; 
};

float Mask_Scrolling(float2 vLifetime, float2 vTexcoord)
{
    float safeProgress = saturate(vLifetime.x * g_MaskScrollSpeed / vLifetime.y) * 2;
    float maskOffset = (safeProgress * 2.0f) - 2.0f;
    float2 maskUV;
    
    if (g_MaskScrollYDir)
    {
        if (g_MaskScrollInv)
            maskUV = float2(vTexcoord.x, vTexcoord.y - maskOffset);
        else
            maskUV = float2(vTexcoord.x, vTexcoord.y + maskOffset);
    }
    else
    {
        if (g_MaskScrollInv)
            maskUV = float2(vTexcoord.x - maskOffset, vTexcoord.y);
        else
            maskUV = float2(vTexcoord.x + maskOffset, vTexcoord.y);
    }

    float maskValue = g_MaskTexture.Sample(ClampSampler, maskUV).r;
    
    return maskValue;
}

float4 Dissolve(float4 InColor, float fDecreaseAlpha, float2 UV)
{
    float4 rt = InColor;
    
    float noise = g_DisolveTexture.Sample(PointSampler, UV).r;
    
    if (noise < fDecreaseAlpha)
    {
        rt.a = 0.f;
        return rt;
    }
    
    float edgeStart = fDecreaseAlpha;
    float edgeEnd = fDecreaseAlpha + g_EdgeWidth;
    float edgeFactor = smoothstep(edgeStart, edgeEnd, noise);
    rt = lerp(g_EdgeColor * 2.f, InColor, edgeFactor);
    
    return rt; 
}

PS_OUT PS_MAIN(PS_IN In)
{
    //if (In.bDead == true)
    //    discard;
    
    PS_OUT Out = (PS_OUT) 0;
    
    float2 fEffectOffset = float2(In.vLifeTime.x * g_ScrollSpeed.x, In.vLifeTime.x * g_ScrollSpeed.y);
    float2 fScrolledEffectUV = In.vTexcoord + fEffectOffset;
    
    vector vEffectTexture = g_DiffuseTexture.Sample(PointSampler, fScrolledEffectUV);
    vector vFinalColor = float4(g_vSourceColor.xyz, min(vEffectTexture.r, g_vSourceColor.a));
    
    if (g_MaskScrollSpeed)
        vFinalColor.a = vFinalColor.a * Mask_Scrolling(In.vLifeTime, In.vTexcoord);
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    if (g_IsDisolve == false) 
        vFinalColor.a -= fDecreaseAlpha;
    else
        vFinalColor = Dissolve(vFinalColor, fDecreaseAlpha, fScrolledEffectUV);
    
    if (vFinalColor.a <= 0.f)
        discard;

    Out.vColor = vFinalColor;
    //Out.vEmissiveColor = vFinalColor * 3.f;
    //Out.vEmissiveColor.a = 1;
    
    return Out;
}

PS_OUT PS_PRESNEL(PS_IN In)
{
    //if (In.bDead == true)
    //    discard;
    
    PS_OUT Out = (PS_OUT) 0;
    
    float2 fEffectOffset = float2(In.vLifeTime.x * g_ScrollSpeed.x, In.vLifeTime.x * g_ScrollSpeed.y);
    float2 fScrolledEffectUV = In.vTexcoord + fEffectOffset;
    
    vector vEffectTexture = g_DiffuseTexture.Sample(PointSampler, fScrolledEffectUV);
    vector vFinalColor = float4(g_vSourceColor.xyz, min(vEffectTexture.r, g_vSourceColor.a));
    
    
    float fresnelFactor = 1.0 - abs(dot(In.vNormal.xyz, normalize(g_vCamPosition - In.vWorldPos)));
    vFinalColor.xyz = vFinalColor.xyz * pow(fresnelFactor, 1.4f);
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    if (g_IsDisolve == false) 
        vFinalColor.a -= fDecreaseAlpha; 
    else
        vFinalColor = Dissolve(vFinalColor, fDecreaseAlpha, fScrolledEffectUV);
    
    if (vFinalColor.a <= 0.f)
        discard;

    Out.vColor = vFinalColor;
    
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

    pass FresnelPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PRESNEL();
    }
}
