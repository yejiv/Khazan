#include "Engine_Shader_Defines.hlsli"

float4x4    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D   g_DiffuseTextures[8];

float4 TexMaping(int index, float2 vTexcoord)
{
    float4 vColor = {0.f, 0.f, 0.f, 0.f };
    
    if (index == 0)
        vColor = g_DiffuseTextures[0].Sample(DefaultSampler, vTexcoord);
    else if (index == 1)
        vColor = g_DiffuseTextures[1].Sample(DefaultSampler, vTexcoord);
    else if (index == 2)
        vColor = g_DiffuseTextures[2].Sample(DefaultSampler, vTexcoord);
    else if (index == 3)
        vColor = g_DiffuseTextures[3].Sample(DefaultSampler, vTexcoord);
    else if (index == 4)
        vColor = g_DiffuseTextures[4].Sample(DefaultSampler, vTexcoord);
    else if (index == 5)
        vColor = g_DiffuseTextures[5].Sample(DefaultSampler, vTexcoord);
    else if (index == 6)
        vColor = g_DiffuseTextures[6].Sample(DefaultSampler, vTexcoord);
    else if (index == 7)
        vColor = g_DiffuseTextures[7].Sample(DefaultSampler, vTexcoord);
    
    return vColor;
}
struct VS_IN
{
    float3 vPosition                    : POSITION;
    row_major float4x4 TransformMatrix  : WORLD;
    float4 vUV                          : TEXCOORD0;
    float4 vColor                       : TEXCOORD1;
    float  vAlpha                       : TEXCOORD2;
    nointerpolation uint2 vPass         : TEXCOORD3;
};

struct VS_DEFAULT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 fSize     : PSIZE;
    float4 vUV       : TEXCOORD0;
    float4 vColor    : TEXCOORD1;
    float  vAlpha    : TEXCOORD2;
    nointerpolation uint2 vPass : TEXCOORD3;
};

VS_DEFAULT_OUT VS_MAIN(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    Out.vPosition = vPosition;
    Out.fSize.x = length(In.TransformMatrix._11_12_13);
    Out.fSize.y = length(In.TransformMatrix._21_22_23);
    
    Out.vUV = In.vUV;
    Out.vColor = In.vColor;
    Out.vAlpha = In.vAlpha;
    Out.vPass = In.vPass;
    
    return Out;
}

struct GS_IN
{
    float4 vPosition : SV_POSITION;
    float2 fSize     : PSIZE;
    float4 vUV       : TEXCOORD0;
    float4 vColor    : TEXCOORD1;
    float vAlpha     : TEXCOORD2;
    nointerpolation uint2 vPass : TEXCOORD3;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vUV       : TEXCOORD1;
    float4 vColor    : TEXCOORD2;
    float vAlpha     : TEXCOORD3;
    nointerpolation uint2 vPass : TEXCOORD4;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];
    
    vector vRight = normalize(float4(g_ViewMatrix._11, g_ViewMatrix._12, g_ViewMatrix._13, 0.f)) * In[0].fSize.x * -0.5f;
    vector vUp = normalize(float4(g_ViewMatrix._21, g_ViewMatrix._22, g_ViewMatrix._23, 0.f)) * In[0].fSize.y * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vUV = In[0].vUV;
    Out[0].vColor = In[0].vColor;
    Out[0].vAlpha = In[0].vAlpha;
    Out[0].vPass = In[0].vPass;
    
    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vUV = In[0].vUV;
    Out[1].vColor = In[0].vColor;
    Out[1].vAlpha = In[0].vAlpha;
    Out[1].vPass = In[0].vPass;
    
    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vUV = In[0].vUV;
    Out[2].vColor = In[0].vColor;
    Out[2].vAlpha = In[0].vAlpha;
    Out[2].vPass = In[0].vPass;
    
    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vUV = In[0].vUV;
    Out[3].vColor = In[0].vColor;
    Out[3].vAlpha = In[0].vAlpha;
    Out[3].vPass = In[0].vPass;
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[1]);
    Vertices.Append(Out[2]);
    Vertices.RestartStrip();
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[2]);
    Vertices.Append(Out[3]);
    Vertices.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vUV       : TEXCOORD1;
    float4 vColor    : TEXCOORD2;
    float vAlpha     : TEXCOORD3;
    nointerpolation uint2 vPass : TEXCOORD4;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

float4 ShaderPass_2(float4 vColor, PS_IN In)
{
    vColor.rgb = (vColor.r) * In.vColor.rgb;
    vColor.a = vColor.a * In.vColor.a * In.vAlpha;
    return vColor;
}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    In.vTexcoord = In.vUV.xy + (In.vUV.zw - In.vUV.xy) * In.vTexcoord;
    Out.vColor = TexMaping(In.vPass.x, In.vTexcoord);
    

    if (In.vPass.y == 2)
        Out.vColor = ShaderPass_2(Out.vColor, In);
    else
        Out.vColor.a = Out.vColor.a * In.vColor.a * In.vAlpha;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }


}