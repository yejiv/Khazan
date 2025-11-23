#include "Engine_Shader_Defines.hlsli"
#include "Engine_Shader_Deferred_Function.hlsli"

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

struct PS_OUT_WEIGHTBLEND   //MRT_PostScene의 Diffsue에 원래 Blend에서 그려줘야했던 내용들 합성해서 그려줌.
{
    float4 vBackBufferColor : SV_TARGET0;
    float4 vEmissiveColor : SV_TARGET1;
};

PS_OUT_BACKBUFFER PS_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_DEBUG_ARRAY(PS_IN In)
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

PS_OUT_LIGHT PS_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vWorldPos = Compute_WorldPosition_FromDepth(In.vTexcoord, vDepthDesc.x, vDepthDesc.y);
    
    float fShade = max(dot(vNormal * -1.f, normalize(g_vLightDir)), 0.f);

    // Toon Shade 명암 대비 강화
    
    // 엠비언트 따로 계산
    float fAmbient = g_vLightAmbient * g_vMtrlAmbient;
    
    float fLevel = floor(g_fToonShadeLevel);

    // 엠비언트를 제외한 순수한 fShade 값으로 툰 셰이딩
    float fToonShade = ceil(fShade * fLevel) / fLevel;
    
    // 엠비언트 분리 후 합산
    float fLightIntensity = saturate(fShade);
    
    if (g_isEnableSSAO)
    {
        float fAO = g_SSAOTexture.Sample(PointSampler, In.vTexcoord).r;
        fToonShade *= fAO;
        fLightIntensity *= fAO;
        fAmbient *= fAO;
    }
    
    if (g_isEnableToonShade)
        Out.vShade = g_vLightDiffuse * (fToonShade + fAmbient);
    else
        Out.vShade = g_vLightDiffuse * (fLightIntensity + fAmbient);
    
    // Specular
    vector vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    float fSpecularValue = vSpecularDesc.g;
    float fSpecularIntensity = vSpecularDesc.r;
    
    float fShininess = lerp(g_vSpecularPower.x, g_vSpecularPower.y, fSpecularValue);
    
    vector vLook = normalize(vWorldPos - g_vCamPosition);
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    float fSpecularBase = max(dot(normalize(vReflect) * -1.f, vLook), 0.f);
    float fSpecular = pow(fSpecularBase, fShininess);
    
    Out.vSpecular = g_vLightSpecular * fSpecular * fSpecularValue + g_vLightSpecular * fSpecular * fSpecularIntensity * 2.f;
    
    if (0.f == vSpecularDesc.a)
        return Out;
    
    // Rim Light
    float fRim = 1.f - saturate(dot(vNormal, vLook * -1.f));
    fRim = pow(fRim, g_fRimPower);
    
    // 빛을 등지고 있을 때
    float fRimFactor = max(dot(g_vLightDir, vLook * -1.f), 0.f);
    fRim *= fRimFactor;
    
    // Rim Toon
    if (g_isToonLight)
        fRim = step(g_fRimToonThreshold, fRim);
    
    if (g_isEnableRimLight)
        Out.vSpecular += g_vLightDiffuse * fRim * g_fRimIntensity;
    
    return Out;
}

