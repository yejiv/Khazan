#include "Engine_Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_Texture;
float4      g_vSourceColor = float4(1.f, 1.f, 1.f, 1.f);

float       g_numCols, g_numRows;
float       g_FrameIdx;

float       g_fSizeRatio = 1.f;
float       g_fSize = 1.f;
float4      g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION; 
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    //float4 vWorldPos : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
            
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);       
    return Out;     
}


struct GS_IN
{
    float4 vPosition : SV_POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0; 
    float4 vProjPos : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];

    vector vLook = g_vCamPosition - In[0].vPosition;
    vector vRight = normalize(vector(cross(float3(0.f, 1.f, 0.f), vLook.xyz), 0.f)) * g_fSize * 0.5f;
    vector vUp = normalize(vector(cross(vLook.xyz, vRight.xyz), 0.f)) * g_fSize * g_fSizeRatio * 0.5f;
   
    
    float Width = 1.0f / g_numCols;
    float Height = 1.0f / g_numRows;
    
    float startU = (g_FrameIdx % g_numCols) * Width;
    float startV = floor(g_FrameIdx / g_numCols) * Height;
    
    matrix matrVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matrVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vTexcoord = float2(startU, startV) + (Out[0].vTexcoord * float2(Width, Height));
    
    
    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matrVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vTexcoord = float2(startU, startV) + (Out[1].vTexcoord * float2(Width, Height)); 
    
    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matrVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vTexcoord = float2(startU, startV) + (Out[2].vTexcoord * float2(Width, Height)); 
    
    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matrVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vTexcoord = float2(startU, startV) + (Out[3].vTexcoord * float2(Width, Height)); 
    
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
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vAccumColor : SV_TARGET0;
    float4 vAccumAlpha : SV_TARGET1;
};

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMask = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    vector vSourColor = float4(g_vSourceColor.xyz, 1.f);
    vector vFinalColor = vSourColor * vMask;
    vFinalColor.a = max(max(vMask.r, vMask.g), vMask.b);

    float vDestAlpha = max(max(vMask.r, vMask.g), vMask.b);
    
    if (vFinalColor.a <= 0)
        discard;

    vFinalColor.xyz *= (g_vSourceColor.a + 1);
    
    float z = In.vProjPos.z / In.vProjPos.w;
    float weight = max(1e-5, exp(-z * 0.8f));
    Out.vAccumColor = float4(vFinalColor.rgb * vFinalColor.a * weight, 0.f);
    Out.vAccumAlpha.r = vFinalColor.a * weight;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthTestOnly, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_BLEND();
    }
}
