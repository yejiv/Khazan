#include "Engine_Shader_Deferred_Global_Constant.hlsli"

float4 Compute_ViewPosition_FromDepth(float2 vTexcoord, float fProjDepth, float fViewDepth)
{
    float4 vViewPos;

    vViewPos.x = vTexcoord.x * 2.f - 1.f;
    vViewPos.y = vTexcoord.y * -2.f + 1.f;
    vViewPos.z = fProjDepth;
    vViewPos.w = 1.f;

    vViewPos = vViewPos * fViewDepth;
    vViewPos = mul(vViewPos, g_ProjMatrixInv);

    return vViewPos;
}

float4 Transform_ViewToWorld(float4 vViewPos)
{
    return mul(vViewPos, g_ViewMatrixInv);
}

float4 Compute_WorldPosition_FromDepth(float2 vTexcoord, float fProjDepth, float fViewDepth)
{
    float4 vViewPos = Compute_ViewPosition_FromDepth(vTexcoord, fProjDepth, fViewDepth);
    
    return Transform_ViewToWorld(vViewPos);
}

float2 Compute_Texcoord_FromProjPos(float2 vProjPos)
{
    float2 vTexcoord;
    vTexcoord.x = vProjPos.x * 0.5f + 0.5f;
    vTexcoord.y = vProjPos.y * -0.5f + 0.5f;
    
    return vTexcoord;
}

float4 Transform_WorldToProj(float4 vWorldPos, matrix ViewMatrix, matrix ProjMatrix)
{
    float4 vProjPos;
        
    vProjPos = mul(vWorldPos, ViewMatrix);
    vProjPos = mul(vProjPos, ProjMatrix);
    vProjPos /= vProjPos.w;
    
    return vProjPos;
}