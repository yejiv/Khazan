#include "Fog.h"
#include "Shader.h"

CFog::CFog()
{
}

HRESULT CFog::Initialize()
{
    m_Config.eType = FOG_CONFIG::LINEAR;
    m_Config.fNear = 0.1f;
    m_Config.fFar = 100.f;
    m_Config.fDensity = 0.0001f;
    m_Config.vColor = { 1.f, 1.f, 1.f, 1.f };

	return S_OK;
}

HRESULT CFog::Bind_Fog_ShaderResources(CShader* pShader)
{
    _uint iFogMode = static_cast<_uint>(m_Config.eType);
    if (FAILED(pShader->Bind_RawValue("g_iFogMode", &iFogMode, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fFogDensity", &m_Config.fDensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fFogNear", &m_Config.fNear, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fFogFar", &m_Config.fFar, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vFogColor", &m_Config.vColor, sizeof(_float4))))
        return E_FAIL;

	return S_OK;
}

CFog* CFog::Create()
{
    CFog* pInstance = new CFog();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CFog"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFog::Free()
{
    __super::Free();
}
