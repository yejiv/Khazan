#include "Door_Gear.h"

#include "GameInstance.h"

CDoor_Gear::CDoor_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject { pDevice, pContext }
{
}

CDoor_Gear::CDoor_Gear(const CDoor_Gear& Prototype)
    : CPartObject { Prototype }
{
}

HRESULT CDoor_Gear::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDoor_Gear::Initialize_Clone(void* pArg)
{
    DOOR_GEAR_DESC* pDesc = static_cast<DOOR_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_NULLPTR(pDesc->pSocketMatrix, E_FAIL);

    m_pSocketMatrix = pDesc->pSocketMatrix;

    m_pUnLock = pDesc->pUnLock;
    m_iEventID = pDesc->iEventID;

    m_eAnimState = ANIM_STATE::IDLE1;
    m_pModelCom->Set_Animation(ANIM_STATE::IDLE1);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(false);
    m_pModelCom->Play_Animation(0.f);
    m_pModelCom->Set_AnimationBlend(true);

    m_pTransformCom->Rotation(XMConvertToRadians(270.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f));

    switch (m_iEventID)
    {
    case 0:
        m_eEventType = EVENT_TYPE::GATE_GEAR0;
        break;
    case 1:
        m_eEventType = EVENT_TYPE::GATE_GEAR1;
        break;
    default:
        m_eEventType = EVENT_TYPE::END;
        break;
    }

    if (EVENT_TYPE::END != m_eEventType)
        m_iEventLisID = m_pGameInstance->Subscribe_Event<EventGateGear>(ENUM_CLASS(m_eEventType), [&](const EventGateGear& e) { m_EventGate = e; });

    // 룬 문자
    m_fMinIntensity = 20.f;
    m_fMaxIntensity = 60.f;
    m_fEmissiveIntensity = m_fMinIntensity;

    return S_OK;
}

void CDoor_Gear::Priority_Update(_float fTimeDelta)
{
}

void CDoor_Gear::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);

    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (_uint i = 0; i < 3; ++i)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));

    m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::EMBARS), TEXT("LeverGear_On_Static"), m_iEffectIdx,
        XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)),
        XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, 1.f));

    if (m_isBlink)
    {
        m_fTimeAcc += fTimeDelta;
        _float fRatio = 0.5f * (1.f + cos(m_fTimeAcc * 5.f));
        m_fEmissiveIntensity = Lerp(m_fMinIntensity, m_fMaxIntensity, fRatio);
    }
}

void CDoor_Gear::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CDoor_Gear::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (GEAR == i)
        {
            CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);
        }
        else if (RUNE == i) // MeshIndex == 1
        {
            _float3 vRuneColor = _float3(0.f, 1.f, 1.5f);
            if (FAILED(m_pShaderCom->Bind_RawValue("g_vRuneColor", &vRuneColor, sizeof(_float3))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &m_fEmissiveIntensity, sizeof(_float))))
                return E_FAIL;

            CHECK_FAILED_ASSERT(m_pShaderCom->Begin(24), E_FAIL);
        }

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CDoor_Gear::Ready_Components(void* pArg)
{
    DOOR_GEAR_DESC* pDesc = static_cast<DOOR_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_Door_Gear"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}


HRESULT CDoor_Gear::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CDoor_Gear::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (GEAR == iMeshIndex)
    {
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
            m_iMtrlFlags |= M_DIFFUSE;
    }
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    if (RUNE == iMeshIndex && true == m_EventGate.isSecondStep())
        m_iMtrlFlags |= M_EMISSIVE;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

void CDoor_Gear::Animation_Update(_float fTimeDelta)
{
    if (m_EventGate.isSecondStep())          // 레버 작동 완료 시
    {
        if (ANIM_STATE::IDLE1 == m_eAnimState)
        {
            m_pGameInstance->PlaySoundOnce(TEXT("IP_Door_Gear_Active.wav"), XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3], nullptr, 0.5f);

            m_eAnimState = ANIM_STATE::ACTIVATION;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            //가동시작!
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("LeverGear_On"), 
                XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)), 
                XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, 1.f));
        }
    }
}

void CDoor_Gear::Animation_Change(_float fTimeDelta)
{
    if (ANIM_STATE::ACTIVATION == m_eAnimState)
    {
        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::IDLE2;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(true);

        *m_pUnLock = true;

        m_EventGate.isActiveGear2 = true;

        m_pGameInstance->Emit_Event<EventGateGear>(ENUM_CLASS(m_eEventType), m_EventGate);
        //가동 끝! 
        m_iEffectIdx = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("LeverGear_On_Static"),
            XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)),
            XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, 1.f));
        
        m_isBlink = true;

        m_pGameInstance->Emit_Event<EVENT_PET_STATE>(ENUM_CLASS(EVENT_TYPE::PET), EVENT_PET_STATE{ false });
    }
}

CDoor_Gear* CDoor_Gear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDoor_Gear* pInstance = new CDoor_Gear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDoor_Gear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDoor_Gear::Clone(void* pArg)
{
    CDoor_Gear* pInstance = new CDoor_Gear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDoor_Gear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDoor_Gear::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(m_eEventType), m_iEventLisID);

    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