PS_OUT_LIGHT PS_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    float4 vWorldPos = Compute_WorldPosition_FromDepth(In.vTexcoord, vDepthDesc.x, vDepthDesc.y);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
    
    float fShade = max(dot(vNormal * -1.f, normalize(g_vLightDir)), 0.f);

    // 엠비언트 따로 계산
    float fAmbient = g_vLightAmbient * g_vMtrlAmbient;
    
    float fLevel = floor(g_fToonShadeLevel);

    // 엠비언트를 제외한 순수한 fShade 값으로 툰 셰이딩
    float fToonShade = ceil(fShade * fLevel) / fLevel;
    
    // 엠비언트 분리 후 합산
    float fLightIntensity = saturate(fShade);
    
    if (g_isEnableSSAO)
    {
        float fAO = g_SSAOTexture.Sample(PointSampler, In.vTexcoord).r;
        fToonShade *= fAO;
        fLightIntensity *= fAO;
        fAmbient *= fAO;
    }
    
    if (g_isEnableToonShade)
        Out.vShade = g_vLightDiffuse * (fToonShade + fAmbient) * fAtt;
    else
        Out.vShade = g_vLightDiffuse * (fLightIntensity + fAmbient) * fAtt;

    
    // Specular
    vector vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    float fSpecularValue = vSpecularDesc.g;
    float fSpecularIntensity = vSpecularDesc.r;
    
    float fShininess = lerp(g_vSpecularPower.x, g_vSpecularPower.y, fSpecularValue);
    
    vector vLook = normalize(vWorldPos - g_vCamPosition);
    vector vReflect = reflect(normalize(vLightDir), vNormal);
    float fSpecularBase = max(dot(normalize(vReflect) * -1.f, vLook), 0.f);
    float fSpecular = pow(fSpecularBase, fShininess);
    
    Out.vSpecular = g_vLightSpecular * fSpecular * fSpecularValue + g_vLightSpecular * fSpecular * fSpecularIntensity * 2.f * fAtt;
    
    if (0.f == vSpecularDesc.a)
        return Out;
    
    // Rim Light
    float fRim = 1.f - saturate(dot(vNormal, vLook * -1.f));
    fRim = pow(fRim, g_fRimPower);
    
    // 빛을 등지고 있을 때
    float fRimFactor = max(dot(vLightDir, vLook * -1.f), 0.f);
    fRim *= fRimFactor;
    
    // Rim Toon
    if (g_isToonLight)
        fRim = step(g_fRimToonThreshold, fRim);
    
    if (g_isEnableRimLight)
        Out.vSpecular += g_vLightDiffuse * fRim * g_fRimIntensity * fAtt;

    return Out;
}

PS_OUT_BACKBUFFER PS_POSTSCENE(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (1.f == vDiffuse.r && 1.f == vDiffuse.g && 1.f == vDiffuse.b && 0.f == vDiffuse.a)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular;
    
    if (!g_isEnableShadow)
        return Out;
    
    // Pixel Depth
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vViewPos = Compute_ViewPosition_FromDepth(In.vTexcoord, vDepthDesc.x, vDepthDesc.y);
    
    float fCameraViewDepth = vViewPos.z;

    if (fCameraViewDepth > g_fSplitFar)
    {
        Out.vColor = lerp(Out.vColor * g_fShadowIntensity, Out.vColor, 1.f);
        return Out;
    }
    
    float4 vWorldPos = Transform_ViewToWorld(vViewPos);
    
    float4 vLightProjPos = Transform_WorldToProj(vWorldPos, g_LightViewMatrix, g_LightProjMatrix);
    vLightProjPos /= vLightProjPos.w;
    
    float2 vTexcoord = Compute_Texcoord_FromProjPos(vLightProjPos.xy);

    float fLightDepth = vLightProjPos.z;
    
    float fShadowSum = 0.f;
    float2 vOffset;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            vOffset.x = j * 1.f / g_vShadowMapSize.x;
            vOffset.y = i * 1.f / g_vShadowMapSize.y;
            float2 vSampleUV = vTexcoord + vOffset;
            
            if (0.f > vSampleUV.x || 1.f < vSampleUV.x || 0.f > vSampleUV.y || 1.f < vSampleUV.y)
                fShadowSum += 1.f;
            else
                fShadowSum += g_ShadowTexture.SampleCmpLevelZero(ComparisonSampler, vSampleUV, fLightDepth - g_fShadowBias);
        }
    }
    
    fShadowSum /= 9.f;
    
    Out.vColor = lerp(Out.vColor * g_fShadowIntensity, Out.vColor, fShadowSum);
    
    return Out;
}

struct PS_OUT_BLUR_X
{
    vector vBlurX : SV_TARGET0;
};

PS_OUT_BLUR_X PS_BRIGHTNESS(PS_IN In)
{
    PS_OUT_BLUR_X Out;

    float4 vEmissive = g_EmissiveTexture.SampleLevel(ClampSampler, In.vTexcoord, 0.f);
    float3 vEmissiveColor = vEmissive.rgb * vEmissive.a;

    float3 vPostSceneColor = g_PostSceneTexture.SampleLevel(ClampSampler, In.vTexcoord, 0.f).rgb;

    float3 vBrightColor = max(vPostSceneColor - 1.f, 0.f);
        
    float3 vCombinedColor = vEmissiveColor + vBrightColor;

    Out.vBlurX = float4(vCombinedColor, 1.f);
    
    return Out;
}

