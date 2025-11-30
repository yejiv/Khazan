#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_WorldMatrixInv, g_ViewMatrixInv, g_ProjMatrixInv;

Texture2D g_DiffuseTexture, g_DepthTexture, g_NormalTexture, g_DecalTexture;

float2 g_vScreenSize;
float3 g_vDecalColor;

float g_fOpacity;
uint g_iRandSeed;

float g_fThreshold;

// Emissive
float3 g_vBaseColor, g_vEmissiveColor, g_vBorderColor;
float g_fEmissiveMaskPower, g_fEmissiveIntensity;

struct VS_IN
{
    float3 vPosition : POSITION;    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    // 투영 공간 -> PS에서 화면상의 좌표까지 변환
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 화면상 위치를 텍스쿠드로 변환
    float2 vTexcoord;
    vTexcoord.x = In.vPosition.x / g_vScreenSize.x;
    vTexcoord.y = In.vPosition.y / g_vScreenSize.y;
    
    // 위에서 구한 텍스쿠드로 깊이 값 읽어오기
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    
    if (vDepthDesc.x >= 1.f)
        discard;
    
    // 깊이로 월드 포지션 복원
    float4 vWorldPos;

    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * vDepthDesc.y;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // 디퓨즈 텍스처를 샘플링
    float4 vDiffuseDesc = g_DiffuseTexture.Sample(ClampSampler, vTexcoord);

    if (1.f == vDiffuseDesc.r && 1.f == vDiffuseDesc.g && 1.f == vDiffuseDesc.b && 0.f == vDiffuseDesc.a)
        discard;
    
    float4 vFinalColor = vDiffuseDesc;
    float fFinalAlpha = 0.f;
    
    // 월드를 현재 데칼의 로컬 공간으로 변환
    float4 vLocalPos = mul(vWorldPos, g_WorldMatrixInv);
        
    if (any(abs(vLocalPos.xyz) > 0.5f))
        return Out;
        
    // 노말 텍스처 읽기
    float3 vNormal = g_NormalTexture.Sample(DefaultSampler, vTexcoord).xyz;
    vNormal = abs(vNormal);
        
    // 각 픽셀마다 노말을 확인
    float2 vDecalTexcoord;
        
    // Y축이 클 때
    if (vNormal.y > vNormal.x && vNormal.y > vNormal.z)
        vDecalTexcoord = vLocalPos.xz;
        
    // X축이 클 때
    else if (vNormal.x > vNormal.y && vNormal.x > vNormal.z)
        vDecalTexcoord = vLocalPos.zy;
        
    // Z축이 클 때
    else if (vNormal.z > vNormal.y && vNormal.z > vNormal.x)
        vDecalTexcoord = vLocalPos.xy;
    else
        vDecalTexcoord = vLocalPos.zy;
        
    // 로컬 좌표 -0.5 0.5 를 데칼 UV 좌표 0, 1 범위로 변환
    vDecalTexcoord += 0.5f;

    //  데칼 텍스처를 샘플링
    float4 vDecalDesc = g_DecalTexture.Sample(BorderSampler, vDecalTexcoord);

    float fMask = 1.f;
        
    float fThresholdR = 1.f / 3.f;
    float fThresholdG = 2.f / 3.f;
    float fRandNum = rand_between(0.f, 1.f, g_iRandSeed);
        
    if (fRandNum < fThresholdR)
        fMask = vDecalDesc.r;
    else if (fRandNum < fThresholdG)
        fMask = vDecalDesc.g;
    else
        fMask = vDecalDesc.b;

    if (fMask > 0.f)
        fFinalAlpha = g_fOpacity * 1.f;
    else
        fFinalAlpha = g_fOpacity * 0.f;
    
    float3 vTargetColor = g_vDecalColor * 0.3f;
    
    Out.vColor.rgb = lerp(vTargetColor, g_vDecalColor, g_fOpacity);
    Out.vColor.a = fFinalAlpha;
   
    return Out;
}

PS_OUT PS_MAP_DECAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 화면상 위치를 텍스쿠드로 변환
    float2 vTexcoord;
    vTexcoord.x = In.vPosition.x / g_vScreenSize.x;
    vTexcoord.y = In.vPosition.y / g_vScreenSize.y;
    
    // 위에서 구한 텍스쿠드로 깊이 값 읽어오기
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    
    if (1.f <= vDepthDesc.x)
        discard;
    
    // 깊이로 월드 포지션 복원
    float4 vWorldPos;

    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * vDepthDesc.y;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // 디퓨즈 텍스처를 샘플링
    float4 vDiffuseDesc = g_DiffuseTexture.Sample(ClampSampler, vTexcoord);

    if (1.f == vDiffuseDesc.r && 1.f == vDiffuseDesc.g && 1.f == vDiffuseDesc.b && 0.f == vDiffuseDesc.a)
        discard;
    
    float4 vFinalColor = vDiffuseDesc;
    float fFinalAlpha = 0.f;
    
    // 월드를 현재 데칼의 로컬 공간으로 변환
    float4 vLocalPos = mul(vWorldPos, g_WorldMatrixInv);
        
    if (any(abs(vLocalPos.xyz) > 0.5f))
        return Out;
        
    // 노말 텍스처 읽기
    float3 vNormal = g_NormalTexture.Sample(DefaultSampler, vTexcoord).xyz;
    vNormal = abs(vNormal);
        
    // 각 픽셀마다 노말을 확인
    float2 vDecalTexcoord;
        
    // Y축이 클 때
    if (vNormal.y > vNormal.x && vNormal.y > vNormal.z)
        vDecalTexcoord = vLocalPos.xz;
        
    // X축이 클 때
    else if (vNormal.x > vNormal.y && vNormal.x > vNormal.z)
        vDecalTexcoord = vLocalPos.zy;
        
    // Z축이 클 때
    else if (vNormal.z > vNormal.y && vNormal.z > vNormal.x)
        vDecalTexcoord = vLocalPos.xy;
    else
        vDecalTexcoord = vLocalPos.zy;
        
    // 로컬 좌표 -0.5 0.5 를 데칼 UV 좌표 0, 1 범위로 변환
    vDecalTexcoord += 0.5f;

    // 데칼 텍스처를 샘플링
    float4 vDecalDesc = g_DecalTexture.Sample(ClampSampler, vDecalTexcoord);

    float fMask = 1.f;

    // 마스크 R, G, B 채널 중 랜덤으로 하나 선택
    float fThresholdR = 0.f;
    float fThresholdG = 0.5f;
    float fThresholdB = 1.f;
        
    if (g_fThreshold == fThresholdR)
        fMask = vDecalDesc.r;
    else if (g_fThreshold == fThresholdG)
        fMask = vDecalDesc.g;
    else if (g_fThreshold == fThresholdB)
        fMask = vDecalDesc.b;

    Out.vColor.rgb = g_vDecalColor;
    Out.vColor.a = fMask;
   
    return Out;
}

