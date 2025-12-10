#include "Engine_Shader_Defines.hlsli"

unsigned int g_MtrlFlags = { 0 };

bool IsFlag(unsigned int iMask)
{
    return (g_MtrlFlags & iMask) != 0;
}

static const unsigned int M_DIFFUSE       = (1 << 0);
static const unsigned int M_NORMAL        = (1 << 1);
static const unsigned int M_EMISSIVE      = (1 << 2);
static const unsigned int M_SPECULAR      = (1 << 3);
static const unsigned int M_METALIC       = (1 << 4);
static const unsigned int M_ROUGHNESS     = (1 << 5);

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix g_LightViewMatrix, g_LightProjMatrix;

Texture2D g_DiffuseTexture, g_NormalTexture, g_SpecularTexture;

Texture2D g_EmissiveTexture, g_MetalicTexture, g_RoughnessTexture;
Texture2D g_MetalnessTexture;
Texture2D g_DissolveTexture;

bool g_isDiffuse = false;
bool g_isNormal = false;
bool g_isEmissive = false;
bool g_isSpecular = false;
bool g_isMetalic = false;
bool g_isRoughness = false;

// 귀검 관련 전역 변수
bool g_isBNEye = false;

/* 모델 전체 뼈기준(x) */
/* 특정 메시에 영향ㅇ르 주는 뼈들 */
matrix g_BoneMatrices[512];

float g_fEmissiveIntensity = 1.f;
bool g_isEnableEmissive, g_isEnableBloom;

// Outline
float g_fOutlineSize = 0.001f;
float3 g_vOutlineColor = { 1.f, 0.f, 1.f };

// Edge
bool g_isEnableEdge = true;
float g_fEdgeIntensity, g_fShadeIntensity;
float4 g_vCamPosition;

// Test
float2 g_vLifeTime;
float3 g_vStartColor, g_vTargetColor;
float g_fRimPower, g_fRimLightIntensity;

// 귀검
float g_fColorRatio;
float3 g_vRimColor;

// Blink RimLight
float g_fTimeDelta;
float g_fCycleSpeed;
float g_fRimEmissive;

//이미시브
float g_fEmissiveValue;

//디죨브
float   g_fDecreaseAlpha;
float   g_fEdgeWidth;
float4  g_fEdgeColor;

// Viper
bool g_HasEmissive;
float g_fGreenIntensity = 1.f;
bool g_isEnableRimLight, g_isBlinkRimLight;
float g_fMinRimIntensity;
bool g_isMaskDiffuse = false;

// Imp
float g_fDiffusePower = 1.f;
float g_fBluePower = 1.f;

//눈 관련 값
float4 g_vEyeWhiteColor;            //눈 흰자
float4 g_vPupilCircle;              //홍채
float4 g_vPupilLens;                //홍채 내부
float4 g_vPupilRing;                //홍채 외곽 강조
float4 g_vShadingColor;             //조명 조정 및 빛 반사
float g_PupilScale;                 //동공 크기

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
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

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
      /* ?젙?젏?쓽 濡쒖뺄?쐞移? * ?썡?뱶 * 酉? * ?닾?쁺 */ 
    
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
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 0.f;
    //  Out.vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // Test
    if (g_isEnableEdge)
    {
        float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
        
        float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
        float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
        Out.vDiffuse *= fEdgeMask;
        Out.vDiffuse *= fShadeMask;
    }

    return Out;
}

