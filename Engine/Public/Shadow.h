#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{
private:
	CShadow();
	virtual ~CShadow() = default;

public:
	const _float4x4* Get_Transform_Float4x4(D3DTS eTransformState) const;

public:
	HRESULT Initialize(_uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_ShadowLight(SHADOW_LIGHT_DESC LightDesc);
	HRESULT Ready_Cascade();

public:
	SHADOW_LIGHT_DESC Get_ShadowLight() { return m_ShadowLight; }
	void Set_ShadowLight(SHADOW_LIGHT_DESC LightDesc) { m_ShadowLight = LightDesc; }

public:
	void Set_CurrentCascade(_uint iIndex) { m_iCurrentCascade = iIndex; }
	_uint Get_NumCascades() { return m_iNumCascade; }

public:
	HRESULT Bind_LightViewProjMatrix(class CShader* pShader, _uint iIndex);
	const _float4x4* Get_CurrentLightViewProjMatrix() const;

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	_float				m_fViewportWidth{}, m_fViewportHeight{};
	_float4x4			m_Matrices[ENUM_CLASS(D3DTS::END)];
	SHADOW_LIGHT_DESC	m_ShadowLight = {};

	// Cascade Test
	_uint m_iNumCascade = {};
	vector<_float> m_Splits;
	vector<_float4x4> m_LightViewProjMatrix;
	//	_float3 m_vWorldPoints[8] = {};
	//	vector<vector<_float3>> m_WorldPoints = {};
	vector<array<_float4, 8>> m_FustumCorners = {};

	_uint m_iCurrentCascade = {};

public:
	static CShadow* Create(_uint iWinSizeX, _uint iWinSizeY);
	virtual void Free();
};

NS_END