#include "Engine_Shader_Function.hlsli"

struct PARTICLE_PARAMS
{
    float4 vInitTranslation;
    float fSize;
};

struct VTXINSTANCE_PARTICLE
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float3 vPrevPosition;
    float bDead;
    float2 vLifeTime;
};

struct VTXINSTANCE_DYNAMIC_DATA
{
    float4 fSpeed;
    float fGravity;
    float bDead;    //CPU에서 읽을 값.
    float2 padding;
};

cbuffer CB_PARTICLE : register(b0)
{
    float g_fTimeDelta;
    float3 g_vPivot;
    uint g_iNumInstances;
    float3 Padding0;
    
    uint g_iSpeedType;
    float2 g_fSpeedRange;
    
    float g_bIsLoop;
    float3 g_SpawnRange;
    float g_TotalTime;
    
    float g_TurblunceSpeed;
    float g_TurblunceSampleSize;
    float2 padding;
};

StructuredBuffer<PARTICLE_PARAMS> g_InputData : register(t0);
Texture2D g_NoiseTexture : register(t1);
RWStructuredBuffer<VTXINSTANCE_PARTICLE> g_OutputData : register(u0);
RWStructuredBuffer<VTXINSTANCE_DYNAMIC_DATA> g_SpeedData : register(u1);
SamplerState g_LinearWrapSampler : register(s0);

void RotateParticle(inout VTXINSTANCE_PARTICLE Particle, uint iIndex)
{
    float fAngle = g_SpeedData[iIndex].fSpeed.y * g_fTimeDelta;
    float s, c;
    sincos(fAngle, s, c);
    
    matrix RotationMatrix =
    {
        c, 0, s, 0,
         0, 1, 0, 0,
        -s, 0, c, 0,
         0, 0, 0, 1
    };
    
    matrix Pivot_World =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        g_vPivot.x, g_vPivot.y, g_vPivot.z, 1
    };
    
    matrix final_Matrix = mul(RotationMatrix, Pivot_World);
    float4 pos = Particle.vTranslation;
    Particle.vTranslation = mul(pos, final_Matrix);
}

[numthreads(256, 1, 1)]
void CS_MOVE(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= g_iNumInstances)
        return; 
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
    VTXINSTANCE_DYNAMIC_DATA SpeedData = g_SpeedData[iIndex];
	
    if (Particle.bDead > 0.f)
        return;
    
    Particle.vPrevPosition = Particle.vTranslation.xyz;
    
    //Scale
    Particle.vRight.x += SpeedData.fSpeed.w;
    Particle.vUp.y += SpeedData.fSpeed.w;
    Particle.vLook.z += SpeedData.fSpeed.w;
	
	//Rotation
    if (SpeedData.fSpeed.y)
        RotateParticle(Particle, iIndex);
	
	//Spread
    vector vMoveDir = vector((Particle.vTranslation.xyz - g_vPivot), 0.f);
    Particle.vTranslation = Particle.vTranslation + normalize(vMoveDir) * SpeedData.fSpeed.x * g_fTimeDelta;    
    
	//MoveLinear
    Particle.vTranslation = Particle.vTranslation + float4(0.f, 1.f, 0.f, 0.f) * SpeedData.fSpeed.z * g_fTimeDelta;
    
    Particle.vLifeTime.x += g_fTimeDelta;

    if (Particle.vLifeTime.x >= Particle.vLifeTime.y
		|| (SpeedData.fSpeed.x < 0 && length(vMoveDir).x < 1.f))
    {
        Particle.vLifeTime.x = 0.f;
        Particle.vTranslation = g_InputData[iIndex].vInitTranslation;
        Particle.vPrevPosition = g_InputData[iIndex].vInitTranslation;
        if (g_bIsLoop == 0)
            Particle.bDead = 1.f;
    }
    
    if (Particle.bDead == 0.f)
        g_SpeedData[0].bDead = 0.f;
	
    if (Particle.vRight.x <= 0.f)
    {
        Particle.vRight = float4(g_InputData[iIndex].fSize, 0.f, 0.f, 0.f);
        Particle.vUp = float4(0.f, g_InputData[iIndex].fSize, 0.f, 0.f);
        Particle.vLook = float4(0.f, 0.f, g_InputData[iIndex].fSize, 0.f);
    }
    
    g_OutputData[iIndex] = Particle;
}

[numthreads(256, 1, 1)]
void CS_UPDATE_SPEED(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x; 
    
    VTXINSTANCE_DYNAMIC_DATA SpeedData = g_SpeedData[iIndex];

    if (g_iSpeedType == 0)
        SpeedData.fSpeed.x = rand_between(g_fSpeedRange.x, g_fSpeedRange.y, iIndex);
    else if (g_iSpeedType == 1)
        SpeedData.fSpeed.y = rand_between(g_fSpeedRange.x, g_fSpeedRange.y, iIndex);
    else if (g_iSpeedType == 2)
        SpeedData.fSpeed.z = rand_between(g_fSpeedRange.x, g_fSpeedRange.y, iIndex);
    else
        SpeedData.fSpeed.w = rand_between(g_fSpeedRange.x, g_fSpeedRange.y, iIndex);
    
    g_SpeedData[iIndex] = SpeedData;
}

