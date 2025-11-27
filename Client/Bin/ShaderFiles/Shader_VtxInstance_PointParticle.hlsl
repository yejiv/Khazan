#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vCamPosition;

float4 g_vSourceColor = float4(1.f, 1.f, 1.f, 1.f);
float g_fSizeRatio = 1.f;

bool g_MaskScrollYDir;
bool g_MaskScrollInv;
float g_MaskScrollSpeed;

bool g_IsEmissive = false;
bool g_IsDisolve = false;
bool g_IsBillboard;

float g_numCols, g_numRows;
float g_FrameIdx;

float g_EdgeWidth;
float4 g_EdgeColor; 
texture2D g_DiffuseTexture;
texture2D g_MaskTexture;
texture2D g_DisolveTexture;
texture2D g_DepthTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    
    row_major float4x4 TransformMatrix : WORLD;

    float3 vPrevPosition : TEXCOORD0;
    float bDead : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

struct VS_DEFAULT_OUT
{
    float4 vPosition : SV_POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float bDead : TEXCOORD1;
    float4 vPrevPosition : TEXCOORD2;
};

VS_DEFAULT_OUT VS_MAIN(VS_IN In)
{
    VS_DEFAULT_OUT Out = (VS_DEFAULT_OUT) 0;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    float4x4 prevMatrix = In.TransformMatrix;
    prevMatrix._41_42_43 = float4(In.vPrevPosition, 1.f);
    float4 vPrevPosition = mul(float4(In.vPosition, 1.f), prevMatrix);

    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vPrevPosition = mul(vPrevPosition, g_WorldMatrix);
        
    Out.fSize = length(In.TransformMatrix._11_12_13);
    Out.vLifeTime = In.vLifeTime;
    Out.bDead = In.bDead;

    return Out;
}

struct GS_IN
{
    float4 vPosition : SV_POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float bDead : TEXCOORD1;
    float4 vPrevPosition : TEXCOORD2;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float bDead : TEXCOORD2;
    float4 vPrevPosition : TEXCOORD3;
    float4 vProjPos : TEXCOORD4;
    float vFrame : TEXCOORD5;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];
    
    vector vRight;
    vector vUp;
    vector vLook;
    
    float legnth = length(In[0].vPosition - In[0].vPrevPosition);
    
    if (legnth < 0.01f || g_IsBillboard)
    {
        vLook = g_vCamPosition - In[0].vPosition;
        vRight = normalize(vector(cross(float3(0.f, 1.f, 0.f), vLook.xyz), 0.f)) * In[0].fSize * 0.5f;
        vUp = normalize(vector(cross(vLook.xyz, vRight.xyz), 0.f)) * In[0].fSize * g_fSizeRatio * 0.5f;
    }
    else
    {
        vUp = normalize(In[0].vPosition - In[0].vPrevPosition) * In[0].fSize * g_fSizeRatio * 0.5f;
        vLook = normalize(g_vCamPosition - In[0].vPosition);
        vRight = normalize(vector(cross(vUp.xyz, vLook.xyz), 0.f)) * In[0].fSize * 0.5f;
        vUp += (In[0].vPosition - In[0].vPrevPosition) * 0.8f;
    }
    
    
    float Width = 1.0f / g_numCols;
    float Height = 1.0f / g_numRows;
    
    float startU = (g_FrameIdx % g_numCols) * Width;
    float startV = floor(g_FrameIdx / g_numCols) * Height;
    
    matrix matrVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float frame = (g_FrameIdx + 1) / (g_numCols * g_numRows);
    
    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matrVP);
    Out[0].vProjPos = Out[0].vPosition;;
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vTexcoord = float2(startU, startV) + (Out[0].vTexcoord * float2(Width, Height));
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].bDead = In[0].bDead;
    Out[0].vPrevPosition = In[0].vPrevPosition;
    Out[0].vFrame = frame;
    
    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matrVP);
    Out[1].vProjPos = Out[1].vPosition;
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vTexcoord = float2(startU, startV) + (Out[1].vTexcoord * float2(Width, Height));
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].bDead = In[0].bDead;
    Out[1].vPrevPosition = In[0].vPrevPosition;
    Out[1].vFrame = frame;
    
    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matrVP);
    Out[2].vProjPos = Out[2].vPosition;
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vTexcoord = float2(startU, startV) + (Out[2].vTexcoord * float2(Width, Height));
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].bDead = In[0].bDead;
    Out[2].vPrevPosition = In[0].vPrevPosition;
    Out[2].vFrame = frame;
    
    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matrVP);
    Out[3].vProjPos = Out[3].vPosition;
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vTexcoord = float2(startU, startV) + (Out[3].vTexcoord * float2(Width, Height));
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].bDead = In[0].bDead;
    Out[3].vPrevPosition = In[0].vPrevPosition; 
    Out[3].vFrame = frame;
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[1]);
    Vertices.Append(Out[2]);
    Vertices.RestartStrip();
    
    Vertices.Append(Out[0]);
    Vertices.Append(Out[2]);
    Vertices.Append(Out[3]);
    Vertices.RestartStrip();
}

