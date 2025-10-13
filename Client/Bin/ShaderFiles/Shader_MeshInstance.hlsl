#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture, g_NormalTexture, g_EmissiveTexture, g_SpecularTexture;

texture2D g_NoiseTexture;

float g_fFar = 1000.f;

// WIREFRAME용 Color
vector g_vColor = vector(0.8f, 0.4f, 0.8f, 1.f);

//=============================================================================================
//==================================== [  VERTEX SHADER  ] ====================================
//=============================================================================================

struct VS_IN
{
    float3 vPosition    : POSITION;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    float2 vTexcoord    : TEXCOORD0;
    
    row_major float4x4 TransformMatrix : WORLD;
    
    unsigned int iID    : ID;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal   : NORMAL;
    float4 vTangent  : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos  : TEXCOORD2;
    
    unsigned int iID : ID;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0; // C 스타일의 캐스팅 가능 - 0 초기화
 
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    Out.iID = In.iID;
    
    return Out;
}

//============================================================================================
//==================================== [  PIXEL SHADER  ] ====================================
//============================================================================================

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal   : NORMAL;
    float4 vTangent  : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos  : TEXCOORD2;    
    
    unsigned int iID : ID;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal  : SV_TARGET1;
    float4 vDepth   : SV_TARGET2;
    float4 vWorld   : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // 여기 모델 Mesh 사용했던거라 이펙트에 맞게 수정해주셔야 할듯합니다 ,, , ,
    vector vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);          // sRGB : 파란색이던디
    
    vector vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);          // Non-Color
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);              // Non-Color
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    // float fNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexcoord);          // 나중에 플레이어 안가려지게 하는 용도로 쓸수있나??
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    
    return Out;
}

PS_OUT PS_WIREFRAME(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_vColor;
    Out.vNormal = 0.f;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

//==================================================================================================
//==================================== [  TECHNIQUE AND PASS  ] ====================================
//==================================================================================================

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

    pass WireFrame
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WIREFRAME();
    }
}