PS_OUT_BLUR_X PS_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;

    float2 vTexcoord;
    float3 vColor = float3(0.f, 0.f, 0.f);;

    for (int i = -g_iWeightRadius; i < g_iWeightRadius + 1; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + i / g_fViewportWidth;
        vTexcoord.y = In.vTexcoord.y;

        float4 vBrightColor = g_BrightTexture.Sample(ClampSampler, vTexcoord);
        
        vColor += g_Weights[i + g_iWeightRadius] * vBrightColor;
    }

    Out.vBlurX = float4(vColor / g_fBlurNormalization, 1.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_BLUR_Y(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    float3 vColor = float3(0.f, 0.f, 0.f);
    
    for (int i = -g_iWeightRadius; i < g_iWeightRadius + 1; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + i / g_fViewportHeight;
        
        float4 vBrightColor = g_BlurXTexture.SampleLevel(ClampSampler, vTexcoord, 0.f);
        
        vColor += g_Weights[i + g_iWeightRadius] * vBrightColor.rgb * vBrightColor.a;
    }

    Out.vColor = float4(vColor / g_fBlurNormalization, 1.f);
    
    return Out;
}

struct PS_OUT_SSAO
{
    float4 vSSAO : SV_TARGET0;
};

PS_OUT_SSAO PS_SSAO(PS_IN In)
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
    float3x3 ViewMatrix = float3x3(g_CamViewMatrix._11_12_13, g_CamViewMatrix._21_22_23, g_CamViewMatrix._31_32_33);
    vNormal = normalize(mul(vNormal.xyz, ViewMatrix));
    
    // TBN
    float3 vTangent = normalize(vRandomVector - vNormal * dot(vRandomVector, vNormal));
    float3 vBinormal = normalize(cross(vNormal, vTangent));
    float3x3 TBNMatrix = float3x3(vTangent, vBinormal, vNormal);
    
    // Depth
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    float4 vViewPos = Compute_ViewPosition_FromDepth(In.vTexcoord, vDepthDesc.x, vDepthDesc.y);
    
    // Occlusion
    float fOcclusion = 0.f;
 
    [loop]
    for (uint i = 0; i < g_iNumKernels; ++i)
    {
        // Kernle Vector Rotation -> View Space Dir
        float3 vSampleDir = mul(g_Kernels[i], TBNMatrix);
        
        // View Sample Position
        float3 vSamplePos = vViewPos.xyz + vSampleDir * g_fSSAORadius;
        
        // Sample Position -> UV
        float4 vProjSamplePos = mul(float4(vSamplePos, 1.f), g_CamProjMatrix);
        vProjSamplePos /= vProjSamplePos.w;

        float2 vSampleTexcoord = Compute_Texcoord_FromProjPos(vProjSamplePos.xy);
        
        float fSampleDepth = g_DepthTexture.Sample(PointSampler, vSampleTexcoord).y;

        float fRangeLerp = smoothstep(0.f, 1.f, g_fSSAORadius / abs(vViewPos.z - fSampleDepth));

        fOcclusion += (vSamplePos.z >= fSampleDepth ? 1.f : 0.f) * fRangeLerp;
    }

    fOcclusion = 1.f - (fOcclusion / (float)g_iNumKernels);
    
    float fAO = pow(saturate(fOcclusion * g_fSSAOIntensity), g_fSSAOContrast);
    
    Out.vSSAO = float4(fAO, fAO, fAO, 1.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vPostSceneDesc = g_PostSceneTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vBloomDesc = g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vFogDesc = g_FogTexture.Sample(DefaultSampler, In.vTexcoord);

    if (1.f == vPostSceneDesc.r && 1.f == vPostSceneDesc.g && 1.f == vPostSceneDesc.b && 0.f == vPostSceneDesc.a)
        discard;

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

        // 0 ~ 1
        bool isOutline = (vDepthDesc.x >= 1.f - g_fOutlineBias && vOutlineDesc.w <= 1.f);

        if (isOutline)
            vFinalColor *= float4(vOutlineDesc.rgb, g_fOutlineAlpha);
    }
    
    // Vignette
    if (g_isEnableVignette)
    {
        float fDistance = length(In.vTexcoord - 0.5f);
        float fVignetteFactor = 1.f - pow(fDistance, g_fVignettePower) * g_fVignetteIntensity;

        vFinalColor.rgb = lerp(g_vVignetteColor, vFinalColor.rgb, fVignetteFactor);
        vFinalColor.a = vPostSceneDesc.a;
    }
    
    Out.vColor = vFinalColor;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_FOG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vPostSceneDesc = g_PostSceneTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    float4 vViewPos = Compute_ViewPosition_FromDepth(In.vTexcoord, vDepthDesc.x, vDepthDesc.y);
    float fViewZ = vViewPos.z;
    
    float4 vWorldPos = Transform_ViewToWorld(vViewPos);
    
    // Linear
    float fLinear = saturate((fViewZ - g_fFogNear) / (g_fFogFar - g_fFogNear));
    
    // Exponential -> (1 - e^{-rho * d})
    float fDistance = abs(fViewZ);
    float fExp = saturate(1.f - exp(-g_fFogDensity * fDistance));
    
    // Exponential^2 -> (1 - e^{-(rho * d)^2})
    float fOpticalDepth = g_fFogDensity * fDistance;
    float fExpSquare = saturate(1.f - exp(-(fOpticalDepth * fOpticalDepth)));
    
    float4 vResultColor;
    float fFogFactor = 0.f;
        
    if (0 == g_iFogMode)
        fFogFactor = fLinear;
    else if (1 == g_iFogMode)
        fFogFactor = fExp;
    else if (2 == g_iFogMode)
        fFogFactor = fExpSquare;

    if (true == g_isEnableFogNoise)
    {
        float2 vNoiseTexcoord;
        
        vNoiseTexcoord = In.vTexcoord * g_vNoiseScale;
        vNoiseTexcoord.x += g_fTimeDelta * g_vNoiseSpeed.x;
        vNoiseTexcoord.y += g_fTimeDelta * g_vNoiseSpeed.y;
        
        float fNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseTexcoord).r;
        fNoise = pow(fNoise, g_fNoiseContrast);
        
        fFogFactor = lerp(fFogFactor, fFogFactor * fNoise, g_fNoiseStrength);
    }
    
    fFogFactor = clamp(fFogFactor, 0.f, g_fFogBias);
    
    if (true == g_isUseHeightFog)
    {
        // Height 계산
        float fFogDiff = vWorldPos.y - g_fFogBaseHeight;
        fFogDiff = max(fFogDiff, 0.f);
        float fFogHeightFactor = saturate(exp(-fFogDiff * g_fFogHeightDensity));
        
        vResultColor = lerp(vPostSceneDesc, g_vFogColor, fFogFactor * fFogHeightFactor);
    }
    else
    {
        vResultColor = lerp(vPostSceneDesc, g_vFogColor, fFogFactor);
    }
    
    Out.vColor = vResultColor;

    return Out;
}

