#include "MotionTrail.h"
#include "GameInstance.h"

CMotionTrail::CMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CMotionTrail::CMotionTrail(const CMotionTrail& Prototype)
    : CComponent(Prototype)
    , m_pShader { Prototype.m_pShader }
{
    Safe_AddRef(m_pShader);
}

HRESULT CMotionTrail::Initialize_Prototype()
{
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_MotionTrail.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    return S_OK;
}

HRESULT CMotionTrail::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    MOTIONTRAIL_DESC* pDesc = static_cast<MOTIONTRAIL_DESC*>(pArg);

    m_Desc = *pDesc;

    return S_OK;
}

void CMotionTrail::Update(const vector<vector<_float4x4>>& PartsBoneMatrices, _float4x4 WorldMatrix, _float fTimeDelta)
{
    MOTIONTRAIL_SEGMENT Segment{};
    Segment.PartsBoneMatrices = PartsBoneMatrices;
    Segment.WorldMatrix = WorldMatrix;
    Segment.vLifeTime = m_Desc.vLifeTime;
    Segment.vStartColor = m_Desc.vStartColor;

    m_Snapshot.push_back(Segment);

    // ===== Update =====
    //  float               fInterval;
    //  bool                isIndividualColor;
    //  float               fColorUpdateSpeed;
}

HRESULT CMotionTrail::Render()
{
    // ===== Render =====
    //  _float2             vLifeTime;
    //  _float4             vStartColor;
    //  _float4             vTargetColor;
    //  float               fRimPower;
    //  float               fRimIntensity;
    //  float               fEmissiveIntensity;
    // + CamPosition
    // + Object Render -> Normal Texture

    // 이렇게 하면 시작할 때 개별 색 적용 못함, 나중에 구조화 버퍼로 업데이트에서 갱신하면서 시작 컬러 배열 인덱스로 접근하기
    // 라이프 타임도 다 똑같은데요
    // Life Time, Start Color는 구조화 버퍼로 만들어야 할 듯

    for (auto& Segment : m_Snapshot)
    {
        //  if (FAILED(m_pShader->Bind_Matrices("g_BoneMatrices", Segment.BoneMatrices.data(), g_iMaxNumBones)))
        //      return E_FAIL;

        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &Segment.WorldMatrix)))
            return E_FAIL;
    }

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vLifeTime", &m_Desc.vLifeTime, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vStartColor", &m_Desc.vStartColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vTargetColor", &m_Desc.vTargetColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fRimPower", &m_Desc.fRimPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fRimLightIntensity", &m_Desc.fRimIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fEmissiveIntensity", &m_Desc.fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    m_pShader->Begin(0);

    return S_OK;
}

CMotionTrail* CMotionTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMotionTrail* pInstance = new CMotionTrail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CMotionTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMotionTrail::Clone(void* pArg)
{
    CMotionTrail* pInstance = new CMotionTrail(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CMotionTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMotionTrail::Free()
{
    __super::Free();

    Safe_Release(m_pShader);
}
