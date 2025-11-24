#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCSM final : public CBase
{
private:
    CCSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CCSM() = default;

public:
    HRESULT             Initialize();
    void                Update(_float fTimeDelta);

public:
    HRESULT				Bind_CascadeDSV(_uint iIndex);
    HRESULT				Bind_Cascade_ShaderResources(class CShader* pShader);
    void				Clear_DSVs();
    void				Start_CascadeShadowTransition(_float fDuration, _float fTargetIntensity);

public:
    void				Set_CurrentCascade(_uint iIndex) { m_iCurrentCascade = iIndex; }
    _uint				Get_NumCascades() { return m_Cascade.iNumCascades; }
    const _float4x4*    Get_CurrentLightViewMatrix() const;
    const _float4x4*    Get_CurrentLightProjMatrix() const;

    CASCADE_CONFIG		Get_CascadeConfig() { return m_Config; }
    void				Set_CascadeConfig(CASCADE_CONFIG Config);

#ifdef _DEBUG
public:
    HRESULT				Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
    HRESULT				Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
    ID3D11Device*                       m_pDevice = { nullptr };
    ID3D11DeviceContext*                m_pContext = { nullptr };
    class CGameInstance*                m_pGameInstance = { nullptr };

    CASCADE_DATA						m_Cascade = {};		// Cascade 필수 구성 요소
    CASCADE_CONFIG						m_Config = {};		// Cascade 수치 조절용 설정 정보

    _uint								m_iCurrentCascade = {};
    _float								m_fCameraNear{}, m_fCameraFar{};
    _float4                             m_vFrustumWorldPoints[8] = {};
    array<_float4, 8>                   m_FrustumCorners = {};

    _bool								m_isTransition = {};
    _float								m_fTransTimeAcc = {};
    _float								m_fDuration = {};
    _float								m_fTargetIntensity = {};

private:
    vector<ID3D11DepthStencilView*>		m_ShadowDSVs;
    ID3D11ShaderResourceView*           m_pShadowSRVArray = { nullptr };

#ifdef _DEBUG
private:
    vector<_float4x4>					m_WorldMatrices = {};
#endif

private:
    HRESULT				                Ready_ShaderResources();

public:
    static CCSM*                        Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void		                Free();
};

NS_END