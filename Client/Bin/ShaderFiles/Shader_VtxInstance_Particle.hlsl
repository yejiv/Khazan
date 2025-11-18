#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vSourceColor = float4(1.f, 1.f, 1.f, 1.f);

float2 g_ScrollSpeed = 0.f; //Texture scroll
bool g_MaskScrollYDir;
bool g_MaskScrollInv;
bool g_IsDissolve = false;
bool g_IsNormal;

float g_MaskScrollSpeed;

float4 g_vCamPosition;

float g_EdgeWidth;
float4 g_EdgeColor;

texture2D g_DiffuseTexture;
texture2D g_MaskTexture;
texture2D g_DissolveTexture;
texture2D g_NormalTexture;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
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

struct VS_NORMAL_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float2 vLifeTime : TEXCOORD3;
    float bDead : TEXCOORD4;
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

VS_NORMAL_OUT VS_NORMAL_MAIN(VS_IN In)
{
    VS_NORMAL_OUT Out = (VS_NORMAL_OUT) 0;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    float4 vNormal = normalize(mul(float4(In.vNormal, 0.f), In.TransformMatrix));
    float4 vTangent = normalize(mul(float4(In.vTangent, 0.f), In.TransformMatrix));
    float4 vBinormal = normalize(mul(float4(In.vBinormal, 0.f), In.TransformMatrix));
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
    Out.vBinormal = normalize(mul(vBinormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vLifeTime = In.vLifeTime;
    Out.bDead = In.bDead;
    Out.vProjPos = Out.vPosition;

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

struct PS_NORMAL_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float2 vLifeTime : TEXCOORD3;
    float bDead : TEXCOORD4;
};

struct PS_NORMAL_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

float Mask_Scrolling(float2 vLifetime, float2 vTexcoord)    //이거 버텍스 셰이더로 넘기면 좀 더 최적화 가능할 듯
{
    float2 maskUV;
    float maskOffset = saturate((vLifetime.x * g_MaskScrollSpeed) / vLifetime.y) - 1.f; // -1 ~ 0
    
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
    
    float noise = g_DissolveTexture.Sample(PointSampler, UV).r;
    
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
    
    vector vEffectTexture = g_DiffuseTexture.Sample(DefaultSampler, fScrolledEffectUV);
    //vector vFinalColor = float4(g_vSourceColor.xyz, min(vEffectTexture.r, g_vSourceColor.a));
    vector vFinalColor = float4(g_vSourceColor.xyz, vEffectTexture.r);
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    if (g_IsDissolve == false) 
        vFinalColor.a -= fDecreaseAlpha;
    else
        vFinalColor = Dissolve(vFinalColor, fDecreaseAlpha, fScrolledEffectUV);
    
    if (g_MaskScrollSpeed)
        vFinalColor.a = vFinalColor.a * Mask_Scrolling(In.vLifeTime, In.vTexcoord);
    
    if (vFinalColor.a <= 0.f)
        discard;

    
    //vFinalColor.a *= 0.2f;
    
    // 0 ~ 1 -> 1 ~  3
    // (g_vSourceColor.a * 2  + 1);
    Out.vColor = vFinalColor * (g_vSourceColor.a + 1);
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
    
    vector vEffectTexture = g_DiffuseTexture.Sample(DefaultSampler, fScrolledEffectUV);
    //vector vFinalColor = float4(g_vSourceColor.xyz, min(vEffectTexture.r, g_vSourceColor.a));
    vector vFinalColor = float4(g_vSourceColor.xyz, vEffectTexture.r);
    
    float fresnelFactor = 1.0 - abs(dot(In.vNormal.xyz, normalize(g_vCamPosition.xyz - In.vWorldPos.xyz)));
    vFinalColor.xyz = vFinalColor.xyz * pow(fresnelFactor, 1.4f);
    
    float fDecreaseAlpha = (In.vLifeTime.x / In.vLifeTime.y);
    if (g_IsDissolve == false) 
        vFinalColor.a -= fDecreaseAlpha; 
    else
        vFinalColor = Dissolve(vFinalColor, fDecreaseAlpha, fScrolledEffectUV);
    
    if (vFinalColor.a <= 0.f)
        discard;

    //Out.vColor = vFinalColor;
    Out.vColor = vFinalColor * (g_vSourceColor.a + 1);

    return Out;
}

PS_NORMAL_OUT PS_NORMAL_MAIN(PS_NORMAL_IN In)
{
    PS_NORMAL_OUT Out = (PS_NORMAL_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSourColor = float4(g_vSourceColor.xyz, max(max(vMtrlDiffuse.r, vMtrlDiffuse.g), vMtrlDiffuse.b));
    vector vFinalColor = vSourColor * vMtrlDiffuse;
    
    //if (vFinalColor.a < 0.01f)
    //    discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
     
    Out.vDiffuse = vFinalColor * (g_vSourceColor.a + 1); 
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    
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

    pass NormalPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_NORMAL_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NORMAL_MAIN();
    }
}
