struct PARTICLE_PARAMS
{
    float fSpeed;
    float3 vPadding;
    float4 vInitTranslation;
    float4 vDirection;
};

struct VTXINSTANCE_PARTICLE
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
};

cbuffer CB_PARTICLE : register(b0)
{
    float g_fTimeDelta;
    float3 g_vPivot;
    uint g_iNumInstances;
    float3 g_vPadding;
};

StructuredBuffer<PARTICLE_PARAMS> g_InputData : register(t0);
RWStructuredBuffer<VTXINSTANCE_PARTICLE> g_OutputData : register(u0);

[numthreads(256, 1, 1)]
void CS_MOVE(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= g_iNumInstances)
        return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[DTid.x];
    
    Particle.vTranslation += g_InputData[DTid.x].vDirection * g_InputData[DTid.x].fSpeed * g_fTimeDelta;
    Particle.vLifeTime.x += g_fTimeDelta;
    
    if (Particle.vLifeTime.x >= Particle.vLifeTime.y)
    {
        Particle.vLifeTime.x = 0.f;
        Particle.vTranslation = g_InputData[DTid.x].vInitTranslation;
    }
    
    g_OutputData[DTid.x].vTranslation = Particle.vTranslation;
    g_OutputData[DTid.x].vLifeTime = Particle.vLifeTime;
}