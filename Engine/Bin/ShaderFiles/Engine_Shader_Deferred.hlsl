#include "Engine_Shader_Defines.hlsli"

// ===== Matrix =====
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
Texture2D g_Texture;
vector g_vCamPosition;

// ===== Light =====
vector g_vLightDir, g_vLightPos;
float g_fRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

// ===== Material =====
vector g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f }, g_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

// ===== Render Target =====
Texture2D g_DiffuseTexture, g_NormalTexture, g_DepthTexture, g_ShadeTexture, g_SpecularTexture;
Texture2D g_LightDepthTexture, g_BackBufferTexture, g_BlurXTexture;

// ===== Cascade Shadow =====
int g_iTextureArrayIndex;
uint g_iNumCascades;
float g_Splits[4];
matrix g_LightViewMatrices[4], g_LightProjMatrices[4];
float2 g_vShadowMapSize;
float g_fBias;
int g_iEnableShadowFlag;

// ===== PCF =====
Texture2DArray<float> g_TextureArray;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_DEBUG_ARRAY(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    float fSlice = (float) g_iTextureArrayIndex;
    Out.vColor = g_TextureArray.Sample(DefaultSampler, float3(In.vTexcoord, fSlice));
    
    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    //  vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    vector vWorldPos;
    
    /* 투영공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/(w == 뷰스페이스상의 z) */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    /* 뷰공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * vDepthDesc.y;
    /* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    /* 월드공간상의 좌표를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    float fShade = max(dot(vNormal * -1.f, normalize(g_vLightDir)), 0.f);
    
    //  vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    //  vector vLook = vWorldPos - g_vCamPosition;
    //  
    //  float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);

    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));
    //  Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    /* 투영공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/(w == 뷰스페이스상의 z) */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    /* 뷰공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * vDepthDesc.y;
    /* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    /* 월드공간상의 좌표를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = saturate((g_fRange - fDistance) / g_fRange);
    
    float fShade = max(dot(vNormal * -1.f, normalize(vLightDir)), 0.f);
    
    //  vector vReflect = reflect(normalize(vLightDir), vNormal);
    //  vector vLook = vWorldPos - g_vCamPosition;
    //  
    //  float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    //  Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular * fAtt;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a == 0.f)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    //  vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //  Out.vColor = vDiffuse * vShade + vSpecular;
    Out.vColor = vDiffuse * vShade;
    
    if (0 == g_iEnableShadowFlag)
        return Out;
    
    /* 내 픽셀의 광원 기준의 깊이 */ 
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    /* 투영공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/(w == 뷰스페이스상의 z) */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    /* 뷰공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * vDepthDesc.y;
    /* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    float fCameraViewDepth = vWorldPos.z;
    
    /* 월드공간상의 좌표를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    uint iCascadeIndex = 0;
    
    if (fCameraViewDepth < g_Splits[0])
        iCascadeIndex = 0;
    else if (fCameraViewDepth < g_Splits[1])
        iCascadeIndex = 1;
    else if (fCameraViewDepth < g_Splits[2])
        iCascadeIndex = 2;
    else
        iCascadeIndex = 3;
    
    vector vPosition = mul(vWorldPos, g_LightViewMatrices[iCascadeIndex]);
    vPosition = mul(vPosition, g_LightProjMatrices[iCascadeIndex]);
    
    /* 광원기준으로 표현됐을때 그려져있어야할 위치에 이미 그려져있떤 누군가의 깊이 */
    float2 vTexcoord;
    
    /* -1 ~ 1 -> 0 ~ 1 */
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    
    /* 1 ~ -1 -> 0 ~ 1 */ 
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;

    float fLightDepth = vPosition.z / vPosition.w;
    
    float fShadowSum = 0.f;
    float2 vOffset;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= i; ++j)
        {
            vOffset.x = j * 1.f / g_vShadowMapSize.x;
            vOffset.y = i * 1.f / g_vShadowMapSize.y;
            float3 vSampleCoord = saturate(float3(vTexcoord + vOffset, iCascadeIndex));
            fShadowSum += g_TextureArray.SampleCmpLevelZero(ComparisonSampler, vSampleCoord, fLightDepth - g_fBias);
        }
    }
    
    fShadowSum /= 9.f;
    
    Out.vColor = lerp(Out.vColor * 0.3f, Out.vColor, fShadowSum);
    
    return Out;
}

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

struct PS_OUT_BLUR_X
{
    vector vBlurX : SV_TARGET0;
};

PS_OUT_BLUR_X PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    vector vColor;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + i / 1280.0f;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_BackBufferTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor / 7.5f;
    
    return Out;
}


PS_OUT_BACKBUFFER PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float2 vTexcoord;
    vector vColor;
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + i / 720.0f;
        
        vColor += g_fWeights[i + 6] * g_BlurXTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vColor = vColor / 7.5f;
    
    return Out;
}

struct PS_OUT_SSAO
{
    float4 vSSAO : SV_TARGET0;
};

PS_OUT_SSAO PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT_SSAO Out;

    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    // View Space Normal
    float3x3 ViewMatrix = float3x3(g_ViewMatrix._11_12_13, g_ViewMatrix._21_22_23, g_ViewMatrix._31_32_33);
    vector vViewNormal = float4(mul(vNormal.xyz, ViewMatrix), 0.f);
    vViewNormal = normalize(vViewNormal);
    
    // View Space Depth
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    // Projection -> View
    vWorldPos = vWorldPos * vDepthDesc.y;
    float4 vViewDepth = mul(vWorldPos, g_ProjMatrixInv);
    
    
    
    Out.vSSAO = float4(1.f, 0.f, 0.f, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass BlurY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass DebugArray
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG_ARRAY();
    }

    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();
    }
}
