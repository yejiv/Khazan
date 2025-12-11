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
Texture2D g_MetalnessTexture;
Texture2D g_DissolveTexture;
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

// Edge
float g_fEdgeIntensity, g_fShadeIntensity;

//이미시브
float g_fEmissiveValue;

//디죨브
float g_fDecreaseAlpha;
float g_fEdgeWidth;
float4 g_fEdgeColor;

// 임프
float g_fDiffusePower = 1.f;

// 바이퍼
float g_fEmissiveIntensity = 1.f;

// 잼스톤
float g_fDiffuseRedPower;
float4 g_vGemColor;

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

PS_OUT PS_DESTINYSTONE(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
        
    /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    float3 vNormal = normalize(vMtrlNormal.xyz * 2.f - 1.f);
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);

    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a <= 0.f)
        vMtrlDiffuse = g_vGemColor;

    Out.vDiffuse = vMtrlDiffuse * g_fEmissiveIntensity;
    Out.vDiffuse.r *= g_fDiffuseRedPower;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = vMtrlSpecular.rgb;
    Out.vSpecular.a = 1.f;

    return Out;
}

PS_OUT PS_DESTINYGEM(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
        
    /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    float3 vNormal = normalize(vMtrlNormal.xyz * 2.f - 1.f);
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);

    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a <= 0.f)
        vMtrlDiffuse = g_vGemColor;

    Out.vDiffuse = vMtrlDiffuse * g_fEmissiveIntensity;
    Out.vDiffuse.r *= g_fDiffuseRedPower;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = vMtrlSpecular.rgb;
    Out.vSpecular.a = 1.f;
    
    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, Out.vDiffuse);
    
    if (Out.vDiffuse.a <= 0.f)
        discard;

    return Out;
}

PS_OUT PS_SWORD_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 1.f;
    
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    Out.vDiffuse *= fEdgeMask;
    Out.vDiffuse *= fShadeMask;
    
    if (vEmissive.g >= 0.95f)
    {
        float3 vColor = { 2.455f, 1.937f, 2.784f };
        //  Out.vDiffuse.rgb = Out.vDiffuse.rgb + vColor * 0.3f * g_fEmissiveValue;
        Out.vEmissive.rgb = Out.vDiffuse.rgb + vColor * 0.3f * g_fEmissiveValue;
    }
    
    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, Out.vDiffuse);
    
    if (Out.vDiffuse.a <= 0.f)
        discard;
    return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 1.f;
    
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    Out.vDiffuse *= fEdgeMask;
    Out.vDiffuse *= fShadeMask;
    
    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, Out.vDiffuse);
    
    if (Out.vDiffuse.a <= 0.f)
        discard;
    
    return Out;
}

PS_OUT PS_IMP_WEAPON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = vector(0.f, 0.f, 0.f, 0.f);
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 0.f;
    //  Out.vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    Out.vDiffuse *= fEdgeMask;
    Out.vDiffuse *= fShadeMask;

    // Diffuse 어두운 문제로 임의값 곱해주기 3~5
    Out.vDiffuse *= g_fDiffusePower;
    
    return Out;
}

PS_OUT PS_VIPER_WEAPON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;

    float4 vEmissiveDesc = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);    
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vSpecular.a = 0.f;
    float4 vMetalnessDesc = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
        
    float fEdgeMask = lerp(1.f - g_fEdgeIntensity, 1.f, vMetalnessDesc.r);
    float fShadeMask = lerp(1.f - g_fShadeIntensity, 1.f, vMetalnessDesc.g); // 음영 보간 0인 부분인 0.5, 1인 부분은 원색
    vMtrlDiffuse *= fEdgeMask;
    vMtrlDiffuse *= fShadeMask;
    
    float fMask = vEmissiveDesc.g;
    vMtrlDiffuse += vMtrlDiffuse * fMask * g_fEmissiveIntensity;

    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_OUT PS_ILLUSION_WALL(PS_IN In)                       // 맵 오브젝트용 픽셀 쉐이더
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_DIFFUSE))
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
        
    if (vMtrlDiffuse.a <= 0.3f)
        discard;
        
    /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector vMtrlNormal = vector(In.vNormal.xyz, 0.f);
    if (IsFlag(M_NORMAL))
        vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    float2 xy = vMtrlNormal.xy * 2.f - 1.f;
    float3 vNormal = float3(xy.x, -xy.y, sqrt(saturate(1.f - dot(xy, xy))));
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);

    vector vMtrlSpecular = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_SPECULAR))
        vMtrlSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vMtrlEmissive = float4(0.f, 0.f, 0.f, 0.f);
    if (IsFlag(M_EMISSIVE))
        vMtrlEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    Out.vWorld = In.vWorldPos;
    Out.vSpecular.rgb = vMtrlSpecular.rgb;
    Out.vSpecular.a = 1.f;
    
    Out.vDiffuse = Dissolve(g_fDecreaseAlpha, g_DissolveTexture.Sample(PointSampler, In.vTexcoord).r, g_fEdgeWidth, g_fEdgeColor, Out.vDiffuse);
    
    if (Out.vDiffuse.a <= 0.f)
        discard;

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

    pass PS_SWORD_EMISSIVE_8 // 엘라메인 검 이미시브 8번
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SWORD_EMISSIVE();
    }
    pass PS_MAIN_DISSOLVE_9 // 기본 ( 9번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

    pass Imp_Weapon // 임프 무기 ( 10번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_IMP_WEAPON();
    }

    pass Viper_Weapon // 바이퍼 무기 ( 11번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_VIPER_WEAPON();
    }

    pass DestinyStonePass // 귀석 받침 패스 ( 12번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DESTINYSTONE();
    }

    pass DestinyGemPass // 귀석 젬 패스 ( 13번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DESTINYGEM();
    }

    pass IllusionWallPass // 귀석 젬 패스 ( 14번 )
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAPOBJECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ILLUSION_WALL();
    }
}
