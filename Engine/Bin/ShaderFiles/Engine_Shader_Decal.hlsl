#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

uint g_iNumActiveDecals;
Texture2D g_DiffuseTexture, g_DepthTexture, g_NormalTexture;
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
    float3 vLocalPos : TEXCOORD0; // 로컬 좌표
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    // 투영 공간 -> PS에서 화면상의 좌표까지 변환
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    
    // 로컬 위치 전달 -> PS에서 데칼 영역 판별 및 UV 좌표 계산에 사용
    Out.vLocalPos = In.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vLocalPos : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 1. 화면상 위치를 텍스쿠드로 변환
    float2 vTexcoord = (In.vPosition.xy + 0.5f) / g_vScreenSize; // 픽셀의 정중앙 좌표
    
    // 2. 위에서 구한 텍스쿠드로 깊이 값 읽어오기
    //  vector vDepthDesc = 
    
    // 3. 깊이로 월드 포지션 복원
    
    // 4. 월드를 현재 데칼의 로컬 공간으로 변환
    
    // 5. 로컬 좌표 -0.5 0.5 를 데칼 UV 좌표 0, 1 범위로 변환
    
    // 6. 데칼 텍스처를 샘플링
    
    // 7. 위 텍스쿠드를 사용하여 원본 디퓨즈를 읽어옴
    
    // 8. CPU에서 계산된 불투명도와 데칼 텍스처의 알파값을 곱함
    
    // 9. 선형 보간을 통해 원본 색상과, 데칼 색상을 혼합, 출력
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, vTexcoord);
    
    Out.vColor = 1.f;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
