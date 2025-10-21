struct PARTICLE_PARAMS
{
    float4 fSpeed;
    float4 vInitTranslation;
};

struct VTXINSTANCE_PARTICLE
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float3 vPrevPosition;
    float  bDead;
    float2 vLifeTime;
};

struct PARTICLE_SPEED_PARAMS
{
    float4 fSpeed;
};

struct PARTICLE_GRAVITY_PARAMS
{
    float fGravity;
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
RWStructuredBuffer<PARTICLE_GRAVITY_PARAMS> g_GravityData : register(u1);
RWStructuredBuffer<PARTICLE_SPEED_PARAMS> g_SpeedData : register(u2);

[numthreads(256, 1, 1)]
void CS_MOVE(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= g_iNumInstances)
        return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
	
    Particle.vPrevPosition = Particle.vTranslation.xyz;
	
    //Scale
    Particle.vRight.x += g_InputData[iIndex].fSpeed.w;
    Particle.vUp.y += g_InputData[iIndex].fSpeed.w;
    Particle.vLook.z += g_InputData[iIndex].fSpeed.w;
	
	//Rotation
    if (g_InputData[iIndex].fSpeed.y)
	{
        float fAngle = g_InputData[iIndex].fSpeed.y * g_fTimeDelta;
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
    vector vMoveDir = vector((Particle.vTranslation.xyz - g_vPivot.xyz), 0.f);
    Particle.vTranslation = Particle.vTranslation + normalize(vMoveDir) * g_InputData[iIndex].fSpeed.x * g_fTimeDelta;
    
	//MoveLinear
	vMoveDir = vector(0.f, 1.f, 0.f, 0.f);
    Particle.vTranslation = Particle.vTranslation + vMoveDir * g_InputData[iIndex].fSpeed.z * g_fTimeDelta;
    
    Particle.vLifeTime.x += g_fTimeDelta;

	if (Particle.vLifeTime.x >= Particle.vLifeTime.y
		|| (g_InputData[iIndex].fSpeed.x < 0 && length(vMoveDir).x < 0.1f))
	{
		Particle.vLifeTime.x = 0.f;
        Particle.vTranslation = g_InputData[iIndex].vInitTranslation;
		Particle.bDead = true;
	}
	
    g_OutputData[iIndex] = Particle;
    
	//if (Particle.vRight.x <= 0.f)
	//{
	//	float fScale = m_pGameInstance->Rand(m_fScale.x, m_fScale.y);
	//	Particle.vRight = float4(fScale, 0.f, 0.f, 0.f);
	//	Particle.vUp = float4(0.f, fScale, 0.f, 0.f);
	//	Particle.vLook = float4(0.f, 0.f, fScale, 0.f);
	//}
}

[numthreads(256, 1, 1)]
void CS_UPDATE_SPEED(uint3 DTid : SV_DispatchThreadID)
{
}

[numthreads(256, 1, 1)]
void CS_UPDATE_GRAVITY(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= g_iNumInstances)
        return;
    
    VTXINSTANCE_PARTICLE Particle = g_OutputData[iIndex];
    PARTICLE_GRAVITY_PARAMS fVelocity = g_GravityData[iIndex];
    
    if (Particle.bDead == true)
    {
        fVelocity.fGravity = 0.f;
        return;
    }
    fVelocity.fGravity += g_fTimeDelta;
    Particle.vTranslation.y -= 4.2f * fVelocity.fGravity * g_fTimeDelta;
    
    g_OutputData[iIndex].vTranslation.y = Particle.vTranslation.y;
    g_GravityData[iIndex].fGravity = fVelocity.fGravity;
}