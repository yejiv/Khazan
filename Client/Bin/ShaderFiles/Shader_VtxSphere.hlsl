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
float3 g_vNebulaColor = float3(0.5f, 0.5f, 0.5f);
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
    
    //Out.vColor = g_LookUpTexture.Sample(DefaultSampler, In.vTexcoord);
    //Out.vColor = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    //Out.vColor = g_DistortionTexture.Sample(DefaultSampler, In.vTexcoord);
    //Out.vColor = g_NebulaTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //Out.vColor *= g_vColor.r;
    
    return Out;
}

PS_OUT PS_SKY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 방향 계산 및 기본 UV
    float3 vDir = normalize(In.vWorldPos.xyz - g_vCamPosition.xyz);

    float2 uv;
    uv.x = atan2(vDir.x, vDir.z) * (1.f / (2.f * g_fPI)) + 0.5f;
    uv.y = saturate(vDir.y * 0.5f + 0.5f);
    
    // 기본 하늘색 + 별
    float3 vSkyColor = g_vNebulaColor;
    float3 vStars = g_StarMaskTexture.Sample(DefaultSampler, uv * 4.f).rgb;
    vStars = pow(vStars, 10.0f) * g_fStarStrength;
    
    // 달 방향 기반 UV 계산
    float3 vMoonDir = normalize(g_vMoonDirection);
    float fMoonDot = dot(vDir, vMoonDir); // 달과 픽셀의 방향 관계
    float moonVisible = step(0.f, fMoonDot); // 달이 보이는 면만 1 (뒤쪽은 0)
    
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vMoonDir));
    float3 vUp = cross(vMoonDir, vRight);

    float2 moonUV;
    moonUV.x = dot(vDir, vRight);
    moonUV.y = dot(vDir, vUp);
    moonUV = moonUV / g_fMoonSize + 0.5f;
    moonUV.y = 1.0f - moonUV.y;
    
    // 달 마스크 (달 외곽 보정 )
    float2 moonCenter = float2(0.5f, 0.5f);
    float fDist = distance(moonUV, moonCenter);

    float fOuter = 0.48f;
    float fInner = fOuter - (0.15f * saturate(g_fMoonSize));
    float fMoonMask = 1.0f - smoothstep(fInner, fOuter, fDist);
    
    // 달 텍스처 (밝기 기반, RGB 얼룩 제거)
    float3 vMoonTex = g_MoonTexture.Sample(DefaultSampler, moonUV).rgb;
    float fLuma = dot(vMoonTex, float3(0.299f, 0.587f, 0.114f));
    float3 vMoonRGB = fLuma.xxx * g_vMoonColor * g_fMoonIntensity;
    
    // 블랙홀 (달 중심부 완전 어둠)
    float fBlackHoleRadius = 0.11f;
    float fBlackHoleFeather = 0.02f;
    float fHoleMask = 1.0f - smoothstep(fBlackHoleRadius, fBlackHoleRadius + fBlackHoleFeather, fDist);

    float3 vBlackHoleColor = float3(0.0f, 0.0f, 0.0f);
    vMoonRGB = lerp(vMoonRGB, vBlackHoleColor, fHoleMask);
    
    // 달 내부엔 별 제거
    vStars *= (1.0f - fMoonMask);

    // 달은 한쪽 면(dot>0)만 보이게
    vMoonRGB *= moonVisible;
    fMoonMask *= moonVisible;
    
    // 스크린 합성 (자연스럽게 달+별+하늘)
    float3 vBaseColor = vSkyColor + vStars;
    float3 vWithMoon = 1.0f - (1.0f - vBaseColor) * (1.0f - vMoonRGB * fMoonMask);
    
    Out.vColor = float4(saturate(vWithMoon), 1.0f);
    
    return Out;
}

PS_OUT PS_CLOUD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    /*
    // UV 이동
    float2 uv = In.vTexcoord * g_fCloudScale;
    uv += g_fTime * g_fCloudSpeed * float2(0.5f, 0.3f) * g_isDynamic;

    // 왜곡
    float2 distortion = (g_DistortionTexture.Sample(DefaultSampler, uv).rg - 0.5f) * 0.05f * g_isDynamic;
    float2 uvDistort = uv + distortion;

    // 클라우드 텍스처
    float4 cloudTex = g_LookUpTexture.Sample(DefaultSampler, uvDistort);

    // 하단 페이드 (Normal.y 기반)
    float heightFactor = saturate(In.vNormal.y * 0.5f + 0.5f);
    float grad = smoothstep(0.05f, 0.55f, heightFactor);
    grad = pow(grad, 1.2f);

    // 노멀과 조명 계산
    float3 normal = g_NormalTexture.Sample(DefaultSampler, uvDistort).xyz * 2 - 1;
    float light = saturate(dot(normalize(g_vLightDir), normal) * 0.5f + 0.5f);

    // 색상과 알파 계산
    float3 color = g_vCloudColor * cloudTex.rgb * grad * light * g_fCloudLightIntensity;
    float alpha = saturate(cloudTex.a * g_fCloudDensity * grad * 0.9f);

    Out.vColor = float4(color, alpha);
    */
    return Out;
}

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
