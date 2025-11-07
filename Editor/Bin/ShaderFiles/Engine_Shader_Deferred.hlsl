#include "Engine_Shader_Defines.hlsli"

// ===== Matrix =====
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
Texture2D g_Texture;
vector g_vCamPosition;

// ===== Light =====
vector g_vLightDir, g_vLightPos;
float g_fRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
bool g_isEnableToonShade;
float g_fToonShadeLevel = { 3.f };

// ===== Material =====
vector g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f }, g_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

// ===== Textures =====
Texture2D g_DiffuseTexture, g_NormalTexture, g_DepthTexture, g_ShadeTexture, g_SpecularTexture, g_EmissiveTexture;
Texture2D g_LightDepthTexture, g_PostSceneTexture, g_BlurXTexture, g_BloomTexture, g_FogTexture, g_OutlineTexture;

// ===== Cascade Shadow =====
int g_iTextureArrayIndex;
uint g_iNumCascades;
float g_Splits[4];
matrix g_LightViewMatrices[4], g_LightProjMatrices[4];
float2 g_vShadowMapSize;
float g_fBias;
bool g_isEnableShadow = { true };

// ===== PCF =====
Texture2DArray<float> g_TextureArray;

// ===== SSAO =====
Texture2D g_NoiseTexture, g_SSAOTexture;
float2 g_vScreenSize;
StructuredBuffer<float3> g_Kernels;
uint g_iNumKernels;
float g_fRadius = { 1.f };
float g_fIntensity = { 1.f }, g_fContrast = { 1.f };
bool g_isEnableSSAO = { true };
matrix g_CameraViewMatrix, g_CameraProjMatrix;

// ===== Blur =====
float g_fViewportWidth, g_fViewportHeight;
StructuredBuffer<float> g_Weights;
float g_fNormalization;
int g_iWeightRadius;

// ===== Fog =====
uint g_iFogMode;
float g_fFogNear, g_fFogFar;
float g_fFogDensity;
float4 g_vFogColor = { 1.f, 1.f, 1.f, 1.f };
bool g_isEnableFog;
float g_fTimeDelta;
bool g_isEnableNoise, g_isWorldFog;
float2 g_vNoiseSpeed, g_vNoiseScale;
float g_fNoiseStrength, g_fNoiseContrast;

// ===== Outline =====
float g_fOutlineAlpha = { 1.f };
float g_fOutlineBias = { 0.01f };
bool g_isEnableOutline;
float g_fCameraFar = { 1000.f };

// ===== Vignette =====
float g_fVignettePower = { 1.f };
float g_fVignetteIntensity = { 1.f };
float3 g_vVignetteColor = { 0.f, 0.f, 0.f };
bool g_isEnableVignette;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_DEBUG_ARRAY(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    float fSlice = (float) g_iTextureArrayIndex;
    Out.vColor = g_TextureArray.Sample(DefaultSampler, float3(In.vTexcoord, fSlice));
    
    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * vDepthDesc.y;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    float fShade = max(dot(vNormal * -1.f, normalize(g_vLightDir)), 0.f);

    // Toon Shade
    float fLightIntensity = saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));
    float fLevel = floor(g_fToonShadeLevel);
    float fToonShade = ceil(fLightIntensity * fLevel) / fLevel;
    
    if (g_isEnableSSAO)
    {
        float fAO = g_SSAOTexture.Sample(PointSampler, In.vTexcoord).r;
        fToonShade *= fAO;
        
        fLightIntensity *= fAO;
    }
    
    if (g_isEnableToonShade)
        Out.vShade = g_vLightDiffuse * fToonShade;
    else
        Out.vShade = g_vLightDiffuse * fLightIntensity;
    
    // Specular
    vector vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    vector vLook = vWorldPos - g_vCamPosition;
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 1.f);
    
    Out.vSpecular = (g_vLightSpecular * vSpecularDesc) * fSpecular;
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * vDepthDesc.y;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = saturate((g_fRange - fDistance) / g_fRange);
    
    float fShade = max(dot(vNormal * -1.f, normalize(g_vLightDir)), 0.f);

    // Toon Shade
    float fLightIntensity = saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));
    float fLevel = floor(g_fToonShadeLevel);
    float fToonShade = ceil(fLightIntensity * fLevel) / fLevel;
    
    if (g_isEnableSSAO)
    {
        float fAO = g_SSAOTexture.Sample(PointSampler, In.vTexcoord).r;
        fToonShade *= fAO;
        
        fLightIntensity *= fAO;
    }
    
    if (g_isEnableToonShade)
        Out.vShade = g_vLightDiffuse * fToonShade * fAtt;
    else
        Out.vShade = g_vLightDiffuse * fLightIntensity * fAtt;

    // Specular
    vector vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vLook = vWorldPos - g_vCamPosition;
    float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular * fAtt;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_POSTSCENE(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (1.f == vDiffuse.r && 1.f == vDiffuse.g && 1.f == vDiffuse.b && 0.f == vDiffuse.a)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);

    //  Out.vColor = vDiffuse * vShade;
    Out.vColor = vDiffuse * vShade + vSpecular;
    
    if (!g_isEnableShadow)
        return Out;
    
    // Pixel Depth
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * vDepthDesc.y;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    float fCameraViewDepth = vWorldPos.z;

    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    uint iCascadeIndex = 0;
    
    if (fCameraViewDepth < g_Splits[0])
        iCascadeIndex = 0;
    else if (fCameraViewDepth < g_Splits[1])
        iCascadeIndex = 1;
    else if (fCameraViewDepth < g_Splits[2])
        iCascadeIndex = 2;
    else
        iCascadeIndex = 3;
    
    vector vPosition = mul(vWorldPos, g_LightViewMatrices[iCascadeIndex]);
    vPosition = mul(vPosition, g_LightProjMatrices[iCascadeIndex]);
    
    float2 vTexcoord;
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

    float fLightDepth = vPosition.z / vPosition.w;
    
    float fShadowSum = 0.f;
    float2 vOffset;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= i; ++j)
        {
            vOffset.x = j * 1.f / g_vShadowMapSize.x;
            vOffset.y = i * 1.f / g_vShadowMapSize.y;
            float3 vSampleCoord = saturate(float3(vTexcoord + vOffset, iCascadeIndex));
            fShadowSum += g_TextureArray.SampleCmpLevelZero(ComparisonSampler, vSampleCoord, fLightDepth - g_fBias);
        }
    }
    
    fShadowSum /= 9.f;
    
    Out.vColor = lerp(Out.vColor * 0.3f, Out.vColor, fShadowSum);
    
    return Out;
}

