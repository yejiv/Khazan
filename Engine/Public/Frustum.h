#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

	void Transform_ToLocalSpace(_fmatrix WorldMatrix);

	_bool isIn_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	ContainmentType isIn_WorldSpace(const BoundingBox& BoundingBox);
	_bool isIn_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);

	/*XMPlaneTransform(, 월드행렬역전치)*/
	const _float4* Get_Point() const { return m_vPoints; }
	const _float4* Get_WorldPoints() const { return m_vWorldPoints; }
	const _float4* Get_WorldPlanes() const { return m_vWorldPlanes; }
	const _float4* Get_LocalPlanes() const { return m_vLocalPlanes; }

private:
	_float4			m_vPoints[8] = {};
	_float4			m_vWorldPoints[8] = {};
	_float4			m_vWorldPlanes[6] = {};
	_float4			m_vLocalPlanes[6] = {};

	class CGameInstance* m_pGameInstance = { nullptr };

private:
	void Make_Planes(const _float4* pPoints, _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

NS_END