[numthreads(256, 1, 1)]
void CS_UPDATE_GRAVITY(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= g_iNumInstances)
        return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
    VTXINSTANCE_DYNAMIC_DATA SpeedData = g_SpeedData[iIndex];

    if (Particle.bDead == true)
    {
        SpeedData.fGravity = 0.f;
        return;
    }
    SpeedData.fGravity += g_fTimeDelta * 2.6f;
    Particle.vTranslation.y -= 4.8f * SpeedData.fGravity * g_fTimeDelta;
    g_OutputData[iIndex] = Particle;
    g_SpeedData[iIndex].fGravity = SpeedData.fGravity;
}

[numthreads(256, 1, 1)]
void CS_RESET(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    //if (iIndex >= g_iNumInstances)
    //    return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
    VTXINSTANCE_DYNAMIC_DATA SpeedData = g_SpeedData[iIndex];

    Particle.vTranslation = g_InputData[iIndex].vInitTranslation;
    Particle.vPrevPosition = g_InputData[iIndex].vInitTranslation;
    Particle.bDead = false;
    Particle.vLifeTime.x = 0.f;
    SpeedData.fGravity = 0.f;
    SpeedData.fSpeed = float4(0.f, 0.f, 0.f, 0.f);
    Particle.vRight = float4(g_InputData[iIndex].fSize, 0.f, 0.f, 0.f);
    Particle.vUp = float4(0.f, g_InputData[iIndex].fSize, 0.f, 0.f);
    Particle.vLook = float4(0.f, 0.f, g_InputData[iIndex].fSize, 0.f);
    
    g_OutputData[iIndex] = Particle;
    g_SpeedData[iIndex] = SpeedData; 
    g_SpeedData[0].bDead = 1.f;

}

[numthreads(256, 1, 1)]
void CS_RESET_SPEED(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    //if (iIndex >= g_iNumInstances)
    //    return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
    VTXINSTANCE_DYNAMIC_DATA SpeedData = g_SpeedData[iIndex];
    
    if (g_iSpeedType == 0 || g_iSpeedType == 4)
        SpeedData.fSpeed.x = 0.f;
    if (g_iSpeedType == 1 || g_iSpeedType == 4)
        SpeedData.fSpeed.y = 0.f;
    if (g_iSpeedType == 2 || g_iSpeedType == 4)
    {
        SpeedData.fSpeed.z = 0.f;
        Particle.vRight = float4(g_InputData[iIndex].fSize, 0.f, 0.f, 0.f);
        Particle.vUp = float4(0.f, g_InputData[iIndex].fSize, 0.f, 0.f);
        Particle.vLook = float4(0.f, 0.f, g_InputData[iIndex].fSize, 0.f);
    }
    if (g_iSpeedType == 3 || g_iSpeedType == 4)
        SpeedData.fSpeed.w = 0.f; 

    g_OutputData[iIndex] = Particle;
    g_SpeedData[iIndex] = SpeedData; 
}

[numthreads(256, 1, 1)]
void CS_TURBULENCE(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    if (iIndex >= g_iNumInstances)
        return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
    float3 pos = Particle.vTranslation.xyz;

    // 방향 편향 방지 ===
    float2 offset1 = float2(sin(g_TotalTime), cos(g_TotalTime)) * 0.5f;
    float2 offset2 = float2(sin(g_TotalTime * 1.37f), cos(g_TotalTime * 1.91f)) * 0.5f;
    float2 offset3 = float2(sin(g_TotalTime * 0.77f), cos(g_TotalTime * 1.21f)) * 0.5f;
    
    float forceX = (g_NoiseTexture.SampleLevel(g_LinearWrapSampler, pos.yz * 0.1f + offset1, 0).r - 0.5f) * 2.f;
    float forceY = (g_NoiseTexture.SampleLevel(g_LinearWrapSampler, pos.xz * 0.1f + offset2, 0).r - 0.5f) * 2.f;
    float forceZ = (g_NoiseTexture.SampleLevel(g_LinearWrapSampler, pos.xy * 0.1f + offset3, 0).r - 0.5f) * 2.f;

    float3 noiseDir = normalize(float3(forceX, forceY, forceZ));

    Particle.vTranslation.xyz += noiseDir * g_TurblunceSpeed * g_fTimeDelta;

    g_OutputData[iIndex] = Particle;
}

[numthreads(256, 1, 1)]
void CS_RESET_DEAD_FLAG(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    if (0 < iIndex)
        return;
    
    g_SpeedData[0].bDead = 1.f;
}
