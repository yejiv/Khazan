#include "MotionTrail.h"
#include "GameInstance.h"

CMotionTrail::CMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
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

    m_pOwnerMasterModel = pDesc->pOwnerMasterModel;
    Safe_AddRef(m_pOwnerMasterModel);

    m_HasPartModels = pDesc->HasPartModels;

    if (m_HasPartModels)
    {
        m_OwnerPartModels = pDesc->OwnerPartModels;
        for (auto& pModel : m_OwnerPartModels)
            Safe_AddRef(pModel);
    }

    m_Config = pDesc->Config;

    return S_OK;
}

void CMotionTrail::Update(_float fTimeDelta)
{
    // 인터벌 간격으로 프레임 스냅샷 캡처
    // 스냅샷 생명 주기와 프레임 히스토리 관리

    if (true == m_isEnable)
    {
        // 지정한 지속 시간만큼 활성화하는 모드, 아닐 땐 지속적으로 활성화
        if (true == m_isDurationMode)
        {
            m_fDurationTimeAcc += fTimeDelta;

            if (m_fDurationTimeAcc >= m_fDuration)
            {
                m_isEnable = false;
                m_isDurationMode = false;
            }
        }

        m_fCaptureTimeAcc += fTimeDelta;

        // 인터벌 시간 충족, 활성화 상태, 최대 프레임 개수보다 기록된 프레임 수가 작을 때만 스냅샷 캡처
        if (m_fCaptureTimeAcc >= m_Config.fInterval && m_FrameHistory.size() < m_Config.iMaxFrames)
        {
            FRAME_SNAPSHOT Snapshot;
            m_pOwnerMasterModel->Capture_CurrentFrameMatrices(Snapshot.BoneCombinedMatrices, &Snapshot.OwnerWorldMatrix);
            Snapshot.vLifeTime = m_Config.vLifeTime;

            if (false == m_Config.isIndividualColor)
            {
                m_fColorTimeAcc += fTimeDelta * m_Config.fColorUpdateSpeed;

                _float fColorRatio = cosf(XMConvertToRadians(m_fColorTimeAcc)) * 0.5f + 0.5f;

                _vector vStartColor = XMLoadFloat3(&m_Config.vStartColor);
                _vector vTargetColor = XMLoadFloat3(&m_Config.vTargetColor);

                _vector vFinalColor = XMVectorLerp(vStartColor, vTargetColor, fColorRatio);

                _float3 vResultColor{};
                XMStoreFloat3(&vResultColor, vFinalColor);

                Snapshot.vStartColor = vResultColor;
                Snapshot.vTargetColor = vResultColor;
            }
            else
            {
                Snapshot.vStartColor = m_Config.vStartColor;
                Snapshot.vTargetColor = m_Config.vTargetColor;
            }

            m_FrameHistory.push_back(Snapshot);

            m_fCaptureTimeAcc = 0.f;
        }
    }

     for (auto& Snapshot : m_FrameHistory)
        Snapshot.vLifeTime.x += fTimeDelta;

     if (!m_FrameHistory.empty())
     {
         if (m_FrameHistory.front().vLifeTime.x >= m_FrameHistory.front().vLifeTime.y)
            m_FrameHistory.pop_front();

         if (FAILED(m_pGameInstance->Add_RenderComponent(this)))
            return;
     }
}

void CMotionTrail::Start_MotionTrail(_float fDuration)
{
    if (true == m_isDurationMode)
        return;

    m_isEnable = true;
    m_isDurationMode = true;
    m_fDuration = fDuration;
    m_fDurationTimeAcc = 0.f;
}

HRESULT CMotionTrail::Render()
{
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fRimPower", &m_Config.fRimPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fRimLightIntensity", &m_Config.fRimIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fEmissiveIntensity", &m_Config.fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    // 백업
    m_pOwnerMasterModel->Cache_CurrentBoneMatrices();

    for (size_t i = 0; i < m_FrameHistory.size(); ++i)
    {
        if (m_pOwnerMasterModel->Restore_Frame(m_FrameHistory[m_FrameHistory.size() - 1 - i].BoneCombinedMatrices))
        {
            if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_FrameHistory[m_FrameHistory.size() - 1 - i].OwnerWorldMatrix)))
                return E_FAIL;

            if (FAILED(m_pShader->Bind_RawValue("g_vLifeTime",
                &m_FrameHistory[m_FrameHistory.size() - 1 - i].vLifeTime, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShader->Bind_RawValue("g_vStartColor",
                &m_FrameHistory[m_FrameHistory.size() - 1 - i].vStartColor, sizeof(_float3))))
                return E_FAIL;

            if (FAILED(m_pShader->Bind_RawValue("g_vTargetColor",
                &m_FrameHistory[m_FrameHistory.size() - 1 - i].vTargetColor, sizeof(_float3))))
                return E_FAIL;

            // 본 행렬 바인딩 (복원된 상태로)
            _uint iNumMeshes = m_pOwnerMasterModel->Get_NumMeshes();

            for (_uint j = 0; j < iNumMeshes; ++j)
            {
                if (FAILED(m_pOwnerMasterModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", j)))
                    continue;
            }

            for (auto& pModel : m_OwnerPartModels)
                Render_PartModel(pModel);
        }
    }

    // 복원
    m_pOwnerMasterModel->Restore_CurrentBoneMatrices();

    return S_OK;
}

HRESULT CMotionTrail::Render_PartModel(CModel* pModel)
{
    if (false == m_HasPartModels || nullptr == pModel)
        return E_FAIL;

    pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        pModel->Bind_Materials(m_pShader, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", i)))
            continue;

        m_pShader->Begin(0);
        pModel->Render(i);
    }

    return S_OK;
}

CMotionTrail* CMotionTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMotionTrail* pInstance = new CMotionTrail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMotionTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMotionTrail::Clone(void* pArg)
{
    CMotionTrail* pInstance = new CMotionTrail(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CMotionTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMotionTrail::Free()
{
    __super::Free();

    for (auto& pModel : m_OwnerPartModels)
        Safe_Release(pModel);

    Safe_Release(m_pOwnerMasterModel);

    Safe_Release(m_pShader);
}
