#include "Vignette.h"
#include "Shader.h"

CVignette::CVignette()
{
}

HRESULT CVignette::Initialize()
{
    m_Config.vColor = _float3(0.f, 0.f, 0.f);
    m_Config.fPower = 1.f;
    m_Config.fIntensity = 1.f;

    return S_OK;
}

void CVignette::Update(_float fTimeDelta)
{
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
