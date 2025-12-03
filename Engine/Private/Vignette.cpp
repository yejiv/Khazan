#include "Vignette.h"
#include "Shader.h"

CVignette::CVignette()
{
}

HRESULT CVignette::Initialize()
{
    m_Config.vColor = _float3(0.f, 0.f, 0.f);
    m_Config.fPower = 4.f;
    m_Config.fIntensity = 1.f;

    m_Config.fMaxIntensity = 5.f;

    return S_OK;
}

void CVignette::Update(_float fTimeDelta)
{
    if (false == m_isEnable)
        return;

    // 시간 누적
    m_fTimeAcc += fTimeDelta;

    // 누적 시간이 Duration을 넘어갔을 경우 비활성화, Intensity 0 초기화
    if (m_fDuration <= m_fTimeAcc)
    {
        m_isEnable = false;
        m_fTimeAcc = 0.f;
        m_Config.fIntensity = 0.f;
    }
    else
    {
        _float fRatio = m_fTimeAcc / m_fDuration;
        _float fProgress = 0.f;

        switch (m_Config.eMode)
        {
        case VIGNETTE_CONFIG::ANIMMODE::SMOOTH_SMOOTH:
            fProgress = sin(fRatio * PI);
            break;
        case VIGNETTE_CONFIG::ANIMMODE::SMOOTH_INTANT:
            fProgress = fRatio;
            break;
        case VIGNETTE_CONFIG::ANIMMODE::INTANT_SMOOTH:
            fProgress = 1.f - fRatio;
            break;
        case VIGNETTE_CONFIG::ANIMMODE::NONE:
            fProgress = m_Config.fMaxIntensity;
            break;
        }
        
        m_Config.fIntensity = m_Config.fMaxIntensity * fProgress;
    }

    // 아닐 경우 Ratio = 누적 시간 / 지속 시간
    // SS -> 0 -> 1 -> 0 -> sin(t * PI)
    // SI -> 0 -> 1 -> t
    // IS -> 1 -> 0 -> 1 - t
    // Intensity = Max Intensity * Progress
}

HRESULT CVignette::Bind_Vignette_ShaderResources(CShader* pShader)
{
    if (FAILED(pShader->Bind_RawValue("g_fVignettePower", &m_Config.fPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fVignetteIntensity", &m_Config.fIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vVignetteColor", &m_Config.vColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(pShader->Bind_Bool("g_isEnableVignette", &m_isEnable)))
        return E_FAIL;

    return S_OK;
}

void CVignette::Start_VignetteAnimation(_float fDuration, const VIGNETTE_CONFIG& Config)
{
    m_isEnable = true;
    m_fDuration = fDuration;
    m_Config = Config;
}

CVignette* CVignette::Create()
{
    CVignette* pInstance = new CVignette();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CVignette"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVignette::Free()
{
    __super::Free();
}
