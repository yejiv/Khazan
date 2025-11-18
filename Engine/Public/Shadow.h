#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CShadow final : public CBase
{
private:
	CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CShadow() = default;

public:
	HRESULT             Initialize();
	void                Update(_float fTimeDelta);

public:
    const _float4x4*    Get_ShadowLightMatrix(D3DTS eTransformState) const;

public:
    HRESULT             Bind_Shadow_ShaderResources(class CShader* pShader);
    void                Bind_ShadowDSV();
    void                Start_ShadowTransition(_float fDuration, _float fTargetIntensity);
    void                Clear_DSV();

#ifdef _DEBUG
public:
	HRESULT				Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT				Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
    ID3D11DepthStencilView*             m_pShadowDSV = { nullptr };
    ID3D11ShaderResourceView*           m_pShadowSRV = { nullptr };

    _float4x4                           m_WorldMatrix = {};
    _float4x4                           m_LightMatrices[ENUM_CLASS(D3DTS::END)] = {};
    _float4                             m_vFrustumCorners[8] = {};
    _float                              m_fCameraNear{}, m_fCameraFar{};
    _float                              m_fSplit = {};
    _float3                             m_vLightDir = {};
    _float                              m_fBias = {};
    _float                              m_fIntensity = {};

	_bool								m_isTransition = {};
	_float								m_fTransTimeAcc = {};
	_float								m_fDuration = {};
	_float								m_fTargetIntensity = {};

private:
	HRESULT				Ready_ShaderResources();

public:
	static CShadow*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free();
};

NS_END