#include "VerticalGate.h"

#include "GameInstance.h"

CVerticalGate::CVerticalGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CVerticalGate::CVerticalGate(const CVerticalGate& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CVerticalGate::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CVerticalGate::Initialize_Clone(void* pArg)
{
    VERTICAL_GATE_DESC* pDesc = static_cast<VERTICAL_GATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::IDLE1;
    m_pModelCom->Set_Animation(ENUM_CLASS((ANIM_STATE::IDLE1)));
    m_pModelCom->Set_AnimationBlend(false);
    m_pModelCom->Play_Animation(0.f);
    m_pModelCom->Set_AnimationBlend(true);

    _int* pEventID = static_cast<_int*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pEventID, E_FAIL);

    m_iEventID = *pEventID;

    switch (m_iEventID)
    {
    case 0:
        m_eEventType = EVENT_TYPE::EMBARS_GIMMICK0;
        break;
    case 1:
        m_eEventType = EVENT_TYPE::EMBARS_GIMMICK1;
        break;
    case 2:
        m_eEventType = EVENT_TYPE::EMBARS_GIMMICK2;
        break;
    default:
        m_eEventType = EVENT_TYPE::END;
        break;
    }

    if (EVENT_TYPE::END != m_eEventType)
        m_iSubscribeEventID = m_pGameInstance->Subscribe_Event<EventGimmick>(ENUM_CLASS(m_eEventType), [&](const EventGimmick& e) { m_EventGimmick = e; });

    return S_OK;
}

void CVerticalGate::Priority_Update(_float fTimeDelta)
{

}

void CVerticalGate::Update(_float fTimeDelta)
{
    if (true == m_isSoundStart)
    {
        if (false == IsPlayingSound(TEXT("IP_VG_Open_Start")))
        {
            if (EVENT_TYPE::EMBARS_GIMMICK2 == m_eEventType)
                SoundOnce(TEXT("IP_VG_Open_End"), Get_Position(), nullptr, m_fInteract_Volume);
            else
                SoundOnce(TEXT("IP_VG_Open_End"), m_fInteract_Volume);

            m_isSoundStart = false;
        }
    }

    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);
}

void CVerticalGate::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
}

HRESULT CVerticalGate::Render()
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

HRESULT CVerticalGate::Ready_Components(void* pArg)
{
    VERTICAL_GATE_DESC* pDesc = static_cast<VERTICAL_GATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

void CVerticalGate::Animation_Update(_float fTimeDelta)
{
    CHECK_TRUE(m_isUnLock, );

    if (m_EventGimmick.isActiveGate())
    {
        switch (m_eAnimState)
        {
        case ANIM_STATE::IDLE1:
            m_eAnimState = ANIM_STATE::ACTIVATION;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->AnimationLoop(false);

            if (EVENT_TYPE::EMBARS_GIMMICK2 == m_eEventType)
                SoundOnce(TEXT("IP_VG_Open_Start"), Get_Position(), nullptr, m_fInteract_Volume);
            else
                SoundOnce(TEXT("IP_VG_Open_Start"), m_fInteract_Volume);

            if (false == m_isSoundStart)
            {
                m_isSoundStart = true;
            }

            break;
        case ANIM_STATE::IDLE2:
            m_eAnimState = ANIM_STATE::DEACTIVATION;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            break;
        }

        m_isUnLock = true;
    }
}

void CVerticalGate::Animation_Change(_float fTimeDelta)
{
    switch (m_eAnimState)
    {
    case ANIM_STATE::ACTIVATION:
        m_eAnimState = ANIM_STATE::IDLE2;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->AnimationLoop(false);
        break;
    case ANIM_STATE::DEACTIVATION:
        m_eAnimState = ANIM_STATE::IDLE1;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->Set_AnimationLoop(false);
        break;
    }
}

CVerticalGate* CVerticalGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVerticalGate* pInstance = new CVerticalGate(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CVerticalGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CVerticalGate::Clone(void* pArg)
{
    CVerticalGate* pInstance = new CVerticalGate(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CVerticalGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVerticalGate::Free()
{
    if (EVENT_TYPE::END != m_eEventType)
        m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(m_eEventType), m_iSubscribeEventID);

    __super::Free();
}
