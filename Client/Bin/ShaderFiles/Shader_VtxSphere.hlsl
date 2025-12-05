#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;

// 구름
texture2D g_GradationTexture;
texture2D g_LookUpTexture;
texture2D g_NormalTexture;
texture2D g_DistortionTexture;

// 하늘
texture2D g_NebulaTexture;
texture2D g_StarMaskTexture;
texture2D g_MoonTexture;
texture2D g_RingTexture;

// 구름
float g_fCloudDensity;
float g_fCloudLightIntensity;
float g_fCloudScale;
float g_fCloudSpeed;
float g_isDynamic;
float3 g_vCloudColor;
float3 g_vLightDir;

// 하늘
float3 g_vNebulaColorR = float3(0.5f, 0.5f, 0.5f);
float3 g_vNebulaColorG = float3(0.5f, 0.5f, 0.5f);
float3 g_vNebulaColorB = float3(0.5f, 0.5f, 0.5f);
float g_fStarStrength = 1.5f;
float g_fMoonSize = 0.35f;
float3 g_vMoonDirection = float3(-0.6f, 0.5f, 1.f);
float3 g_vMoonColor = float3(0.5f, 0.5f, 0.5f);
float g_fMoonIntensity = 1.f;

float g_fTime;
float4 g_vCamPosition;
float4 g_vColor = { 1.f, 1.f, 1.f, 1.f };

float g_fPI = 3.1415926f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
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
    
    /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
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
    float4 vColor : SV_TARGET0;
    
    
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_SKY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // --------------------------------------------
    // 1. Direction / UV 계산
    // --------------------------------------------
    float3 vDir = normalize(In.vWorldPos.xyz - g_vCamPosition.xyz);

    float2 uv;
    uv.x = atan2(vDir.x, vDir.z) * (1.f / (2.f * g_fPI)) + 0.5f;
    uv.y = saturate(vDir.y * 0.5f + 0.5f);

    // --------------------------------------------
    // 2. 텍스처 없는 3단 Sky Gradient
    //    R=Upper, G=Middle, B=Lower
    // --------------------------------------------
    float t = saturate(vDir.y * 0.5f + 0.5f); // -1~1 → 0~1

    float3 colUpper = g_vNebulaColorR; // Zenith
    float3 colMiddle = g_vNebulaColorG; // Middle layer
    float3 colLower = g_vNebulaColorB; // Horizon/Lower

    // Lower → Middle
    float3 lowMid = lerp(colLower, colMiddle, smoothstep(0.0f, 0.50f, t));

    // Middle → Upper
    float3 vSkyColor = lerp(lowMid, colUpper, smoothstep(0.50f, 1.0f, t));

    // --------------------------------------------
    // 3. 별(SkyStars)
    // --------------------------------------------
    float3 vStars = g_StarMaskTexture.Sample(DefaultSampler, uv * 4.f).rgb;
    vStars = pow(vStars, 10.0f) * g_fStarStrength;

    // --------------------------------------------
    // 4. Moon Direction & UV
    // --------------------------------------------
    float3 vMoonDir = normalize(g_vMoonDirection);
    float fMoonDot = dot(vDir, vMoonDir);
    float moonVisible = step(0.f, fMoonDot); // 정면만

    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vMoonDir));
    float3 vUp = cross(vMoonDir, vRight);

    float2 moonUV;
    moonUV.x = dot(vDir, vRight);
    moonUV.y = dot(vDir, vUp);
    moonUV = moonUV / g_fMoonSize + 0.5f;
    moonUV.y = 1.0f - moonUV.y;

    float2 moonCenter = float2(0.5f, 0.5f);
    float fDist = distance(moonUV, moonCenter);

    float fOuter = 0.48f;
    float fInner = fOuter - (0.15f * saturate(g_fMoonSize));
    float fMoonMask = 1.0f - smoothstep(fInner, fOuter, fDist);

    // --------------------------------------------
    // 5. Moon Texture → Luma Only
    // --------------------------------------------
    float3 vMoonTex = g_MoonTexture.Sample(DefaultSampler, moonUV).rgb;
    float fLuma = dot(vMoonTex, float3(0.299f, 0.587f, 0.114f));

    float3 vMoonRGB = fLuma.xxx * g_vMoonColor * g_fMoonIntensity;

    // --------------------------------------------
    // 6. Moon 중심 Black Hole
    // --------------------------------------------
    float fBlackHoleRadius = 0.11f;
    float fBlackHoleFeather = 0.02f;

    float fHoleMask = 1.0f - smoothstep(fBlackHoleRadius,
                                        fBlackHoleRadius + fBlackHoleFeather,
                                        fDist);

    vMoonRGB = lerp(vMoonRGB, float3(0, 0, 0), fHoleMask);

    // moon front face only
    vMoonRGB *= moonVisible;
    fMoonMask *= moonVisible;

    // --------------------------------------------
    // 7. 별에서 Moon 부분은 제거
    // --------------------------------------------
    vStars *= (1.0f - fMoonMask);

    // --------------------------------------------
    // 8. Sky + Stars + Moon 합성
    // --------------------------------------------
    float3 vBaseColor = vSkyColor + vStars;

    float3 vFinal = 1.0f - (1.0f - vBaseColor) * (1.0f - vMoonRGB * fMoonMask);

    Out.vColor = float4(saturate(vFinal), 1.0f);
    return Out;
}

float3 SafeUp(float3 n)
{
    return (abs(n.y) < 0.99f) ? float3(0.f, 1.f, 0.f) : float3(0.f, 0.f, 1.f);
}

