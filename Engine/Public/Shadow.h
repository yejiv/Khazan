#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{
private:
	CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShadow() = default;

public:
	HRESULT Initialize();
	void Update();

public:
	HRESULT				Bind_ShadowDSV(_uint iIndex);
	HRESULT				Bind_ShadowSRVArray(class CShader* pShader, const _char* pConstantName);

public:
	void				Set_CurrentCascade(_uint iIndex) { m_iCurrentCascade = iIndex; }
	_uint				Get_NumCascades() { return m_Cascade.iNumCascades; }
	const _float*		Get_Splits() const { return m_Cascade.Splits.data(); }
	void				Set_Splits(const _float* pSplits);
	const _float4x4*	Get_CurrentLightViewMatrix() const;
	const _float4x4*	Get_CurrentLightProjMatrix() const;
	const _float4x4*	Get_LightViewMatrices() const { return m_Cascade.LightViewMatrices.data(); }
	const _float4x4*	Get_LightProjMatrices() const { return m_Cascade.LightProjMatrices.data(); }
	_float				Get_Bias() { return m_fBias; }
	void				Set_Bias(_float fBias) { m_fBias = fBias; }
	_float				Get_Lamda() { return m_fLamda; }
	void				Set_Lamda(_float fLamda);

	// 임시로 추가
	_float4				Get_ShadowLightDir() { return m_vLightDir; }
	void				Set_ShadowLightDir(const _float4 vLightDir) { m_vLightDir = vLightDir; }

public:
	void				Clear_DSVs();

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };

	CASCADE_DATA						m_Cascade;

	_uint								m_iCurrentCascade = {};
	_float4								m_vLightDir = {};
	_float								m_fLamda = {};
	_float								m_fCameraNear{}, m_fCameraFar{};
	_float								m_fBias = {};

private:
	// Resource
	vector<ID3D11DepthStencilView*>		m_ShadowDSVs;
	ID3D11ShaderResourceView*			m_pShadowSRVArray = { nullptr };

private:
	HRESULT Ready_Cascade_Shadow_Resources();

public:
	static CShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};

NS_END