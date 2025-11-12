#include "LUT.h"
#include "GameInstance.h"

CLUT::CLUT(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CLUT::Initialize()
{
    if (FAILED(Ready_LUTTexture()))
        return E_FAIL;

    m_isEnable = true;
    m_Desc.fIntensity = 0.2f;
    m_Desc.iSliceSize = 16;
    m_Desc.vTextureSize = _float2(256.f, 16.f);

    return S_OK;
}

HRESULT CLUT::Bind_LUT_ShaderResources(CShader* pShader)
{
    if (FAILED(pShader->Bind_Bool("g_isEnableLUT", &m_isEnable)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fLUTIntensity", &m_Desc.fIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_iLUTSliceSize", &m_Desc.iSliceSize, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vLUTTextureSize", &m_Desc.vTextureSize, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pTexture->Bind_Shader_Resource(pShader, "g_LUTTexture", 0)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLUT::Ready_LUTTexture()
{
    m_pTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Shader/LUT/LUT_Bypass.png"), 1);
    if (nullptr == m_pTexture)
        return E_FAIL;

    return S_OK;
}

CLUT* CLUT::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLUT* pInstance = new CLUT(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CLUT"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLUT::Free()
{
    __super::Free();

    Safe_Release(m_pTexture);

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}