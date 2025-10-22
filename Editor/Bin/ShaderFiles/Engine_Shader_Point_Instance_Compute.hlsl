struct PARTICLE_PARAMS
{
    float4 vInitTranslation;
    float2 fSize;
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

struct VTXINSTANCE_PARTICLE_SPEED
{
    float4 fSpeed;
    float fGravity;
};

cbuffer CB_PARTICLE : register(b0)
{
    float g_fTimeDelta;
    float3 g_vPivot;
    uint g_iNumInstances;
    float2 g_fScale;
    float Padding0;
    
    uint g_iSpeedType;
    float2 g_fSpeedRange;
    float Padding1;
};

StructuredBuffer<PARTICLE_PARAMS> g_InputData : register(t0);
RWStructuredBuffer<VTXINSTANCE_PARTICLE> g_OutputData : register(u0);
RWStructuredBuffer<VTXINSTANCE_PARTICLE_SPEED> g_SpeedData : register(u1);

uint pcg_hash(uint seed)
{
    uint state = seed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rand_float(uint seed)
{
    return (float) pcg_hash(seed) / 4294967295.0f;
}

float rand_between(float minVal, float maxVal, uint ID)
{
    float t = rand_float(ID);
    return lerp(minVal, maxVal, t);
}

[numthreads(256, 1, 1)]
void CS_MOVE(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= g_iNumInstances)
        return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
    VTXINSTANCE_PARTICLE_SPEED SpeedData = g_SpeedData[iIndex];
	
    Particle.vPrevPosition = Particle.vTranslation.xyz;
    
    //Scale
    Particle.vRight.x += SpeedData.fSpeed.w;
    Particle.vUp.y += SpeedData.fSpeed.w;
    Particle.vLook.z += SpeedData.fSpeed.w;
	
	//Rotation
    if (SpeedData.fSpeed.y)
	{
        float fAngle = SpeedData.fSpeed.y * g_fTimeDelta;
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
	
	//Spread
    vector vMoveDir = vector((Particle.vTranslation.xyz - g_vPivot), 0.f);
    Particle.vTranslation = Particle.vTranslation + normalize(vMoveDir) * SpeedData.fSpeed.x * g_fTimeDelta;    
    
	//MoveLinear
	vMoveDir = vector(0.f, 1.f, 0.f, 0.f);
    Particle.vTranslation = Particle.vTranslation + vMoveDir * SpeedData.fSpeed.z * g_fTimeDelta;
    
    Particle.vLifeTime.x += g_fTimeDelta;

	if (Particle.vLifeTime.x >= Particle.vLifeTime.y
		|| (SpeedData.fSpeed.x < 0 && length(vMoveDir).x < 0.1f))
	{
		Particle.vLifeTime.x = 0.f;
        Particle.vTranslation = g_InputData[iIndex].vInitTranslation;
		Particle.bDead = true;
	}
	
    if (Particle.vRight.x <= 0.f)
    {
        float fScale = (g_InputData[iIndex].fSize.x, g_InputData[iIndex].fSize.y);
        Particle.vRight = float4(fScale, 0.f, 0.f, 0.f);
        Particle.vUp = float4(0.f, fScale, 0.f, 0.f);
        Particle.vLook = float4(0.f, 0.f, fScale, 0.f);
    }
    
    g_OutputData[iIndex] = Particle;
}

[numthreads(256, 1, 1)]
void CS_UPDATE_SPEED(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x; 
    
    VTXINSTANCE_PARTICLE_SPEED SpeedData = g_SpeedData[iIndex];

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
    VTXINSTANCE_PARTICLE_SPEED SpeedData = g_SpeedData[iIndex];

    if (Particle.bDead == true)
    {
        SpeedData.fGravity = 0.f;
        return;
    }
    SpeedData.fGravity += g_fTimeDelta;
    Particle.vTranslation.y -= 4.2f * SpeedData.fGravity * g_fTimeDelta;
    g_OutputData[iIndex] = Particle;
    g_SpeedData[iIndex].fGravity = SpeedData.fGravity;
}

[numthreads(256, 1, 1)]
void CS_RESET(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= g_iNumInstances)
        return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
    VTXINSTANCE_PARTICLE_SPEED SpeedData = g_SpeedData[iIndex];

    Particle.vTranslation = g_InputData[iIndex].vInitTranslation;
    
    Particle.vLifeTime.x = 0.f;
    SpeedData.fGravity = 0.f;
    SpeedData.fSpeed = float4(0.f, 0.f, 0.f, 0.f);
   
    g_OutputData[iIndex] = Particle;
}