PS_OUT PS_MAIN_NONPICK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = vector(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 1.f;
    //  Out.vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    // Test
    if (g_isEnableEdge)
    {
        float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
        float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
        Out.vDiffuse *= fEdgeMask;
        Out.vDiffuse *= fShadeMask;
    }
    
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
    // NonBlend 媛앹껜
    
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);

    //  Out.vDiffuse = vMtrlDiffuse;
    // Alpha Rim Light Test
    //  Out.vDiffuse.a = 0.f;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 1.f;
    
    if (true == g_isEnableEmissive)
        Out.vDiffuse.rgb *= g_fEmissiveIntensity;
    
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
    
    //  vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    //  
    //  if (vMtrlDiffuse.a < 0.3f)
    //      discard;
    
    // Alpha Rim Light Test
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    // Rim Light
    vector vLook = normalize(g_vCamPosition - In.vWorldPos);
    
    float fRim = 1.f - saturate(dot(float4(vNormal, 0.f), vLook));
    fRim = pow(fRim, g_fRimPower);

    float fLifeRatio = g_vLifeTime.x / g_vLifeTime.y;
    
    float3 vFinalColor = lerp(g_vStartColor, g_vTargetColor, fLifeRatio);
    float fFinalAlpha = 1.f - fLifeRatio;
    
    // Rim Color * Rim * Rim Light Intensity * Emissive Intensity
    Out.vPostScene = float4(vFinalColor, fFinalAlpha) * fRim * g_fRimLightIntensity * g_fEmissiveIntensity;
    
    return Out;
}

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
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
    {
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
        
        float3 vNormal = vMtrlNormal.xyz * 2.f - 1.f;
        
        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
        vNormal = mul(vNormal, WorldMatrix);
        
        vMtrlNormal = float4(normalize(vNormal.xyz), 0.f);
    }
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
    {
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
    {
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
        vMtrlSpecular.a = 0.f;
    }
    
    vector vMtrlMetalic = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_METALIC))
    {
        vMtrlMetalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
        
        //  float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMtrlMetalic.r);
        //  float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMtrlMetalic.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
        
        float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMtrlMetalic.r);
        float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMtrlMetalic.g);
        
        Out.vDiffuse *= fEdgeMask;
        Out.vDiffuse *= fShadeMask;
    }
    
    vector vMtrlRoughness = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_ROUGHNESS))
    {
        vMtrlRoughness = g_RoughnessTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    //  Out.vDiffuse = vMtrlDiffuse * vMtrlSpecular * g_fEmissiveIntensity;
    vMtrlDiffuse.g = 0.f;
    Out.vDiffuse = lerp(vMtrlDiffuse, vMtrlSpecular, g_fColorRatio) * g_fEmissiveIntensity;
    //  Out.vDiffuse = vMtrlMetalic;
    // Out.vDiffuse = vMtrlSpecular * 10.f;
    
    Out.vNormal = vector(vMtrlNormal);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    //  Out.vEmissive = vMtrlEmissive;
    
    //  Out.vEmissive = vMtrlSpecular * 2.f;

    return Out;
}

PS_OUT PS_MAP_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
    {
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    float3 vNormal = normalize(vMtrlNormal.xyz * 2.f - 1.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
    {
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
    {
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlMetalic = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_METALIC))
    {
        vMtrlMetalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    // 미사용으로 주석 처리
    //vector vMtrlRoughness = float4(0.f, 0.f, 0.f, 0.f);
    //if (IsFlag(M_ROUGHNESS))
    //{
    //    vMtrlRoughness = g_RoughnessTexture.Sample(DefaultSampler, In.vTexcoord);
    //}
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = vMtrlSpecular;
    Out.vSpecular.a = 0.f;
    Out.vEmissive = vMtrlEmissive;

    return Out;
}

PS_OUT PS_RUNE_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 tex = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);

    if (tex.a < 0.1f)
        discard;

    // 마스크 분리
    float runeMask = tex.r;
    float energyMask = tex.g;
    float noiseMask = tex.b;

    // 원작 느낌의 청록 블루 계열
    float3 runeColor = float3(0.2f, 0.8f, 1.0f);
    float3 energyColor = float3(0.1f, 0.6f, 0.9f);
    float3 noiseColor = float3(0.05f, 0.15f, 0.3f);
    
    float3 emissive = float3(0.f, 0.f, 0.f);
    
    if (IsFlag(M_EMISSIVE))
    {
        emissive = runeMask * runeColor * 18.0f + energyMask * energyColor * 6.0f + noiseMask * noiseColor * 2.0f;
    }

    Out.vDiffuse = float4(0.f, 0.f, 0.f, 1.f);
    Out.vNormal = float4(0.5f, 0.5f, 1.f, 0.f);
    Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);

    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;

    // 반드시 Emissive로 출력
    Out.vEmissive = float4(emissive, 1.f);

    return Out;
}

PS_OUT PS_LANTERN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = vector(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fEmissvie = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord).g;

    Out.vEmissive = float4(fEmissvie, fEmissvie, fEmissvie, 1.f);
    Out.vEmissive *= g_fEmissiveIntensity;
    
    return Out;
}

struct PS_OUT_VELOCITY
{
    float4 vVelocity : SV_TARGET0;
};

