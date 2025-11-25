#include "Dragonian_Rampage.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Dragonian_Rampage.h"
#include "Dragonian_Claw_L.h"
#include "Dragonian_Claw_R.h"

#include "AI_Controller_Dragonian_Rampage.h"

#include "Mon_Hp.h"

CDragonian_Rampage::CDragonian_Rampage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CDragonian_Rampage::CDragonian_Rampage(const CDragonian_Rampage& Prototype)
    :CMonster(Prototype)
{
}

CDragonian_Rampage::MONDATA& CDragonian_Rampage::Get_Data()
{
    return m_Data;
}

void CDragonian_Rampage::Move_F()
{
    m_pTarget->Get_Position();
    m_pTransformCom->LookAt(m_pTarget->Get_Position());

    _float fWorkSpeed = m_Data.isSlowWalk ? 3.5f : 4.1f;

    m_pTransformCom->AI_Chase(m_pTarget->Get_Position(), m_fTimeDelta, fWorkSpeed);
}

void CDragonian_Rampage::Hp_Visivle(_bool isVisivle)
{
    m_pUI_HP->Update_Visible(isVisivle);
}

void CDragonian_Rampage::Hp_Dead()
{
    m_pUI_HP->Set_IsDead(true);
    Safe_Release(m_pUI_HP);
}

_bool CDragonian_Rampage::Check_AttackRanage(string strKey)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));
    _float fAttackRanage = m_pBlackBoard->Get_Value<_float>(m_strName, strKey);

    if (fAttackRanage <= 0)
        return false;

    if (fDist <= fAttackRanage)
        return true;
    else
        return false;
}

HRESULT CDragonian_Rampage::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDragonian_Rampage::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_MonData(), E_FAIL);
    CHECK_FAILED(Ready_ETC(), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(), E_FAIL);
    m_pHeadMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Head");

    CHECK_FAILED(Ready_AnimEvent(), E_FAIL);
    CHECK_FAILED(Ready_Components(), E_FAIL);


    return S_OK;
}

void CDragonian_Rampage::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);

    //if (m_pGameInstance->Key_Down(DIK_M))
    //{
    //    m_fCurrentHP = m_fMaxHP;
    //    m_Data.isSleep = true;
    //}
    //else if (m_pGameInstance->Key_Down(DIK_V))
    //    Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL, m_pTarget);
    //else if (m_pGameInstance->Key_Down(DIK_B))
    //    Take_Damage(10.f, HITREACTION::BRUTAL_ATTACK, m_pTarget);
    //else if (m_pGameInstance->Key_Down(DIK_N))
    //    m_fCurrentStamina = 0;

}

void CDragonian_Rampage::Update(_float fTimeDelta)
{
    m_fTimeDelta = fTimeDelta;

    if (m_Data.fAttackCool >= 0.f)
        m_Data.fAttackCool -= fTimeDelta;

    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);
    Update_UIHp();
}

void CDragonian_Rampage::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);
}

void CDragonian_Rampage::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CDragonian_Rampage::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CDragonian_Rampage::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}

