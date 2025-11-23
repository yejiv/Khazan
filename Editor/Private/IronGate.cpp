#include "IronGate.h"

#include "GameInstance.h"

#include "IronGate_Lock.h"
#include "IronGate_Part_L.h"
#include "IronGate_Part_R.h"

CIronGate::CIronGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CIronGate::CIronGate(const CIronGate& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CIronGate::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CIronGate::Initialize_Clone(void* pArg)
{
    IRONGATE_DESC* pDesc = static_cast<IRONGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::ACTIVATION;
    m_pModelCom->Set_Animation(ENUM_CLASS((m_eAnimState)));
    m_pModelCom->Set_AnimationLoop(false);
    m_pModelCom->Set_AnimationBlend(false);

    m_pModelCom->Play_Animation(0.f);

    return S_OK;
}

void CIronGate::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CIronGate::Update(_float fTimeDelta)
{
    if (false == m_isUnLock && m_pGameInstance->Key_Down(DIK_6))
    {
        m_isUnLock = true;
    }

    if (true == m_isUnLock)
        m_pModelCom->Play_Animation(fTimeDelta);

    if (ANIM_STATE::ACTIVATION != m_eAnimState)
        m_isState = true;
    else
        m_isState = false;

    __super::Update(fTimeDelta);
}

void CIronGate::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CIronGate::Render()
{
    return S_OK;
}

HRESULT CIronGate::Ready_Components(void* pArg)
{
    IRONGATE_DESC* pDesc = static_cast<IRONGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CIronGate::Ready_PartObjects(void* pArg)
{
    IRONGATE_DESC* pDesc = static_cast<IRONGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CIronGate_Lock::IRONGATE_LOCK_DESC PartLockDesc = {};

    PartLockDesc.eLevel = eLevel;
    PartLockDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartLockDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("LockLever");

    PartLockDesc.pUnLock = &m_isUnLock;
    PartLockDesc.pState = &m_isState;

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Lock"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_IronGate_Lock"), &PartLockDesc), E_FAIL);

    CIronGate_Part_L::IRONGATE_PART_LEFT_DESC PartLeftDesc = {};

    PartLeftDesc.eLevel = eLevel;
    PartLeftDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartLeftDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Door_Pivot_L");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gate_L"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_IronGate_Part_L"), &PartLeftDesc), E_FAIL);

    CIronGate_Part_R::IRONGATE_PART_RIGHT_DESC PartRightDesc = {};

    PartRightDesc.eLevel = eLevel;
    PartRightDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartRightDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Door_Pivot_R");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gate_R"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_IronGate_Part_R"), &PartRightDesc), E_FAIL);

    return S_OK;
}

CIronGate* CIronGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CIronGate* pInstance = new CIronGate(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CIronGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CIronGate::Clone(void* pArg)
{
    CIronGate* pInstance = new CIronGate(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CIronGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIronGate::Free()
{
    __super::Free();
}
