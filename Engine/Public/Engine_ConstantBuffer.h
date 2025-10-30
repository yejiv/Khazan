#pragma once

namespace Engine
{
	typedef struct tagPointInstanceConstantBuffer
	{
		_float		fTimeDelta;
		_float3		vPivot;				//16
		_uint		iNumInstances;
		_float3		padding0;			//16

		_uint		iSpeedType;
		_float2		fSpeedRange;

		_float		bIsLoop;			//16 
		_float3		vSpawnRange;
		_float		fTotalTime;
	}POINT_INSTANCE_CB;
}