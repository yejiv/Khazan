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
    m_Desc.fDuration = 2.f;
    m_Desc.vFadeTime = { 0.3f, 1.f };

    return S_OK;
}

void CRadialBlur::Update(_float fTimeDelta)
{
    if (false == m_isEnable)
        return;
    
    m_fTimeAcc += fTimeDelta;
    
    if (m_Desc.vCenterUV.x < 0.f || m_Desc.vCenterUV.x > 1.f || m_Desc.vCenterUV.y < 0.f || m_Desc.vCenterUV.y > 1.f)
    {
        // Fade Out
        if (m_fTimeAcc > m_Desc.vFadeTime.y)
        {
            _float fFadeDuration = m_Desc.fDuration - m_Desc.vFadeTime.y;
            _float fFadeTimeAcc = m_fTimeAcc - m_Desc.vFadeTime.y;
            _float fRatio = (fFadeTimeAcc / fFadeDuration);
            m_Desc.fStrength = m_fTargetStrength - fRatio;
            m_Desc.fStrength = max(0.f, m_Desc.fStrength);
        }
    }
    else
    {
        if (m_fTimeAcc >= m_Desc.fDuration)
        {
            m_isEnable = false;
            m_fTimeAcc = 0.f;
            m_Desc.fStrength = 0.f;
            return;
        }

        // Fade Out
        if (m_fTimeAcc > m_Desc.vFadeTime.y)
        {
            _float fFadeDuration = m_Desc.fDuration - m_Desc.vFadeTime.y;
            _float fFadeTimeAcc = m_fTimeAcc - m_Desc.vFadeTime.y;
            _float fRatio = (fFadeTimeAcc / fFadeDuration);
            m_Desc.fStrength = m_fTargetStrength - fRatio;
            m_Desc.fStrength = max(0.f, m_Desc.fStrength);
        }

        // Fade In
        if (m_fTimeAcc < m_Desc.vFadeTime.x)
        {
            m_Desc.fStrength = m_fTimeAcc / m_Desc.vFadeTime.x;
            m_Desc.fStrength = min(m_fTargetStrength, m_Desc.fStrength);
        }
    }
}

HRESULT CRadialBlur::Bind_RadialBlur_ShaderResources(CShader* pShader)
{
    if (FAILED(pShader->Bind_Bool("g_isEnableRadialBlur", &m_isEnable)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vRadialBlurCenterUV", &m_Desc.vCenterUV, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fRadialBlurRadius", &m_Desc.fSampleRadius, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vRadialBlurMaskRadius", &m_Desc.vMaskRadius, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fRadialBlurExp", &m_Desc.fExponent, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_iNumRadialBlurSamples", &m_Desc.iNumSamples, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fRadialBlurAtt", &m_Desc.fAttenuation, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fRadialBlurStrength", &m_Desc.fStrength, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

void CRadialBlur::Set_RadialBlurCenter(_fvector vCenter, _float fOffset)
{
    _vector vCenterPos = vCenter;
    vCenterPos = XMVectorAdd(vCenterPos, XMVectorSet(0.f, fOffset, 0.f, 0.f));
    vCenterPos = XMVector3TransformCoord(vCenterPos, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
    vCenterPos = XMVector3TransformCoord(vCenterPos, m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));
    _float fU = XMVectorGetX(vCenterPos) * 0.5f + 0.5f;
    _float fV = XMVectorGetY(vCenterPos) * -0.5f + 0.5f;
    m_Desc.vCenterUV = _float2(fU, fV);
}

void CRadialBlur::Start_RadialBlur(const RADIAL_BLUR_DESC& Desc)
{
    // 강도 조절만, 다른 설정은 유지, 다른 설정 변경하고 싶으면 인자 Desc 추가 후 시작, 타겟 멤버 변수 추가
    m_isEnable = true;
    m_Desc = Desc;
    m_fTargetStrength = Desc.fStrength;
    m_Desc.vFadeTime.y = m_Desc.fDuration - m_Desc.vFadeTime.y; // 페이드 아웃 시작 시간으로 변경
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