struct PS_OUT_BLUR_X
{
    vector vBlurX : SV_TARGET0;
};

PS_OUT_BLUR_X PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;

    float2 vTexcoord;
    float3 vColor = float3(0.f, 0.f, 0.f);;

    for (int i = -g_iWeightRadius; i < g_iWeightRadius + 1; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + i / g_fViewportWidth;
        vTexcoord.y = In.vTexcoord.y;
        
        // 기존 샘플링
        float4 vEmissive = g_EmissiveTexture.SampleLevel(ClampSampler, vTexcoord, 0.f);
        float3 vEmissiveColor = vEmissive.rgb * vEmissive.a;
        
        // 포스트씬 샘플링
        float3 vPostSceneColor = g_PostSceneTexture.SampleLevel(ClampSampler, vTexcoord, 0.f).rgb;
        
        // 임계치 넘는 영역만 추출
        float3 vBrightColor = max(vPostSceneColor - 1.f, 0.f); // 씬 컬러 - 임계치
        
        // 이미시브 + 밝은 영역
        float3 vCombinedColor = vEmissiveColor + vBrightColor;
        
        // 합친 컬러에 가중치 곱
        vColor += g_Weights[i + g_iWeightRadius] * vCombinedColor;
    }

    Out.vBlurX = float4(vColor / g_fNormalization, 1.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    float3 vColor = float3(0.f, 0.f, 0.f);
    
    for (int i = -g_iWeightRadius; i < g_iWeightRadius + 1; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + i / g_fViewportHeight;
        
        float4 vEmissive = g_BlurXTexture.SampleLevel(ClampSampler, vTexcoord, 0.f);
        
        vColor += g_Weights[i + g_iWeightRadius] * vEmissive.rgb * vEmissive.a;
    }

    Out.vColor = float4(vColor / g_fNormalization, 1.f);
    
    return Out;
}

struct PS_OUT_SSAO
{
    float4 vSSAO : SV_TARGET0;
};

