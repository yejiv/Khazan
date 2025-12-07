#include "Engine_Shader_Defines.hlsli"

unsigned int g_MtrlFlags = { 0 };

bool IsFlag(unsigned int iMask)
{
    return (g_MtrlFlags & iMask) != 0;
}

static const unsigned int M_DIFFUSE = (1 << 0);
static const unsigned int M_NORMAL = (1 << 1);
static const unsigned int M_EMISSIVE = (1 << 2);
static const unsigned int M_SPECULAR = (1 << 3);
static const unsigned int M_METALIC = (1 << 4);
static const unsigned int M_ROUGHNESS = (1 << 5);

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/*재질*/
Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_SpecularTexture;
Texture2D g_EmissiveTexture;

Texture2D g_NoiseTexture;

/*눈 내리는 맵에 사용*/
Texture2D g_SnowTexture;
float g_fSnowAmount = float(0.5f);
float3 g_vSnowColor = float3(0.92f, 0.94f, 1.f);

/*흔들리는 풀때기에 사용*/
float g_fTime = 0.f;
float3 g_vWindDir = float3(1.f, 0.f, 3.f);
float g_fWindPower = 0.15f;
float g_fWindSpeed = 1.2f;

/* 바인딩 여부 */
bool g_isDiffuse = { false };
bool g_isNormal = { false };
bool g_isEmissive = { false };
bool g_isSpecular = { false };

vector g_vCamPosition;

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
    Out.vWorldPos.w = float(In.iID);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

VS_OUT VS_MAPOBJECT(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0; // C 스타일의 캐스팅 가능 - 0 초기화
 
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), In.TransformMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), In.TransformMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), In.TransformMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    Out.vWorldPos.w = float(In.iID);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

VS_OUT VS_PLANT(VS_IN In)
{
    // 풀 살랑살랑 할 예정 진짜 나중에
    
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
    Out.vWorldPos.w = float(In.iID);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos  : TEXCOORD0;
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
};

struct PS_OUT
{
    float4 vDiffuse  : SV_TARGET0;
    float4 vNormal   : SV_TARGET1;
    float4 vDepth    : SV_TARGET2;
    float4 vWorld    : SV_TARGET3;
    float4 vSpecular : SV_TARGET4;
    float4 vEmissive : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);          // sRGB : 파란색이던디
    
    vector vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);          // Non-Color
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);              // Non-Color
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    // float fNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexcoord);          // 나중에 플레이어 안가려지게 하는 용도로 쓸수있나??
    
    if (0.5f > vMtrlDiffuse.a)
        discard;
    
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
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
        
    if (vMtrlDiffuse.a <= 0.3f)
        discard;
        
    /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector vNormalDesc = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
        vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    float2 xy = vNormalDesc.xy * 2.f - 1.f;
    float3 vNormal = float3(xy.x, -xy.y, sqrt(saturate(1.f - dot(xy, xy))));
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    // Specular Test
    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // Emissive Test
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
        
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = vMtrlSpecular.rgb;
    Out.vSpecular.a = 0.f;
    //  Out.vEmissive = vMtrlEmissive;
    
    return Out;
}

PS_OUT PS_MAP_ICE(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
        
    /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector vNormalDesc = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
        vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    float2 xy = vNormalDesc.xy * 2.f - 1.f;
    float3 vNormal = float3(xy.x, -xy.y, sqrt(saturate(1.f - dot(xy, xy))));
    
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
    if (IsFlag(M_SPECULAR))
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // Emissive Test
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = float4(vFinalColor, fAlpha);
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = vMtrlSpecular.rgb;
    Out.vSpecular.a = 0.f;
    // Out.vEmissive = Out.vDiffuse * 0.1f;

    return Out;
}