PS_OUT_BACKBUFFER PS_DISTORTION(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    float4 vFinalColor;
    
    if (g_isEnableDistortion)
    {
        float2 vNoiseUV = In.vTexcoord;
        vNoiseUV += g_fTimeDelta * g_fDistortionSpeed;
        
        float2 vNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV).rg * 2.f - 1.f;

        float4 vCenterPos = Transform_WorldToProj(float4(g_vWorldCenterPos, 1.f), g_CamViewMatrix, g_CamProjMatrix);

        float2 vCenterUV = Compute_Texcoord_FromProjPos(vCenterPos.xy);
        
        float2 vDir = In.vTexcoord - vCenterUV;

        vDir.x *= g_fAspect;
        
        float fDistance = length(vDir) / g_fDistortionRange;

        float fMask = pow(saturate(1.f - fDistance), 2.f);
        
        float2 vDistortionUV = In.vTexcoord + vNoise * g_fDistortionPower * fMask;
        vFinalColor = g_CombinedTexture.Sample(DefaultSampler, vDistortionUV);
    }
    else
        vFinalColor = g_CombinedTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vColor = vFinalColor;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_LUT(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    // 컴바인드 텍스처 읽기
    float3 vSceneColor = g_CombinedTexture.Sample(ClampSampler, In.vTexcoord).rgb;
    float3 vFinalColor = float3(0.f, 0.f, 0.f);
    
    if (true == g_isEnableLUT)
    {
        // LUT 텍스처 좌표 계산
    
        // LUT 한 슬라이스의 크기
        float fSliceIndex = vSceneColor.b * (g_iLUTSliceSize - 1.f);
        int iSliceLow = (int) floor(fSliceIndex);
        int iSliceHigh = min(iSliceLow + 1, g_iLUTSliceSize - 1);
    
        // 두 슬라이스 간 보간 비율
        float fSliceBlend = fSliceIndex - iSliceLow;
    
        // 현재 슬라이스 내부 좌표 계산
        float fLutX = vSceneColor.r * (float) (g_iLUTSliceSize - 1);
        float fLutY = vSceneColor.g * (float) (g_iLUTSliceSize - 1);
    
        // LUT 텍스처 상에서 실제 픽셀 위치 계산
        float fSliceLowPixelX = (float) iSliceLow * (float) g_iLUTSliceSize + fLutX;
        float fSliceHighPixelX = (float) iSliceHigh * (float) g_iLUTSliceSize + fLutX;
        float fSlicePixelY = fLutY;
    
        // 픽셀 좌표 -> 정규화된 UV 좌표로 변환 (텍셀 중앙 맞춤)
        float vLowUV = float2((fSliceLowPixelX + 0.5f) / g_vLUTTextureSize.x,
                           (fSlicePixelY + 0.5f) / g_vLUTTextureSize.y);
        float vHighUV = float2((fSliceHighPixelX + 0.5f) / g_vLUTTextureSize.x,
                           (fSlicePixelY + 0.5f) / g_vLUTTextureSize.y);
    
        // 두 슬라이스에서 샘플링
        float3 vLowColor = g_LUTTexture.Sample(PointClampSampler, vLowUV).rgb;
        float3 vHighColor = g_LUTTexture.Sample(PointClampSampler, vHighUV).rgb;
    
        // 두 색을 보간
        float3 vLUTColor = lerp(vLowColor, vHighColor, fSliceBlend);
    
        // LUT 강도만큼 원본과 보정된 색 사이를 보간
        vFinalColor = lerp(vSceneColor, vLUTColor, saturate(g_fLUTIntensity));
    }
    else
        vFinalColor = vSceneColor;
    
    // 최종 출력 시 감마 보정(일단 주석) -> Linear -> sRGB
    //  vFinalColor = pow(vFinalColor, 1.f / 2.2f);

    Out.vColor = float4(vFinalColor, 1.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_RADIAL_BLUR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float3 vSceneColor = g_CombinedTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    float3 vFinalColor = float3(0.f, 0.f, 0.f);

    if (true == g_isEnableRadialBlur)
    {
        float2 vDir = normalize(g_vRadialBlurCenterUV - In.vTexcoord);
    
        // Mask
        float fDist = length(In.vTexcoord - g_vRadialBlurCenterUV);
        float fMask = smoothstep(g_vRadialBlurMaskRadius.x, g_vRadialBlurMaskRadius.y, fDist);
    
        // 곡선 강화
        fMask = pow(fMask, g_fRadialBlurExp);
    
        float3 vColor = float3(0.f, 0.f, 0.f);
        float fWeightAcc = 0.f;
    
        for (uint i = 0; i < g_iNumRadialBlurSamples; ++i)
        {
            float fRatio = (float) (i + 1) / (float) g_iNumRadialBlurSamples;
            float2 vSampleUV = In.vTexcoord + vDir * fRatio * g_fRadialBlurRadius;
            float fWeight = pow(fRatio, g_fRadialBlurAtt);
            vColor += g_CombinedTexture.Sample(ClampSampler, vSampleUV) * fWeight;
            fWeightAcc += fWeight;
        }

        vFinalColor = lerp(vSceneColor, vColor / max(fWeightAcc, 1e-6), fMask * g_fRadialBlurStrength); // 10^-6
    }
    else
    {
        vFinalColor = vSceneColor;
    }

    Out.vColor = float4(vFinalColor, 1.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MOTION_BLUR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float3 vSceneColor = g_CombinedTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    float3 vFinalColor = float3(0.f, 0.f, 0.f);
    
    // 벨로시티 맵 샘플링
    float4 vVelocityDesc = g_VelocityTexture.Sample(DefaultSampler, In.vTexcoord);
    float2 vMotionVector = vVelocityDesc.rg;
    float fCurDepth = g_VelocityTexture.Sample(DefaultSampler, In.vTexcoord).b;

    // 모션 벡터 길이 계산 및 임계값 설정 (현재 Dynamic 기록 안 되는 문제로 임시 예외 처리)
    float fMotionLength = length(vMotionVector);
    float fThreshold = 0.0001f;
    
    if (g_isEnableMotionBlur && (fMotionLength > fThreshold))
    {    
        float fSampleCount = 0.f;
    
        [loop]
        for (uint i = 0; i < g_iNumMotionBlurSamples; ++i)
        {
            // Ratio, Sample UV, Depth 비교 -> 경계선 처리, 색 누적, 샘플 개수 누적
            float fRatio = (float) i / (float) (g_iNumMotionBlurSamples - 1);
            float2 vSampleUV = In.vTexcoord - vMotionVector * fRatio;
        
            float fSampleDepth = g_DepthTexture.Sample(PointSampler, vSampleUV).x;
        
            //  if (abs(fSampleDepth - fCurDepth) > g_fMotionBlurBias)
            //      break;
        
            vFinalColor += g_CombinedTexture.Sample(ClampSampler, vSampleUV).rgb;
            fSampleCount += 1.f;
        }

        vFinalColor = lerp(vSceneColor, vFinalColor / max(fSampleCount, 1e-6), g_fMotionBlurStrength);
    }
    else
        vFinalColor = vSceneColor;
    
    Out.vColor = float4(vFinalColor, 1.f);

    return Out;
}

PS_OUT_BACKBUFFER PS_STATIC_VELOCITY(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fCurDepth = vDepthDesc.x;
    
    if (fCurDepth >= 1.f)
    {
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }

    float4 vWorldPos = Compute_WorldPosition_FromDepth(In.vTexcoord, vDepthDesc.x, vDepthDesc.y);
    
    // 월드 포지션 -> 이전 뷰 투영 행렬 스페이스로 변환
    float4 vPrevPos = Transform_WorldToProj(vWorldPos, g_PrevViewMatrix, g_PrevProjMatrix);
    
    // Prev UV = Texcoord 범위로 변환 (-1 ~ 1 -> 0 ~ 1)
    float2 vPrevUV = Compute_Texcoord_FromProjPos(vPrevPos.xy);
    
    // 현재 프레임의 UV 좌표와의 차이 계산
    float2 vCurUV = In.vTexcoord;
    float2 vMotionVector = vCurUV - vPrevUV;
    
    Out.vColor = float4(vMotionVector.x, vMotionVector.y, fCurDepth, 1.f);

    return Out;
}

PS_OUT_WEIGHTBLEND PS_WEIGHT_BLEND(PS_IN In)
{
   PS_OUT_WEIGHTBLEND Out;
   
   //float4 vColorAccDesc = g_AccumColorTexture.Sample(DefaultSampler, In.vTexcoord);
   //float vAlphaAccDesc = g_AccumAlphaTexture.Sample(DefaultSampler, In.vTexcoord).r;
   //Out.vBackBufferColor = vColorAccDesc / (vAlphaAccDesc + 1e-5);
   //Out.vBackBufferColor.a = saturate(vAlphaAccDesc); //클램핑 
   
   
   float4 vColorAccDesc = g_AccumColorTexture.Sample(DefaultSampler, In.vTexcoord);
   float vAlphaAccDesc = g_AccumAlphaTexture.Sample(DefaultSampler, In.vTexcoord).r;
   Out.vBackBufferColor = vColorAccDesc / (vColorAccDesc.a + 1e-5);
   Out.vBackBufferColor.a = 1.f - saturate(vAlphaAccDesc); 
   
   return Out;
}

technique11 DefaultTechnique
{
    pass Debug // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DEBUG();
    }

    pass Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DIRECTIONAL();
    }

    pass Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POINT();
    }

    pass PostScene // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POSTSCENE();
    }

    pass BlurX // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_X();
    }

    pass BlurY // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_Y();
    }

    pass DebugArray // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DEBUG_ARRAY();
    }

    pass SSAO // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO();
    }

    pass Combined // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COMBINED();
    }

    pass Fog // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FOG();
    }

    pass Distortion // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass LUT // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LUT();
    }

    pass RadialBlur // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RADIAL_BLUR();
    }

    pass MotionBlur // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOTION_BLUR();
    }

    pass StaticVelocity // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STATIC_VELOCITY();
    }

    pass Brightness // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRIGHTNESS();
    }

    pass WeightBlend // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WEIGHT_BLEND();
    }
}