PS_OUT_VELOCITY PS_MOTIONVECTOR(PS_IN In)
{
    PS_OUT_VELOCITY Out = (PS_OUT_VELOCITY) 0;
    
    Out.vVelocity = float4(1.f, 0.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_BLINK_UNLOCK_GEAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
    {
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    float3 vNormal = normalize(vMtrlNormal.xyz * 2.f - 1.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
    {
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
    {
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlMetalic = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_METALIC))
    {
        vMtrlMetalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlRoughness = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_ROUGHNESS))
    {
        vMtrlRoughness = g_RoughnessTexture.Sample(DefaultSampler, In.vTexcoord);
    }

    // Emissive
    float fMask = vMtrlEmissive.b;
    vMtrlDiffuse += vMtrlDiffuse * fMask * g_fEmissiveIntensity;
    
    // Rim Light
    vector vLook = normalize(g_vCamPosition - In.vWorldPos);
    float fRim = 1.f - saturate(dot(float4(vNormal, 0.f), vLook));
    fRim = pow(fRim, g_fRimPower);
    
    // Blink Cycle = cos = (-1 ~ 1) + 1 -> (0 ~ 2) * RimIntensity / 2 => 0 ~ 1
    float fFinalIntensity = g_fRimLightIntensity / 2.f * (1.f + cos(g_fTimeDelta * g_fCycleSpeed));

    float3 vRimColor = g_vRimColor * fRim * fFinalIntensity;

    Out.vDiffuse.rgb = vMtrlDiffuse.rgb + vRimColor * g_fRimEmissive; // Rim Emissive
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = vMtrlSpecular * 0.1f;
    Out.vSpecular.a = 0.f;
    //  Out.vEmissive = vMtrlEmissive;
    
    return Out;
}

PS_OUT PS_UNLOCK_GEAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
    {
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    float3 vNormal = normalize(vMtrlNormal.xyz * 2.f - 1.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
    {
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
    {
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlMetalic = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_METALIC))
    {
        vMtrlMetalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlRoughness = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_ROUGHNESS))
    {
        vMtrlRoughness = g_RoughnessTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    float fMask = vMtrlEmissive.b;

    vMtrlDiffuse += vMtrlDiffuse * fMask * g_fEmissiveIntensity;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = vMtrlSpecular * 0.1f;
    Out.vSpecular.a = 0.f;
    //  Out.vEmissive = vMtrlEmissive;

    return Out;
}

PS_OUT PS_ELEVATOR_L(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
    {
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    float3 vNormal = normalize(vMtrlNormal.xyz * 2.f - 1.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
    {
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
    {
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlMetalic = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_METALIC))
    {
        vMtrlMetalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlRoughness = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_ROUGHNESS))
    {
        vMtrlRoughness = g_RoughnessTexture.Sample(DefaultSampler, In.vTexcoord);
    }

    Out.vDiffuse = vMtrlDiffuse * vMtrlSpecular.r * g_fEmissiveIntensity;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    //  Out.vSpecular = vMtrlSpecular * 0.1f;
    //  Out.vSpecular.a = 0.f;
    //  Out.vEmissive = vMtrlEmissive;

    return Out;
}

PS_OUT PS_MAIN_HAIR_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    float4 vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = vector(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular.a = 1.f;
    Out.vSpecular.rgb = vSpecular.rgb * 1.f;
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    Out.vDiffuse *= fEdgeMask;
    Out.vDiffuse *= fShadeMask;
    
    vEmissive.b *= g_fEmissiveValue;
    Out.vDiffuse = Out.vDiffuse + (vEmissive * 1.f);
    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, Out.vDiffuse);
    
    if (Out.vDiffuse.a <= 0.f)
        discard;
    
    return Out;
}


PS_OUT PS_MAIN_SHIELD_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = vector(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 1.f;
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
   
    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    Out.vDiffuse *= fEdgeMask;
    Out.vDiffuse *= fShadeMask;

    if (vEmissive.g >= 0.95f)
    {
        float3 vColor = { 2.455f, 1.937f, 2.784f };
        //  Out.vDiffuse.rgb = Out.vDiffuse.rgb + vColor * 0.3f * g_fEmissiveValue;
        Out.vEmissive.rgb = Out.vDiffuse.rgb + vColor * 0.3f * g_fEmissiveValue;
    }
    
    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, Out.vDiffuse);
    if (Out.vDiffuse.a <= 0.f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = vector(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 1.f;
    //  Out.vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g);
    Out.vDiffuse *= fEdgeMask;
    Out.vDiffuse *= fShadeMask;

    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, Out.vDiffuse);
    
    
    if (Out.vDiffuse.a <= 0.f)
        discard;
    return Out;
}

PS_OUT PS_VIPER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;

    float4 vEmissiveDesc = float4(0.f, 0.f, 0.f, 0.f);
    if (g_HasEmissive)
        vEmissiveDesc = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 0.f;
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
        
    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    vMtrlDiffuse *= fEdgeMask;
    vMtrlDiffuse *= fShadeMask;
    
    vMtrlDiffuse.g *= g_fGreenIntensity;
    
    float fMask = vEmissiveDesc.g;    
    vMtrlDiffuse += vMtrlDiffuse * fMask * g_fEmissiveIntensity;

    Out.vDiffuse = vMtrlDiffuse;
    
    // Rim Light
    if (g_isEnableRimLight)
    {
        vector vLook = normalize(g_vCamPosition - In.vWorldPos);
        float fRim = 1.f - saturate(dot(float4(vNormal, 0.f), vLook));
        fRim = pow(fRim, g_fRimPower);
        float fFinalIntensity = g_vRimColor * fRim * g_fRimLightIntensity;
        
        if (g_isBlinkRimLight)
        {
            fFinalIntensity = g_fRimLightIntensity / 2.f * (1.f + cos(g_fTimeDelta * g_fCycleSpeed));
            fFinalIntensity = clamp(fFinalIntensity, g_fMinRimIntensity, 1.f);
        }
        
        float3 vRimColor = g_vRimColor * fRim * fFinalIntensity;

        Out.vDiffuse.rgb = vMtrlDiffuse.rgb + vRimColor * g_fRimEmissive; // Rim Emissive
    }

    return Out;
}

PS_OUT PS_IMP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = vector(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 0.f;
    //  Out.vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    Out.vDiffuse *= fEdgeMask;
    Out.vDiffuse *= fShadeMask;

    // Diffuse 어두운 문제로 임의값 곱해주기 3~5
    Out.vDiffuse.b *= g_fBluePower;
    Out.vDiffuse *= g_fDiffusePower;
    
    return Out;
}

PS_OUT PS_DANJINJAR(PS_IN In)
{    
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
    {
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    float3 vNormal = normalize(vMtrlNormal.xyz * 2.f - 1.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
    {
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
    {
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    
    vector vMtrlMetalic = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_METALIC))
    {
        vMtrlMetalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    }

    // Rim Light
    vector vLook = normalize(g_vCamPosition - In.vWorldPos);
    float fRim = 1.f - saturate(dot(float4(vNormal, 0.f), vLook));
    fRim = pow(fRim, g_fRimPower);

    float3 vRimColor = g_vRimColor * fRim * g_fRimLightIntensity;
    
    Out.vDiffuse = vMtrlDiffuse + float4(vRimColor, 1.f) * g_fRimEmissive;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = vMtrlSpecular;
    Out.vSpecular.a = 0.f;
    Out.vEmissive = vMtrlDiffuse * g_fDiffusePower;
    
    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMtrlMetalic.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMtrlMetalic.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    Out.vDiffuse *= fEdgeMask;
    Out.vDiffuse *= fShadeMask;

    return Out;
}

PS_OUT PS_MAIN_DISSOLVE_EYE_ELAMEIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 EyeWhiteColor = float4(0.760417, 0.760417, 0.760417, 1.0);
    float4 PupilRing = float4(0.291254, 0.223749, 0.427083, 1.0);
    float4 ShadingColor = float4(1.081201, 0.62283, 1.2, 1.0);

    float pupilScale = 0.18 * 0.75f; // 0.75 들어감
    float irisScale = 0.34;

    float dist = distance(In.vTexcoord, float2(0.5, 0.5));
    float4 color = EyeWhiteColor;

    color = lerp(color, PupilRing * 10.f, color);
    color.rgb *= ShadingColor.rgb;
    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, color);
    
    
    if (Out.vDiffuse.a <= 0.f)
        discard;
    return Out;
}

PS_OUT PS_MAIN_DISSOLVE_EYE_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
        
    float pupilScale = 0.18 * g_PupilScale; // 0.75 들어감
    float irisScale = 0.34;

    float dist = distance(In.vTexcoord, float2(0.5, 0.5));
    float4 color = g_vEyeWhiteColor;
    color = lerp(color, g_vPupilCircle * 5.f, smoothstep(irisScale, irisScale - 0.05, dist));
    color = lerp(color, g_vPupilLens, smoothstep(pupilScale, pupilScale - 0.03, dist));
    float ring = smoothstep(pupilScale + 0.02, pupilScale, dist);
    color = lerp(color, g_vPupilRing, color);
    color.rgb *= g_vShadingColor.rgb;
    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, color);
    
    
    if (Out.vDiffuse.a <= 0.f)
        discard;
    return Out;
}

