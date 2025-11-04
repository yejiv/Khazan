#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/*재질*/
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_SpecularTexture;
texture2D g_EmissiveTexture;

/*눈 내리는 맵에 사용*/
texture2D g_SnowTexture;
float g_fSnowAmount = float(0.5f);
float3 g_vSnowColor = float3(0.92f, 0.94f, 1.f);

/*바인딩 여부*/
bool g_isDiffuse = { false };
bool g_isNormal = { false };
bool g_isEmissive = { false };
bool g_isSpecular = { false };

vector g_vCamPosition;

float g_fFar = 1000.f;

vector    g_vColor = vector(1.f, 0.f, 1.f, 1.f);

unsigned int g_iMapObjectID;

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

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */ 
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */ 
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

VS_OUT VS_MAPOBJECT(VS_IN In)
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
    Out.vWorldPos.w = float(g_iMapObjectID);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};


VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
      /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP); // In.vPosition 은 float3 짜리이므로 w = 1.f 넣어서 행렬과 곱 가능하게
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

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
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vWorld : SV_TARGET3;
    float4 vSpecular : SV_TARGET4;
    float4 vEmissive : SV_TARGET5;
};

/* 만든 픽셀 각각에 대해서 픽셀 쉐이더를 수행한다. */
/* 픽셀의 색을 결정한다. */


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector      vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //if (vMtrlDiffuse.a < 0.3f)
    //    discard;
    
    /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector      vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3      vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);        
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    return Out;
}

PS_OUT PS_WIREFRAME(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_vColor;
    
    return Out;
}

PS_OUT PS_SOLIDFRAME(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_vColor;
    Out.vWorld = In.vWorldPos;
    
    return Out;
}

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    float4 vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.vLightDepth = float4(In.vProjPos.w / g_fFar, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAP(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a <= 0.3f)
        discard;
        
    /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    // Specular Test
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_isSpecular)
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // Emissive Test
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_isEmissive)
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
        
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = vMtrlSpecular;
    //  Out.vEmissive = vMtrlEmissive;
    
    return Out;
}

PS_OUT PS_MAP_ICE(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    float3 vToCamera = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    
    float fReflectPower = pow(saturate(1.f - dot(vNormal, vToCamera)), 4.f);
    
    float3 vReflectColor = float3(0.45f, 0.65f, 0.9f);
    
    float3 vIceColor = lerp(vMtrlDiffuse.rgb, vReflectColor, fReflectPower * 0.1f);
    
    float3 vFinalColor = saturate(vIceColor * 0.95f);
    
    float fAlpha = saturate(0.6f + fReflectPower * 0.3f);
    
    // Specular Test
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_isSpecular)
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // Emissive Test
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_isEmissive)
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = float4(vFinalColor, fAlpha);
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = vMtrlSpecular;
    // Out.vEmissive = Out.vDiffuse * 0.1f;

    return Out;
}

PS_OUT PS_SNOWMAP(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a <= 0.3f)
        discard;
        
    /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    float3 vWorldNormal = normalize(In.vNormal.rgb);
    float upFactor = saturate(dot(vWorldNormal, float3(0.f, 1.f, 0.f)));
    
    float fSnowMask = 0.8f;
    
    float fSnowBlend = saturate(upFactor * fSnowMask * g_fSnowAmount);
    
    float3 vSnowColor = g_vSnowColor;
    
    float3 vFinalColor = lerp(vMtrlDiffuse.rgb, vSnowColor, fSnowBlend);
    
    // Specular Test
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_isSpecular)
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // Emissive Test
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_isEmissive)
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = float4(vFinalColor, vMtrlDiffuse.a);
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = vMtrlSpecular;
    // Out.vEmissive = vMtrlEmissive;
    
    return Out;
}

PS_OUT PS_SNOWMAP_ICE(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT)0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    float3 vToCamera = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    
    float fReflectPower = pow(saturate(1.f - dot(vNormal, vToCamera)), 4.f);
    
    float3 vReflectColor = float3(0.45f, 0.65f, 0.9f);
    
    float3 vIceColor = lerp(vMtrlDiffuse.rgb, vReflectColor, fReflectPower * 0.1f);
    
    float3 vWorldNormal = normalize(vNormal);
    float upFactor = saturate(dot(vWorldNormal, float3(0.f, 1.f, 0.f)));
    
    float fSnowMask = 0.8f;

    float fSnowBlend = saturate(upFactor * fSnowMask * g_fSnowAmount);
    float3 vSnowColor = g_vSnowColor;
    
    float3 vFinalColor = lerp(vIceColor, vSnowColor, fSnowBlend);
    
    float fAlpha = saturate(0.6f + fReflectPower * 0.3f);
    
    // Specular Test
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_isSpecular)
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // Emissive Test
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_isEmissive)
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = float4(vFinalColor, fAlpha);
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = vMtrlSpecular;
    // Out.vEmissive = Out.vDiffuse * 0.1f;

    return Out;
}

technique11 DefaultTechnique
{
    /* 특정 패스를 이용해서 점정을 그려냈다. */
    /* 하나의 모델을 그려냈다. */ 
    /* 모델의 상황에 따라 다른 쉐이딩 기법 세트(명암 + 림라이트 + 스펙큘러 + 노멀맵 + ssao )를 먹여주기위해서 */
    pass DefaultPass            // 기본 ( 0번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass WireFrame // 와이어프레임 ( 1번 )
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WIREFRAME();
    }

    pass SolidFrame // 솔리드프레임 ( 2번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SOLIDFRAME();
    }

    pass Shadow             // 그림자 ( 3번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass MapPass                        // 맵 오브젝트용 패스 ( 4번 ) ( 눈 X )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP();
    }

    pass MapIcePass                   // 맵 오브젝트용 패스 ( 5번 ) ( 눈 X )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP_ICE();
    }

    pass SnowMapPass               // 맵 오브젝트용 패스 ( 6번 ) ( 눈 O )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SNOWMAP();
    }

    pass SnowMapIcePass               // 맵 오브젝트용 패스 ( 7번 ) ( 눈 O )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SNOWMAP_ICE();
    }

    ///* 모델의 상황에 따라 다른 쉐이딩 기법 세트(블렌딩 + 디스토션  )를 먹여주기위해서 */
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

    ///* 정점의 정보에 따라 쉐이더 파일을 작성한다. */
    ///* 정점의 정보가 같지만 완전히 다른 취급을 하느 ㄴ객체나 모델을 그리는 방식 -> 렌더링방식에 차이가 생길 수 있다. */ 
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

}