struct PS_DEFAULT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float bDead : TEXCOORD2;
    float4 vPrevPosition : TEXCOORD3;
    float4 vProjPos : TEXCOORD4;
    float vFrame : TEXCOORD5;
};

struct PS_OUT
{
    float4 vAccumColor : SV_TARGET0;
    float4 vAccumAlpha : SV_TARGET1;
};

float Mask_Scrolling(float2 vLifetime, float2 vTexcoord)
{
    float safeProgress = saturate(vLifetime.x * g_MaskScrollSpeed / vLifetime.y) * 2; // 0 ~ 2   그니까 0 이면 -2. 1이면 0이 되어야함
    //float maskOffset = (safeProgress * 2.0f) - 2.0f; // -2 ~ 2 이거 끝까지 돌리기로수 ㅓㅇ
    float maskOffset = safeProgress - 1.0f;
    float2 maskUV; //-2 -1
    
    if (g_MaskScrollYDir)
    {
        if (g_MaskScrollInv)
            maskUV = float2(vTexcoord.x, vTexcoord.y - maskOffset);
        else
            maskUV = float2(vTexcoord.x, vTexcoord.y + maskOffset);
    }
    else
    {
        if (g_MaskScrollInv)
            maskUV = float2(vTexcoord.x - maskOffset, vTexcoord.y);
        else
            maskUV = float2(vTexcoord.x + maskOffset, vTexcoord.y);
    }

    float maskValue = g_MaskTexture.Sample(ClampSampler, maskUV).r;
    
    return maskValue;
}


float4 Dissolve(float4 InColor, float fDecreaseAlpha, float2 UV)
{
    float4 rt = InColor;
    
    float noise = g_DisolveTexture.Sample(PointSampler, UV).r;
    
    if (noise < fDecreaseAlpha)
    {
        rt.a = 0.f;
        return rt;
    }
    
    float edgeStart = fDecreaseAlpha;
    float edgeEnd = fDecreaseAlpha + g_EdgeWidth;
    float edgeFactor = smoothstep(edgeStart, edgeEnd, noise);
    rt = lerp(g_EdgeColor * 2.f, InColor, edgeFactor);
    
    return rt;
}


PS_OUT PS_MAIN(PS_DEFAULT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (In.bDead)
        discard;
    
    vector vMask = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vSourColor = float4(g_vSourceColor.xyz, 1.f);
    vector vFinalColor = vSourColor * vMask;
    
    vFinalColor.a = max(max(vMask.r, vMask.g), vMask.b);
    
    if (g_MaskScrollSpeed)
        vFinalColor.a = vFinalColor.a * Mask_Scrolling(In.vLifeTime, In.vTexcoord);
     
    float fDecreaseAlpha;  
    
    fDecreaseAlpha = 1.0f - abs((In.vLifeTime.x / In.vLifeTime.y) * 2.0f - 1.0f); 
    
    if (g_IsDisolve == false) //스프라이트 중에서, 라이프타임이 적은 건 안한다.
        vFinalColor.a *= fDecreaseAlpha;
    else
        vFinalColor = Dissolve(vFinalColor, (In.vLifeTime.x / In.vLifeTime.y), In.vTexcoord);
    
    if (vFinalColor.a <= 0)
        discard;

    vFinalColor.xyz *= (g_vSourceColor.a + 1.5);
    
    /* Soft Effect */
    //float2 vTexcoord;
    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    //vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    //vFinalColor.a = vFinalColor.a * saturate(vDepthDesc.y - In.vProjPos.w);
    
    /* Blend Weight */
    float z = In.vProjPos.z / In.vProjPos.w; // 0..1 depth
    float weight = max(1e-5, exp(-z * 0.75f));
    Out.vAccumColor = float4(vFinalColor.rgb * vFinalColor.a, vFinalColor.a) * weight;
    Out.vAccumAlpha.r = vFinalColor.a;

    //Out.vAccumColor = float4(vFinalColor.rgb * vFinalColor.a * weight, 0.f); 
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthTestOnly, 0);
        SetBlendState(BS_WeightBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
 