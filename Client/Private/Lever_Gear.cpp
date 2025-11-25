#include "Lever_Gear.h"

#include "GameInstance.h"

CLever_Gear::CLever_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CLever_Gear::CLever_Gear(const CLever_Gear& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CLever_Gear::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CLever_Gear::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    LEVER_GEAR_DESC* pDesc = static_cast<LEVER_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    _int* pInt = static_cast<_int*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pInt, E_FAIL);

    m_iEventID = *pInt;

    m_eAnimState = ANIM_STATE::IDLE1;
    m_pModelCom->Set_Animation(ANIM_STATE::IDLE1);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(false);
    m_pModelCom->Play_Animation(0.f);
    m_pModelCom->Set_AnimationBlend(true);

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
        m_iEventID = m_pGameInstance->Subscribe_Event<EventGateGear>(ENUM_CLASS(m_eEventType), [&](const EventGateGear& e) { m_EventGate = e; });

    return S_OK;
}

void CLever_Gear::Priority_Update(_float fTimeDelta)
{
}

void CLever_Gear::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);
}

void CLever_Gear::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CLever_Gear::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CLever_Gear::Ready_Components(void* pArg)
{
    LEVER_GEAR_DESC* pDesc = static_cast<LEVER_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

void CLever_Gear::Animation_Update(_float fTimeDelta)
{
    if (m_EventGate.isFirstStep())          // 레버 작동 완료 시
    {
        if (ANIM_STATE::IDLE1 == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::ACTIVATION1;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
    }
}

void CLever_Gear::Animation_Change(_float fTimeDelta)
{
    if (ANIM_STATE::ACTIVATION1 == m_eAnimState)
    {
        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::ACTIVATION2;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(false);

        m_EventGate.isActiveGear1 = true;

        // OPENING 중에는 UI, Player 용 Active 변수는 false, 상자 앞 위치랑 상자 위치 던지기
        m_pGameInstance->Emit_Event<EventGateGear>(ENUM_CLASS(m_eEventType), m_EventGate);
    }
    else if (ANIM_STATE::ACTIVATION2 == m_eAnimState)
    {
        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::IDLE2;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(false);
        //m_pModelCom->Set_AnimationBlend(false);
    }
}

CLever_Gear* CLever_Gear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLever_Gear* pInstance = new CLever_Gear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLever_Gear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLever_Gear::Clone(void* pArg)
{
    CLever_Gear* pInstance = new CLever_Gear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLever_Gear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLever_Gear::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(m_eEventType), m_iEventID);

    __super::Free();
}
