#pragma once

namespace Engine
{
	typedef struct tagPointInstanceConstantBuffer
	{
		_float		fTimeDelta;
		_float3		vPivot;
		_uint		iNumInstances;
		_float3		vPadding;
	}POINT_INSTANCE_CB;
}