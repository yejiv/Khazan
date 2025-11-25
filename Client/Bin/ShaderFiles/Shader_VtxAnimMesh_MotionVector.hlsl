#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_PrevWorldMatrix, g_PrevViewMatrix, g_PrevProjMatrix;

matrix g_BoneMatrices[256];
matrix g_PrevBoneMatrices[256];

float2 g_vScreenSize;

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
    float4 vPrevPosition : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix CurBoneMatrix =
        g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    matrix PrevBoneMatrix =
        g_PrevBoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_PrevBoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_PrevBoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_PrevBoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    // 로컬 위치 계산
    float4 vCurPosition = mul(float4(In.vPosition, 1.f), CurBoneMatrix);
    float4 vPrevPosition = mul(float4(In.vPosition, 1.f), PrevBoneMatrix);
    
    // 월드 -> 클립 위치 계산
    float4x4 CurMatrixWV, CurMatrixWVP;
    
    CurMatrixWV = mul(g_WorldMatrix, g_ViewMatrix);
    CurMatrixWVP = mul(CurMatrixWV, g_ProjMatrix);
    
    float4x4 PrevMatrixWV, PrevMatrixWVP;
    
    PrevMatrixWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
    PrevMatrixWVP = mul(PrevMatrixWV, g_PrevProjMatrix);
    
    Out.vPosition = mul(vCurPosition, CurMatrixWVP);
    Out.vPrevPosition = mul(vPrevPosition, PrevMatrixWVP);
    
    //  float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    //  
    //  matrix BoneMatrix =
    //      g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
    //      g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
    //      g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
    //      g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    //  
    //  vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    //      
    //  float4x4 matWV, matWVP;
    //  
    //  matWV = mul(g_WorldMatrix, g_PrevViewMatrix);
    //  matWVP = mul(matWV, g_PrevProjMatrix);
    //  
    //  Out.vPosition = mul(vPosition, matWVP);    
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vPrevPosition : TEXCOORD0;
};

struct PS_OUT
{
    float4 vVelocity : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 이전 위치 NDC 공간으로 변환
    float4 vPrevPos = In.vPrevPosition / In.vPrevPosition.w;
    
    // Texcoord 범위로 변환
    float2 vPrevUV;
    vPrevUV.x = vPrevPos.x * 0.5f + 0.5f;
    vPrevUV.y = vPrevPos.y * -0.5f + 0.5f;
    
    // 현재 위치(스크린 좌표) NDC 공간으로 변환
    //  float4 vCurUV = In.vPosition.xy / g_vScreenSize;
    float4 vCurPos = In.vPosition;
    float2 vCurUV = vCurPos.xy / g_vScreenSize;
    //  vCurUV.y = 1.f - vCurUV.y;
    
    float2 vMotionvector = vCurUV - vPrevUV;
    
    //  Out.vVelocity = float4(vMotionvector.xy, vCurPos.z, 1.f);
    Out.vVelocity = float4(vMotionvector.xy, vCurPos.z, 1.f);
    
    //  Out.vVelocity = float4(1.f, 0.f, 0.f, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_DepthTestOnly, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