HRESULT CDragonian_Rampage::Ready_Prototype()
{
    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Rampage"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Rampage/Dragonian_Rampage/Dragonian_Rampage.dat"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_DragonClaw_R"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Rampage/DragonClaw_R/DragonClaw_R.dat"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_DragonClaw_L"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Rampage/DragonClaw_L/DragonClaw_L.dat"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Rampagee_Body"),
        CBody_Dragonian_Rampage::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Claw_L"),
        CDragonian_Claw_L::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Claw_R"),
        CDragonian_Claw_R::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_ETC()
{
    m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));
    CHECK_NULLPTR(m_pUI_HP, E_FAIL);
    Safe_AddRef(m_pUI_HP);
    m_pUI_HP->Setting_HP(&m_vHpPos, { 0.f, 0.f }, &m_fCurrentHP, &m_fMaxHP, &m_fCurrentStamina, &m_fMaxStamina);
    m_pGameInstance->Push_PoolObject_ToLayer(m_iPrototypeIndex, TEXT("Layer_UI"), m_pUI_HP);

    m_pController = CAI_Controller_Dragonian_Rampage::Create(this);
    CHECK_NULLPTR(m_pController, E_FAIL);
    
    m_pBlackBoard = m_pController->Get_BlackBoard();
    Safe_AddRef(m_pBlackBoard);
    m_pBlackBoard->Set_Value<CGameObject*>(m_strName, "Target", m_pTarget);

    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_Components()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.6f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 0.3f;
    tCharVirDesc.fHeight = 0.7f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fPenetrationRecoverySpeed = 0.1f;

    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_PartObjects()
{
    CBody_Dragonian_Rampage::BODY_DESC BodyDesc{};
    BodyDesc.pData = &m_Data;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Rampagee_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Dragonian_Rampage*>(pBody);
    Safe_AddRef(m_pBody);

    CDragonian_Claw_L::WEAPON_DESC WeaponLDesc{};
    WeaponLDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponLDesc.pOwnerTransform = m_pTransformCom;
    WeaponLDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-L-Hand");

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon_L"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Claw_L"), &WeaponLDesc)))
        return E_FAIL;

    CPartObject* pWeapon = Find_PartObject(TEXT("Part_Weapon_L"));
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pClaw_L = dynamic_cast<CDragonian_Claw_L*>(pWeapon);
    Safe_AddRef(m_pClaw_L);

    CDragonian_Claw_R::WEAPON_DESC WeaponRDesc{};
    WeaponRDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponRDesc.pOwnerTransform = m_pTransformCom;
    WeaponRDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-R-Hand");

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon_R"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Claw_R"), &WeaponRDesc)))
        return E_FAIL;

    pWeapon = Find_PartObject(TEXT("Part_Weapon_R"));
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pClaw_R = dynamic_cast<CDragonian_Claw_R*>(pWeapon);
    Safe_AddRef(m_pClaw_L);
    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_AnimEvent()
{
    CModel* pModel = m_pBody->Get_Model();

    pModel->Register_Event("Walk1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isSlowWalk = false; });
    pModel->Register_Event("Walk2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isSlowWalk = false; });
    pModel->Register_Event("Walk3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isSlowWalk = false; });
    pModel->Register_Event("Walk4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isSlowWalk = false; });

    pModel->Register_Event("Walk1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isSlowWalk = true; });
    pModel->Register_Event("Walk2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isSlowWalk = true; });
    pModel->Register_Event("Walk3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isSlowWalk = true; });
    pModel->Register_Event("Walk4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isSlowWalk = true; });

    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_MonData()
{
    m_Data.pOwner = this;
    Safe_AddRef(m_Data.pOwner);

    m_Data.fGloggyTime = 3.f;
    m_Data.pCulHp = &m_fCurrentHP;
    m_Data.pMaxHp = &m_fMaxHP;

    m_Data.pCulStamina = &m_fCurrentStamina;
    m_Data.pMaxStamina = &m_fMaxStamina;

    return S_OK;
}

void CDragonian_Rampage::Update_UIHp()
{
    m_vHpPos = { m_pHeadMatrix->m[3][0], m_pHeadMatrix->m[3][1], m_pHeadMatrix->m[3][2], 1.f };
    XMStoreFloat4(&m_vHpPos, XMVector4Transform(XMLoadFloat4(&m_vHpPos), m_pTransformCom->Get_WorldMatrix()));
    m_vHpPos.y += 0.5f;
}

CDragonian_Rampage* CDragonian_Rampage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CDragonian_Rampage* pInstance = new CDragonian_Rampage(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CDragonian_Rampage"));
    }
    return pInstance;
}

CGameObject* CDragonian_Rampage::Clone(void* pArg)
{
    CDragonian_Rampage* pInstance = new CDragonian_Rampage(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CDragonian_Rampage"));
    }
    return pInstance;
}

void CDragonian_Rampage::Free()
{
    if (m_pUI_HP != nullptr)
    {
        m_pUI_HP->Set_IsDead(true);
        Safe_Release(m_pUI_HP);
    }
    Safe_Release(m_Data.pOwner);

    __super::Free();
    Safe_Release(m_pBody);
    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pClaw_L);
    Safe_Release(m_pClaw_R);

}