void BuildSphericalTBN(float3 n, out float3 T, out float3 B, out float3 N)
{
    N = normalize(n);
    float3 up = SafeUp(n);
    T = normalize(cross(up, N));
    B = cross(N, T);
}

float2 WrapU(float2 uv)
{
    uv.x = uv.x - floor(uv.x);
    return uv;
}

float4 SampleSeamless2D(Texture2D tex, SamplerState samp, float2 uv, float seamWidth)
{
    float u = uv.x;
    float wL = 1.0 - smoothstep(0.0, seamWidth, u);
    float wR = 1.0 - smoothstep(0.0, seamWidth, 1.0 - u);
    float wC = saturate(1.0 - (wL + wR));
    float4 cC = tex.Sample(samp, uv);
    float4 cL = tex.Sample(samp, float2(u + 1.0, uv.y));
    float4 cR = tex.Sample(samp, float2(u - 1.0, uv.y));
    return cC * wC + cL * wL + cR * wR;
}

PS_OUT PS_CLOUD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 vDir = normalize(In.vWorldPos.xyz - g_vCamPosition.xyz);

    float2 uv;
    uv.x = atan2(vDir.x, vDir.z) * 0.159155f + 0.5f;
    uv.y = saturate(vDir.y * 0.5f + 0.5f);
    uv = WrapU(uv);

    uv.x = WrapU(float2(uv.x + g_fTime * g_fCloudSpeed * 0.02f * g_isDynamic, 0)).x;

    float polar = 1.0f - abs(uv.y * 2.0f - 1.0f);
    float2 dUV = (g_DistortionTexture.Sample(SkySampler, uv * 0.5f).rg - 0.5f)
               * (0.035f * polar * polar);

    uv.x = WrapU(float2(uv.x + dUV.x * g_isDynamic, 0)).x;

    float seamW = 0.0025f;
    float4 vCloud = SampleSeamless2D(g_LookUpTexture, SkySampler, uv * g_fCloudScale, seamW);

    float gradRaw = g_GradationTexture.Sample(ClampSampler, float2(0.5f, uv.y)).r;
    float fGrad = pow(gradRaw, 1.2f);

    float3 T, B, N;
    BuildSphericalTBN(vDir, T, B, N);

    float3 nTS = SampleSeamless2D(g_NormalTexture, SkySampler, uv, seamW).xyz * 2.0f - 1.0f;
    float3 nW = normalize(T * nTS.x + B * nTS.y + N * nTS.z);

    float3 L = normalize(g_vLightDir);
    float fLight = saturate(dot(L, nW));
    fLight = lerp(0.35f, 1.0f, fLight);

    float mask = (vCloud.r + vCloud.g + vCloud.b) * (1.0f / 3.0f);
    float fAlpha = saturate((mask + 0.12f) * fGrad * g_fCloudDensity);

    // 기존 위 → 아래 페이드 유지
    float skyY = saturate((uv.y - 0.5f) / 0.5f);
    float fVerticalFade = 1.0f - smoothstep(0.0f, 0.8f, skyY);
    fAlpha *= fVerticalFade;

    // ------------------------------
    // 새로운 seam 부드러움 추가 (핵심)
    // ------------------------------
    float seamFade = smoothstep(0.0f, 0.15f, uv.y);
    fAlpha *= seamFade;
    // ------------------------------

    float3 vColor = g_vCloudColor * vCloud.rgb * fGrad * fLight * g_fCloudLightIntensity;
    Out.vColor = float4(vColor, fAlpha);

    return Out;
}

/*
PS_OUT PS_CLOUD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 카메라 기준 방향 벡터 (정규화)
    float3 vDir = normalize(In.vWorldPos.xyz - g_vCamPosition.xyz);

    // 반구 UV 계산
    float u = atan2(vDir.x, vDir.z) * (1.f / (2.f * g_fPI)) + 0.5f; // 수평 회전
    float v = saturate(In.vNormal.y * 0.5f + 0.5f); // 세로 방향 (아래=0, 위=1)
    float2 uv = float2(frac(u), v);

    // 시간 이동 (수평 흐름)
    uv.x = frac(uv.x + g_fTime * g_fCloudSpeed * 0.02f * g_isDynamic);

    // 디스토션 (바람 흔들림)
    float2 vDistort = (g_DistortionTexture.Sample(SkySampler, uv * 0.5f).rg - 0.5f) * 0.08f;
    uv = frac(uv + vDistort * g_isDynamic);

    // 기본 구름 밀도 맵 (LookUp)
    float4 vCloud = g_LookUpTexture.Sample(DefaultSampler, uv * g_fCloudScale);

    // 세로 감쇠 (Gradation)
    float fGrad = pow(g_GradationTexture.Sample(DefaultSampler, float2(0.5f, v)).r, 1.2f);

    // 노멀 및 조명 (광원 반응)
    float3 vNormal = g_NormalTexture.Sample(DefaultSampler, uv).xyz * 2.f - 1.f;
    float fLight = saturate(dot(normalize(g_vLightDir), vNormal) * 0.5f + 0.5f);

    // 알파 (밀도 × 감쇠)
    float fAlpha = saturate(vCloud.a * fGrad * g_fCloudDensity);

    // 색상 (기본색 × 밝기 × 조명)
    float3 vColor = g_vCloudColor * (vCloud.rgb * fGrad * fLight) * g_fCloudLightIntensity;

    Out.vColor = float4(vColor, fAlpha);
    
    return Out;
}
*/

technique11 DefaultTechnique
{
    pass Sky
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SkyMesh
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKY();
    }

    pass CloudMesh
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CLOUD();
    }
}
