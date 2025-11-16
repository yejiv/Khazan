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
		_float		fTotalTime;		    //16 

		_float		fTurbulenceSpeed;
		_float		fTurbulenceSampleSize;
		_float2 	padding;		    //16 
	}POINT_INSTANCE_CB;
}