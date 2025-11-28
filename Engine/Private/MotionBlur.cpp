#include "MotionBlur.h"
#include "GameInstance.h"

CMotionBlur::CMotionBlur()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMotionBlur::Initialize()
{
    m_Desc.fDepthBias = 0.001f;
    m_Desc.iNumSamples = 16;
    m_Desc.fStrength = 0.45f;

    return S_OK;
}

HRESULT CMotionBlur::Bind_MotionBlur_ShaderResources(CShader* pShader)
{
    // 이전 뷰 투영 행렬
    if (FAILED(pShader->Bind_Matrix("g_PrevViewMatrix", m_pGameInstance->Get_PrevTransform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(pShader->Bind_Matrix("g_PrevProjMatrix", m_pGameInstance->Get_PrevTransform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    // 모션 블러 정보
    if (FAILED(pShader->Bind_RawValue("g_iNumMotionBlurSamples", &m_Desc.iNumSamples, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fMotionBlurBias", &m_Desc.fDepthBias, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fMotionBlurStrength", &m_Desc.fStrength, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_Bool("g_isEnableMotionBlur", &m_isEnable)))
        return E_FAIL;

    return S_OK;
}

CMotionBlur* CMotionBlur::Create()
{
    CMotionBlur* pInstance = new CMotionBlur();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CMotionBlur"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMotionBlur::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}