PS_OUT PS_MAP_DECAL_WIREFRAME(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = float4(0.f, 1.0f, 1.0f, 1.f);
   
    return Out;
}

PS_OUT PS_EMISSIVE_DECAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 화면상 위치를 텍스쿠드로 변환
    float2 vTexcoord;
    vTexcoord.x = In.vPosition.x / g_vScreenSize.x;
    vTexcoord.y = In.vPosition.y / g_vScreenSize.y;
    
    // 위에서 구한 텍스쿠드로 깊이 값 읽어오기
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    
    if (vDepthDesc.x >= 1.f)
        discard;
    
    // 깊이로 월드 포지션 복원
    float4 vWorldPos;

    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * vDepthDesc.y;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // 디퓨즈 텍스처를 샘플링
    float4 vDiffuseDesc = g_DiffuseTexture.Sample(ClampSampler, vTexcoord);

    if (1.f == vDiffuseDesc.r && 1.f == vDiffuseDesc.g && 1.f == vDiffuseDesc.b && 0.f == vDiffuseDesc.a)
        discard;
    
    float3 vFinalColor = vDiffuseDesc.rgb;
    float fFinalAlpha = 0.f;
    
    // 월드를 현재 데칼의 로컬 공간으로 변환
    float4 vLocalPos = mul(vWorldPos, g_WorldMatrixInv);
        
    if (any(abs(vLocalPos.xyz) > 0.5f))
        return Out;
        
    // 노말 텍스처 읽기
    float3 vNormal = g_NormalTexture.Sample(DefaultSampler, vTexcoord).xyz;
    vNormal = abs(vNormal);
        
    // 각 픽셀마다 노말을 확인
    float2 vDecalTexcoord;
        
    // Y축이 클 때
    if (vNormal.y > vNormal.x && vNormal.y > vNormal.z)
        vDecalTexcoord = vLocalPos.xz;
        
    // X축이 클 때
    else if (vNormal.x > vNormal.y && vNormal.x > vNormal.z)
        vDecalTexcoord = vLocalPos.zy;
        
    // Z축이 클 때
    else if (vNormal.z > vNormal.y && vNormal.z > vNormal.x)
        vDecalTexcoord = vLocalPos.xy;
    else
        vDecalTexcoord = vLocalPos.zy;
        
    // 로컬 좌표 -0.5 0.5 를 데칼 UV 좌표 0, 1 범위로 변환
    vDecalTexcoord += 0.5f;

    //  데칼 텍스처를 샘플링
    float4 vDecalDesc = g_DecalTexture.Sample(ClampSampler, vDecalTexcoord);

    // 색상 혼합
    // R채널 - 일반 빨강
    // G채널 - 이미시브 빨강
    // B채널 - 검정
    // 최종 RGB 색상은 각 채널의 기여분을 모두 더함
    // 최종 알파도 각 채널의 기여분을 모두 더함
    // 위는 컬러를 더하고 알파는 각 채널 값을 더함 -> 기여분(RGB 중 하나라도 데이터가 조금이라도 있으면 그리기 위함)
    // 알파는 마스크가 1을 넘지 않도록 saturate, 전역 투명도를 g_fOpacity로 곱하여 적용
    
    // g 채널을 이미시브 마스크로 사용 -> 제곱 값으로 사용하여 낮은 값은 더 낮추고 높은 값만 남김
    // g 채널이 1인 부분은 그대로 강하게 발광
    // 3~5 사이의 값으로 조절
    // 기존 vColorEmissive 식에서 -> 컬러 * 인텐시티 * 이미시브 마스크
    
    float fEmissiveMask = pow(vDecalDesc.g, g_fEmissiveMaskPower);
    
    float3 vBaseColor = g_vBaseColor * vDecalDesc.r;
    float3 vEmissiveColor = g_vEmissiveColor * g_fEmissiveIntensity * fEmissiveMask;
    float3 vBorderColor = g_vBorderColor * vDecalDesc.b;
    
    vFinalColor.rgb = vBaseColor + vEmissiveColor + vBorderColor;
    
    fFinalAlpha = vDecalDesc.r + vDecalDesc.g + vDecalDesc.b;
    fFinalAlpha = saturate(fFinalAlpha) * g_fOpacity * vDecalDesc.a;
    
    Out.vColor = float4(vFinalColor.rgb, fFinalAlpha);

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthTestOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Map_Decal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthTestOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP_DECAL();
    }

    pass Map_WireFrame
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP_DECAL_WIREFRAME();
    }

    pass Emissive_Decal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthTestOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EMISSIVE_DECAL();
    }
}