PS_OUT_SSAO PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT_SSAO Out;

    // Noise Texture -> RandomVector
    float2 vNoiseScale = g_vScreenSize / 4.f;
    float3 vRandomVector = g_NoiseTexture.Sample(PointSampler, In.vTexcoord * vNoiseScale).xyz;
    vRandomVector = normalize(vRandomVector);
    
    // View Space Normal
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vNormalDesc.w >= 1.f)
        discard;
    
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float3x3 ViewMatrix = float3x3(g_CameraViewMatrix._11_12_13, g_CameraViewMatrix._21_22_23, g_CameraViewMatrix._31_32_33);
    vNormal = normalize(mul(vNormal.xyz, ViewMatrix));
    
    // TBN
    float3 vTangent = normalize(vRandomVector - vNormal * dot(vRandomVector, vNormal));
    float3 vBinormal = normalize(cross(vNormal, vTangent));
    float3x3 TBNMatrix = float3x3(vTangent, vBinormal, vNormal);
    
    // Depth
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vNDCPos;
    
    vNDCPos.x = In.vTexcoord.x * 2.f - 1.f;
    vNDCPos.y = In.vTexcoord.y * -2.f + 1.f;
    vNDCPos.z = vDepthDesc.x;
    vNDCPos.w = 1.f;

    // Projection -> View
    vNDCPos = vNDCPos * vDepthDesc.y; // View.Z
    float4 vViewPos = mul(vNDCPos, g_ProjMatrixInv); // 현재 픽셀의 View Space 위치
    
    // Occlusion
    float fOcclusion = 0.f;
 
    [loop]
    for (uint i = 0; i < g_iNumKernels; ++i)
    {
        // Kernle Vector Rotation -> View Space Dir
        float3 vSampleDir = mul(g_Kernels[i], TBNMatrix);
        
        // View Sample Position
        float3 vSamplePos = vViewPos.xyz + vSampleDir * g_fRadius; // 주변 SampleDir 방향으로 Radius 반경만큼 떨어진 샘플 위치
        
        // Sample Position -> UV
        // 샘플 포지션 투영 변환 -> 투영 행렬 곱해주고 w나누기 후 -1 ~ 1 => 0 ~ 1 범위로 변환 * 0.5 + 0.5
        float4 vProjSamplePos = mul(float4(vSamplePos, 1.f), g_CameraProjMatrix);
        vProjSamplePos /= vProjSamplePos.w;
        float2 vSampleTexcoord;
        vSampleTexcoord.x = vProjSamplePos.x * 0.5f + 0.5f;
        vSampleTexcoord.y = vProjSamplePos.y * -0.5f + 0.5f;
        
        // 샘플 깊이 비교
        float fSampleDepth = g_DepthTexture.Sample(PointSampler, vSampleTexcoord).y; // 샘플 픽셀(주변 픽셀)의 깊이
        
        // 샘플 깊이가 현재 픽셀의 깊이보다 더 크면 차폐가 없음, 샘플 깊이가 현재 픽셀의 깊이보다 더 작으면 그 방향에 다른 오브젝트가 있어서 시야가 막힌 것
        
        // Smoothstep -> 갑자기 0 ~ 1 안 되게 경계를 부드럽게 만들어줌
        // 반경 / 깊이 차이 -> 멀수록 값이 작아짐
        // => 깊이 차이가 크면 차폐도 영향 적게, 가까우면 영향 크게 해서 부드럽게 만들어줌
        float fRangeLerp = smoothstep(0.f, 1.f, g_fRadius / abs(vViewPos.z - fSampleDepth));
        
        // 차폐도 누적
        fOcclusion += (vSamplePos.z >= fSampleDepth ? 1.f : 0.f) * fRangeLerp;
    }

    // 정규화 및 보정 -> 막힘 비율 -> 빛이 닿는 정도로 반전
    // Occlusion = 막힌 방향의 수
    fOcclusion = 1.f - (fOcclusion / (float)g_iNumKernels);
    
    float fAO = pow(saturate(fOcclusion * g_fIntensity), g_fContrast);
    
    Out.vSSAO = float4(fAO, fAO, fAO, 1.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vPostSceneDesc = g_PostSceneTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vEmissiveDesc = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vBloomDesc = g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vFogDesc = g_FogTexture.Sample(DefaultSampler, In.vTexcoord);

    //  if (1.f == vPostSceneDesc.r && 0.f == vPostSceneDesc.g && 1.f == vPostSceneDesc.b)
    //      discard;

    //  if (true == g_isEnableFog)
    //      Out.vColor = vFogDesc + vEmissiveDesc + vBloomDesc;
    //  else
    //      Out.vColor = vPostSceneDesc + vEmissiveDesc + vBloomDesc;
    
    float4 vFinalColor;
    
    if (true == g_isEnableFog)
        vFinalColor = vFogDesc + vBloomDesc;
    else
        vFinalColor = vPostSceneDesc + vBloomDesc;

    // Outline
    if (g_isEnableOutline)
    {
        vector vOutlineDesc = g_OutlineTexture.Sample(DefaultSampler, In.vTexcoord);
        vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
        //  // Depth View Space
        //  vector vNDCPos;
        //  float4 vViewPos;
        //  
        //  vNDCPos.x = In.vTexcoord.x * 2.f - 1.f;
        //  vNDCPos.y = In.vTexcoord.y * -2.f + 1.f;
        //  vNDCPos.z = vDepthDesc.x;
        //  vNDCPos.w = 1.f;
        //  
        //  vNDCPos = vNDCPos * vDepthDesc.y; // View.Z
        //  vViewPos = mul(vNDCPos, g_ProjMatrixInv); // 현재 픽셀의 View Space 위치
        //  float fViewDepth = vViewPos.z;
        //  
        //  // Outline View Space
        //  vNDCPos.x = In.vTexcoord.x * 2.f - 1.f;
        //  vNDCPos.y = In.vTexcoord.y * -2.f + 1.f;
        //  vNDCPos.z = vOutlineDesc.w;
        //  vNDCPos.w = 1.f;
        //  
        //  vNDCPos = vNDCPos * vDepthDesc.y; // View.Z
        //  vViewPos = mul(vNDCPos, g_ProjMatrixInv); // 현재 픽셀의 View Space 위치
        //  float fOutlineDepth = vViewPos.z;
        //  
        //  // View 비교
        //  bool isOutline = (fViewDepth >= g_fCameraFar - g_fOutlineBias && fOutlineDepth < fViewDepth);
        
        // 0~1 비교
        bool isOutline = (vDepthDesc.x >= 1.f - g_fOutlineBias && vOutlineDesc.w <= 1.f);

        if (isOutline)
            vFinalColor *= float4(vOutlineDesc.rgb, g_fOutlineAlpha);
    }
    
    // Vignette
    if (g_isEnableVignette)
    {
        float fDistance = length(In.vTexcoord - 0.5f);
        float fVignetteFactor = 1.f - pow(fDistance, g_fVignettePower) * g_fVignetteIntensity;

        //  vFinalColor.rgb = vFinalColor.rgb * fVignetteFactor * g_vVignetteColor + vFinalColor.rgb * fVignetteFactor;
        vFinalColor.rgb = lerp(g_vVignetteColor, vFinalColor.rgb, fVignetteFactor);
        vFinalColor.a = vPostSceneDesc.a;
    }

    Out.vColor = vFinalColor;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_FOG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vPostSceneDesc = g_PostSceneTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    // View Space
    vWorldPos = vWorldPos * vDepthDesc.y;   // View Z
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    float fViewZ = vWorldPos.z;
    
    // World Space
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    // Linear
    float fLinear = saturate((fViewZ - g_fFogNear) / (g_fFogFar - g_fFogNear));
    
    // Exponential -> (1 - e^{-rho * d})
    float fDistance = abs(fViewZ);
    float fExp = saturate(1.f - exp(-g_fFogDensity * fDistance));
    
    // Exponential^2 -> (1 - e^{-(rho * d)^2})
    float fOpticalDepth = g_fFogDensity * fDistance;    // 광학 깊이
    float fExpSquare = saturate(1.f - exp(-(fOpticalDepth * fOpticalDepth)));
    
    float4 vResultColor;
    float fFogFactor = 0.f;
        
    if (0 == g_iFogMode)
        fFogFactor = fLinear;
    else if (1 == g_iFogMode)
        fFogFactor = fExp;
    else if (2 == g_iFogMode)
        fFogFactor = fExpSquare;

    if (true == g_isEnableNoise)
    {
        float2 vNoiseTexcoord;
        
        if (true == g_isWorldFog)
        {
            vNoiseTexcoord = vWorldPos.xz * g_vNoiseScale;
            vNoiseTexcoord.x += g_fTimeDelta * g_vNoiseSpeed.x;
            vNoiseTexcoord.y += g_fTimeDelta * g_vNoiseSpeed.y;
        }
        else
        {
            vNoiseTexcoord = In.vTexcoord * g_vNoiseScale;
            vNoiseTexcoord.x += g_fTimeDelta * g_vNoiseSpeed.x;
            vNoiseTexcoord.y += g_fTimeDelta * g_vNoiseSpeed.y;
        }
        
        float fNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseTexcoord).r;
        fNoise = pow(fNoise, g_fNoiseContrast);
        
        fFogFactor = lerp(fFogFactor, fFogFactor * fNoise, g_fNoiseStrength);
    }
    
    vResultColor = lerp(vPostSceneDesc, g_vFogColor, fFogFactor);
    Out.vColor = vResultColor;
    //  Out.vColor = float4(vResultColor.rgb, vPostSceneDesc.a);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass PostScene
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTSCENE();
    }

    pass BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass BlurY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass DebugArray
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG_ARRAY();
    }

    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass Fog
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOG();
    }
}
