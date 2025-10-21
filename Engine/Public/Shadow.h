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
	void Update();
	HRESULT Ready_ShadowLight(SHADOW_LIGHT_DESC LightDesc);

public:
	SHADOW_LIGHT_DESC Get_ShadowLight() { return m_ShadowLight; }
	void Set_ShadowLight(SHADOW_LIGHT_DESC LightDesc) { m_ShadowLight = LightDesc; }

public:
	void Set_CurrentCascade(_uint iIndex) { m_iCurrentCascade = iIndex; }
	_uint Get_NumCascades() { return m_iNumCascade; }
	const _float* Get_Splits() const { return m_Splits.data(); }

public:
	const _float4x4* Get_CurrentLightViewMatrix() const;
	const _float4x4* Get_CurrentLightProjMatrix() const;
	const _float4x4* Get_LightViewMatrices() const { return m_LightViewMatrix.data(); }
	const _float4x4* Get_LightProjMatrices() const { return m_LightProjMatrix.data(); }

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	_float				m_fViewportWidth{}, m_fViewportHeight{};
	_float4x4			m_Matrices[ENUM_CLASS(D3DTS::END)];
	SHADOW_LIGHT_DESC	m_ShadowLight = {};

	// Cascade
	_uint m_iNumCascade = {};
	vector<_float> m_Splits;
	vector<_float4x4> m_LightViewMatrix;
	vector<_float4x4> m_LightProjMatrix;

	vector<array<_float4, 8>> m_FustumCorners = {};

	_uint m_iCurrentCascade = {};

	_float4 m_vLightDir = {};
	_float m_fLamda = {};
	_float m_fCameraNear = {};
	_float m_fCameraFar = {};

public:
	static CShadow* Create(_uint iWinSizeX, _uint iWinSizeY);
	virtual void Free();
};

NS_END