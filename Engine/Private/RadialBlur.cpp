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
    m_Desc.fSampleRadius = 0.25f;
    m_Desc.vMaskRadius = { 0.f, 0.3f };
    m_Desc.fExponent = 1.f;
    m_Desc.iNumSamples = 8;
    m_Desc.fAttenuation = 1.f;
    m_Desc.fStrength = 1.f;

    return S_OK;
}

void CRadialBlur::Update(_float fTimeDelta)
{
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

void CRadialBlur::Start_RadialBlur(_float fDuration, const RADIAL_BLUR_DESC& Desc)
{
    // 렌더러 레디얼 블러 Enable True
    // 디스크립션 세팅
    // 업데이트에서 시간 누적, 지속 시간 넘어가면 렌더러 레디얼 블러 비활성화
    // 업데이트에서 sin으로 애니메이션 실행
    // 업데이트에서 타겟 아우터 Radius랑 타겟 강도로 보간하기
    // 외각에서부터 가운데로 서서히 집중되듯이
    // 반대로 끝날 때쯤에는 중앙에서부터 퍼지듯이
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
