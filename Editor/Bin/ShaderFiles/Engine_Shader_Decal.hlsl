#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

uint g_iNumActiveDecals;
Texture2D g_DiffuseTexture, g_DepthTexture, g_NormalTexture, g_DecalTexture;
float2 g_vScreenSize;

struct DECAL_PARAMS
{
    matrix vWorldMarixInv;
    float fOpacity;
    float fLifeRatio;
    float fPadding[2];
};

StructuredBuffer<DECAL_PARAMS> g_DecalParams;

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
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    
    // 깊이로 월드 포지션 복원
    vector vWorldPos;

    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * vDepthDesc.y;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // 디퓨즈 텍스처를 샘플링
    vector vDiffuseDesc = g_DiffuseTexture.Sample(ClampSampler, vTexcoord);

    float4 vFinalColor = vDiffuseDesc;
    
    [loop]
    for (uint i = 0; i < g_iNumActiveDecals; ++i)
    {
        // 월드를 현재 데칼의 로컬 공간으로 변환
        vector vLocalPos = mul(g_DecalParams[i].vWorldMarixInv, vWorldPos);
        
        if (any(abs(vLocalPos.xyz) > 0.5f))
            continue;
        
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
        vector vDecalDesc = g_DecalTexture.Sample(ClampSampler, vDecalTexcoord);

        float fMask = vDecalDesc.r;
        float3 vBloodColor = { 0.67f, 0.08f, 0.08f };

        vFinalColor.rgb = lerp(vDiffuseDesc.rgb, vBloodColor, fMask);
        vFinalColor.a = g_DecalParams[i].fOpacity * fMask;
    }
    
    Out.vColor = vFinalColor;

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
}