PS_OUT PS_SNOWMAP(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float2 xy = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord).xy * 2.f - 1.f;
    float3 normalTS = float3(
        xy.x,
        -xy.y,
        sqrt(saturate(1.f - dot(xy, xy)))
    );

    float3x3 worldMat = float3x3(
        In.vTangent.xyz,
        In.vBinormal.xyz * -1.f,
        In.vNormal.xyz
    );

    float3 normalWS = normalize(mul(normalTS, worldMat));

    float up = saturate(dot(normalWS, float3(0.f, 1.f, 0.f)));

    float snow = saturate((up * g_fSnowAmount - 0.2f) * 1.5f);
    snow *= snow;

    float dist = distance(g_vCamPosition.xyz, In.vWorldPos.xyz);
    float distFade = lerp(0.35f, 1.0f, saturate(1.f - dist / 30.f));
    snow *= distFade;

    float3 color = lerp(vMtrlDiffuse.rgb, g_vSnowColor, snow);

    float3 flatUp = float3(0.f, 1.f, 0.f);
    float3 finalNormal = normalize(lerp(normalWS, flatUp, snow * 0.85f));

    Out.vDiffuse = float4(color, vMtrlDiffuse.a);
    Out.vNormal = float4(finalNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;

    Out.vSpecular = float4(0.02f, 0.02f, 0.02f, 0.f);
    Out.vEmissive = 0.f;

    return Out;
}

PS_OUT PS_SNOWMAP_ICE(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vMtrlDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // BC5 노멀 복원
    vector vNormalDesc = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
        vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    float2 xy = vNormalDesc.xy * 2.f - 1.f;
    float3 normalTS = float3(xy.x, -xy.y, sqrt(saturate(1.f - dot(xy, xy))));

    float3x3 worldMat = float3x3(
        In.vTangent.xyz,
        In.vBinormal.xyz * -1.f,
        In.vNormal.xyz
    );

    float3 normalWS = normalize(mul(normalTS, worldMat));

    // 카메라 반사 계산 (기존 ICE 로직 유지)
    float3 vToCamera = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    float fReflectPower = pow(saturate(1.f - dot(normalWS, vToCamera)), 4.f);
    float3 vReflectColor = float3(0.45f, 0.65f, 0.9f);
    float3 vIceColor = lerp(vMtrlDiffuse.rgb, vReflectColor, fReflectPower * 0.1f);

    // 위쪽 방향 눈 마스크
    float up = saturate(dot(normalWS, float3(0.f, 1.f, 0.f)));

    float rawSnow = up * g_fSnowAmount;
    float snow = saturate((rawSnow - 0.2f) * 1.5f);
    snow = snow * snow;

    // 눈 색 블렌딩 (기존 ICE 유지)
    float3 finalColor = lerp(vIceColor, g_vSnowColor, snow);

    // 핵심: 눈이 쌓인 부분에서 노멀 평탄화
    float3 flatUp = float3(0.f, 1.f, 0.f);
    float3 finalNormal = normalize(lerp(normalWS, flatUp, snow * 0.85f));

    // 알파 기존 방식 유지
    float fAlpha = saturate(0.6f + fReflectPower * 0.3f);

    float4 spec = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
        spec = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vDiffuse = float4(finalColor, fAlpha);
    Out.vNormal = float4(finalNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular = float4(spec.rgb, 0.f);
    Out.vEmissive = 0.f;

    return Out;
}

//==================================================================================================
//==================================== [  TECHNIQUE AND PASS  ] ====================================
//==================================================================================================

technique11 DefaultTechnique
{
    pass DefaultPass            // 기본 ( 0번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass WireFrame          // 와이어프레임 ( 1번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WIREFRAME();
    }

    pass Shadow         // 그림자 ( 2번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass MapPass // 맵 오브젝트용 패스 ( 3번 ) ( 눈 X )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP();
    }

    pass MapIcePass // 맵 오브젝트용 패스 ( 4번 ) ( 눈 X )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP_ICE();
    }

    pass SnowMapPass // 맵 오브젝트용 패스 ( 5번 ) ( 눈 O )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SNOWMAP();
    }

    pass SnowMapIcePass // 맵 오브젝트용 패스 ( 6번 ) ( 눈 O )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SNOWMAP_ICE();
    }

    pass PlantPass // 맵 오브젝트용 패스 ( 7번 ) ( 풀 흔들림 ( 눈 X ) )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_PLANT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAP();
    }

    pass SnowPlantPass // 맵 오브젝트용 패스 ( 8번 ) ( 풀 흔들림 ( 눈 O ) )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_PLANT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SNOWMAP();
    }
}