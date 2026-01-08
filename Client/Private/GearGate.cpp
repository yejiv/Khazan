#include "GearGate.h"

#include "GameInstance.h"

#include "Door_Gear.h"

CGearGate::CGearGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CGearGate::CGearGate(const CGearGate& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CGearGate::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CGearGate::Initialize_Clone(void* pArg)
{
    GEARGATE_DESC* pDesc = static_cast<GEARGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::IDLE1;
    m_pModelCom->Set_Animation(ENUM_CLASS((m_eAnimState)));
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(false);
    m_pModelCom->Play_Animation(0.f);
    m_pModelCom->Set_AnimationBlend(true);

    return S_OK;
}

void CGearGate::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CGearGate::Update(_float fTimeDelta)
{
    if (true == m_isUnLock[L] && true == m_isUnLock[R])
    {
        if (ANIM_STATE::IDLE1 == m_eAnimState)
        {
            SoundOnce(TEXT("IP_Gate_Open_Start"), m_pTransformCom->Get_State(STATE::POSITION), nullptr, m_fInteract_Volume * 1.5f);
            SoundLoop(TEXT("IP_Door_Gear_Idle"), m_pTransformCom->Get_State(STATE::POSITION), nullptr, m_fInteract_Volume);

            m_eAnimState = ANIM_STATE::ACTIVATION;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            
            // 예지 레버로 작동해서 열리는 문
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("DoorOpen"), m_pTransformCom->Get_Rotation_Quat(), m_pTransformCom->Get_State(STATE::POSITION));
        }
    }
    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
    }

    __super::Update(fTimeDelta);
}

void CGearGate::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);

    __super::Late_Update(fTimeDelta);
}

HRESULT CGearGate::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CLever : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

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

HRESULT CGearGate::Ready_Components(void* pArg)
{
    GEARGATE_DESC* pDesc = static_cast<GEARGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CGearGate::Ready_PartObjects(void* pArg)
{
    GEARGATE_DESC* pDesc = static_cast<GEARGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    DOOR_GEAR_EVENTID* pEventIDs = { static_cast<DOOR_GEAR_EVENTID*>(pDesc->pOtherDesc) };

    CDoor_Gear::DOOR_GEAR_DESC DoorLeftGearDesc = {};

    DoorLeftGearDesc.eLevel = eLevel;
    DoorLeftGearDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    DoorLeftGearDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Gear_L");

    DoorLeftGearDesc.pUnLock = &m_isUnLock[L];
    DoorLeftGearDesc.iEventID = pEventIDs->iLeftEventID;

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_L"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Door_Gear"), &DoorLeftGearDesc), E_FAIL);

    CDoor_Gear::DOOR_GEAR_DESC DoorRightGearDesc = {};

    DoorRightGearDesc.eLevel = eLevel;
    DoorRightGearDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    DoorRightGearDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Gear_R");

    DoorRightGearDesc.pUnLock = &m_isUnLock[R];
    DoorRightGearDesc.iEventID = pEventIDs->iRightEventID;

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_R"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Door_Gear"), &DoorRightGearDesc), E_FAIL);

    return S_OK;
}

CGearGate* CGearGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGearGate* pInstance = new CGearGate(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGearGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGearGate::Clone(void* pArg)
{
    CGearGate* pInstance = new CGearGate(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGearGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGearGate::Free()
{
    __super::Free();
}
