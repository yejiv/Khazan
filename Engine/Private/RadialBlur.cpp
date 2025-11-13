#include "RadialBlur.h"
#include "GameInstance.h"

CRadialBlur::CRadialBlur()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CRadialBlur::Initialize()
{
    m_Desc.vCenterUV = { 0.5f, 0.5f };
    m_Desc.fSampleRadius = 0.05f;
    m_Desc.vMaskRadius = { 0.f, 0.3f };
    m_Desc.fExponent = 1.f;
    m_Desc.iNumSamples = 16;
    m_Desc.fAttenuation = 0.1f;
    m_Desc.fStrength = 0.f;

    return S_OK;
}

void CRadialBlur::Update(_float fTimeDelta)
{
    if (false == m_pGameInstance->isEnableRadialBlur())
        return;
    
    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= m_fDuration)
    {
        m_pGameInstance->Set_EnableRadialBlur(false);
        m_fTimeAcc = 0.f;
        m_Desc.fStrength = 0.f;
        return;
    }

    // Fade Out
    if (m_fTimeAcc > m_vFadeTime.y)
    {
        _float fFadeDuration = m_fDuration - m_vFadeTime.y;
        _float fFadeTimeAcc = m_fTimeAcc - m_vFadeTime.y;
        _float fRatio = (fFadeTimeAcc / fFadeDuration);
        m_Desc.fStrength = 1.f - fRatio;
        m_Desc.fStrength = max(0.f, m_Desc.fStrength);
    }

    // Fade In
    if (m_fTimeAcc < m_vFadeTime.x)
    {
        m_Desc.fStrength = m_fTimeAcc / m_vFadeTime.x;
        m_Desc.fStrength = min(1.f, m_Desc.fStrength);
    }
}

HRESULT CRadialBlur::Bind_RadialBlur_ShaderResources(CShader* pShader)
{
    if (FAILED(pShader->Bind_RawValue("g_vCenterUV", &m_Desc.vCenterUV, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fSampleRadius", &m_Desc.fSampleRadius, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vMaskRadius", &m_Desc.vMaskRadius, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fExponent", &m_Desc.fExponent, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_iNumSamples", &m_Desc.iNumSamples, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fAttenuation", &m_Desc.fAttenuation, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fStrength", &m_Desc.fStrength, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

void CRadialBlur::Set_RadialBlurCenter(_fvector vCenter)
{
    _vector vCenterPos = vCenter;
    vCenterPos = XMVector3TransformCoord(vCenterPos, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
    vCenterPos = XMVector3TransformCoord(vCenterPos, m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));
    _float fU = XMVectorGetX(vCenterPos) * 0.5f + 0.5f;
    _float fV = XMVectorGetY(vCenterPos) * -0.5f + 0.5f;
    m_Desc.vCenterUV = _float2(fU, fV);
}

void CRadialBlur::Start_RadialBlur(_float fDuration, const _float2& vFadeTime, const RADIAL_BLUR_DESC& Desc)
{
    // 강도 조절만, 다른 설정은 유지, 다른 설정 변경하고 싶으면 인자 Desc 추가 후 시작, 타겟 멤버 변수 추가
    m_pGameInstance->Set_EnableRadialBlur(true);
    m_fDuration = fDuration;
    //  m_Desc = Desc;
    m_vFadeTime = vFadeTime;
    m_vFadeTime.y = m_fDuration - m_vFadeTime.y;
}

CRadialBlur* CRadialBlur::Create()
{
    CRadialBlur* pInstance = new CRadialBlur();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CRadialBlur"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRadialBlur::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
