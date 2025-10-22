#pragma once

namespace Engine
{
	typedef struct tagPointInstanceConstantBuffer
	{
		_float		fTimeDelta;
		_float3		vPivot;
		_uint		iNumInstances;
		_float3		padding0;
		_uint		iSpeedType;
		_float2		fRange;
		_float		padding;
	}POINT_INSTANCE_CB;
}