PS_OUT PS_VIPER_WEAPON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;

    float4 vEmissiveDesc = float4(0.f, 0.f, 0.f, 0.f);
    if (g_HasEmissive)
        vEmissiveDesc = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 0.f;
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
        
    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    vMtrlDiffuse *= fEdgeMask;
    vMtrlDiffuse *= fShadeMask;
    
    vMtrlDiffuse.g *= g_fGreenIntensity;
    
    float fMask = vEmissiveDesc.g;
    vMtrlDiffuse += vMtrlDiffuse * fMask * g_fEmissiveIntensity;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.a = vMtrlDiffuse.r;
    
    // Rim Light
    if (g_isEnableRimLight)
    {
        vector vLook = normalize(g_vCamPosition - In.vWorldPos);
        float fRim = 1.f - saturate(dot(float4(vNormal, 0.f), vLook));
        fRim = pow(fRim, g_fRimPower);
        float fFinalIntensity = g_vRimColor * fRim * g_fRimLightIntensity;
        
        if (g_isBlinkRimLight)
        {
            fFinalIntensity = g_fRimLightIntensity / 2.f * (1.f + cos(g_fTimeDelta * g_fCycleSpeed));
            fFinalIntensity = clamp(fFinalIntensity, g_fMinRimIntensity, 1.f);
        }
        
        float3 vRimColor = g_vRimColor * fRim * fFinalIntensity;

        Out.vDiffuse.rgb = vMtrlDiffuse.rgb + vRimColor * g_fRimEmissive; // Rim Emissive
    }

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass        // 0 번
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NonPick            // 1 번
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONPICK();
    }

    pass Cascade // Depth Only          // 2 번
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = NULL;
    }

    pass Outline                        // 3 번
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_DepthTestOnly, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }

    pass Debug                          // 4 번
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass DebugNonLight                  // 5 번
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG_EMISSIVE();
    }

    pass DebugBlend                     // 6 번
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG_EMISSIVE();
    }

    pass SimpleColorView                // 7 번
    {

        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SIMPLE_COLOR_VIEW();
    }

    // 귀검 패스        ( 8번 )
    pass BladeNexusPass
    {

        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLADENEXUS();
    }

    // 맵 애님 패스        ( 9번 )
    pass MapAnimPass
    {

        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP_ANIM();
    }

    // 렌턴 패스        ( 10번 )
    pass Lantern
    {

        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LANTERN();
    }

    // 모션 벡터 패스        ( 11번 )
    pass MotionVector
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOTIONVECTOR();
    }

    // 깜빡이는 림 라이트 (활성화된 오브젝트용) 패스        ( 12번 )
    pass BlinkUnlockGear
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLINK_UNLOCK_GEAR();
    }

    // 언락 기어 패스        ( 13번 )
    pass UnlockGear
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UNLOCK_GEAR();
    }

    // 엘레베이터 L 패스        ( 14번 )
    pass ElevatorL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ELEVATOR_L();
    }

    // 엘라메인 헤어 패스         ( 15번 )
    pass PS_MAIN_HAIR_EMISSIVE_15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HAIR_EMISSIVE();
    }
    // 엘라메인 쉴드 패스        ( 16번 )
    pass PS_MAIN_SHIELD_EMISSIVE_16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHIELD_EMISSIVE();
    }
    // 디죨브         ( 17번 )
    pass PS_MAIN_DISSOLVE_17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

    // 바이퍼 패스        ( 18번 )
    pass Viper
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_VIPER();
    }

    // 임프 패스        ( 19번 )
    pass Imp
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_IMP();
    }

    // 항아리 요정 패스        ( 20번 )
    pass DanjinJar
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DANJINJAR();
    }

    //눈 엘라메인 (21번)
    pass PS_MAIN_DISSOLVE_EYE_ELAMEIN_21
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE_EYE_ELAMEIN();
    }

    // 디죨브 컬 논       ( 22번 )
    pass PS_MAIN_DISSOLVE_22
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

    //공용 눈 (23번)
    pass PS_MAIN_DISSOLVE_EYE_DEFAULT_23
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE_EYE_DEFAULT();
    }

    // 룬 문자 (24번)
    pass PS_RUNE_EMISSIVE_24
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RUNE_EMISSIVE();
    }

    // 바이퍼 무기 마스크 패스        ( 25번 )
    pass Viper_Weapon
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_VIPER_WEAPON();
    }

    // 맵 애님 패스 ( 논 컬 )        ( 26번 )
    pass MapAnimNonCullPass
    {

        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP_ANIM();
    }
}
