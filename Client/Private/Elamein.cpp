#include "Elamein.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Elamein.h"
#include "Elamein_Sword.h"
#include "Elamein_Shield.h"

#include "AI_Controller_Elamein.h"

#include "Mon_Hp.h"
#include "MeshTrail.h"

#include "UI_Talk_Danjinjar.h"

CElamein::CElamein(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CElamein::CElamein(const CElamein& Prototype)
    :CMonster(Prototype)
{
}

void CElamein::LockOnLerp(_float fTimeDetla, _float fSpeed)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDetla, fSpeed);
}

void CElamein::LockOn()  
{
    m_pTransformCom->LookAt(m_pTarget->Get_Position());
}

CElamein::MONDATA& CElamein::Get_Data()
{
    return m_Data;
}

void CElamein::Move_F()
{
    m_pTarget->Get_Position();
    m_pTransformCom->LookAt(m_pTarget->Get_Position());

    _float fWorkSpeed = 4.1f;

    m_pTransformCom->AI_Chase(m_pTarget->Get_Position(), m_fTimeDelta, fWorkSpeed);
}

void CElamein::Hp_Visivle(_bool isVisivle)
{
    m_pUI_HP->Update_Visible(isVisivle);
}

void CElamein::Hp_Dead()
{
    m_pUI_HP->Set_IsDead(true);
    m_pUI_HP = nullptr;
}

_bool CElamein::Check_Ranage(string strKey)
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

_bool CElamein::Check_Ranage(_float fRange)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));

    if (fRange <= 0)
        return true;

    if (fDist <= fRange)
        return true;
    else
        return false;
}

TARGET_DIR CElamein::Get_DIR()
{
    return Check_Dir(m_pTransformCom->Get_WorldMatrix(), m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
}

void CElamein::Add_Charge(_float fValue)
{
    m_pShield->Add_Charge(fValue);
    m_pSword->Add_Charge(fValue);
}

void CElamein::Reset_Charge()
{
    m_pShield->Reset_Charge();
    m_pSword->Reset_Charge();
}

_float CElamein::Get_TrackPotion()
{
    return m_pBody->Get_CulTrack();
}

void CElamein::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
    if (m_Data.fDodgeCool <= 0.f && m_Data.eHitType != HITREACTION::BRUTAL_ATTACK && m_Data.eAttackState == ATTACKSTATE::END)
    {
        m_pController->AI_ApplyDamage(pGameObject, fDamage, ENUM_CLASS(eHitreaction), 3.f);
    }
    else
    {
        if (m_Data.eHitType == HITREACTION::BRUTAL_ATTACK)
            ++m_Data.iBrutalHit;

        __super::Take_Damage(fDamage, eHitreaction, pGameObject);
    }
}

void CElamein::Creature_Release()
{
    m_isHit = false;
    m_pHitBodyCom->Collision_Active(m_isHit);

    __super::Creature_Release();
}

HRESULT CElamein::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CElamein::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_MonData(), E_FAIL);
    CHECK_FAILED(Ready_ETC(), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(), E_FAIL);
    m_pHeadMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Head");
    m_pBodySocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Spine2");
    m_pLeftLegSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-R-Foot");
    m_pLockOnSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Spine2");
    m_vLockOnPosition = &m_vLockOnPos;

    CHECK_FAILED(Ready_AnimEvent(), E_FAIL);
    CHECK_FAILED(Ready_Components(), E_FAIL);


    CMeshTrail::TRAIL_DESC MeshDsc;
    MeshDsc.iTextureIdx = 9;
    MeshDsc.fLifeTime = .25f;
    MeshDsc.iDivisionCount = 10.f;

    m_pMeshTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDsc));

    CUIObject::UIOBJECT_DESC Desc;

    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 3.625f, 1.f };
    Desc.szName = "Dangin_TalkUI";
    m_pTalk = static_cast<CUI_Talk_Danjinjar*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_TalkDanjinjar"), &Desc));
    CHECK_NULLPTR(m_pTalk, E_FAIL);

    return S_OK;
}

void CElamein::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);

    m_pMeshTrail->Priority_Update(fTimeDelta);
    m_pTalk->Priority_Update(fTimeDelta);
}

void CElamein::Update(_float fTimeDelta)
{
    m_fTimeDelta = fTimeDelta;

    if (m_Data.fAttackCool > 0.f)
        m_Data.fAttackCool -= fTimeDelta;

    if (m_Data.fGuardCool > 0.f)
        m_Data.fGuardCool -= fTimeDelta;

    if (m_Data.fSpecial_AttackCool > 0.f)
        m_Data.fSpecial_AttackCool -= fTimeDelta;

    if (m_Data.fLong_AttackCool > 0.f)
        m_Data.fLong_AttackCool -= fTimeDelta;

    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);
    Update_UIHp();
    Update_Body(fTimeDelta);


    _float4x4 LockOnMatrix{};
    XMStoreFloat4x4(&LockOnMatrix, XMLoadFloat4x4(m_pLockOnSocketMatrix) * m_pTransformCom->Get_WorldMatrix());
    m_vLockOnPos = { LockOnMatrix._41, LockOnMatrix._42, LockOnMatrix._43, 1.f };

    _float4x4 m_vSwordMat = m_pSword->Get_CombindMat();
    _vector m_vRot = { m_vSwordMat._21, m_vSwordMat._22, m_vSwordMat._23 };
    _vector vSwordPos = { m_vSwordMat._41, m_vSwordMat._42, m_vSwordMat._43 };
    _vector vSwordStart = vSwordPos - XMVector3Normalize(m_vRot) * 1.5f;
    _vector vSwordEnd = vSwordPos + XMVector3Normalize(m_vRot) * 1.5f;
    XMStoreFloat4(&m_vSword_Start, XMVectorSetW(vSwordStart, 1.f));
    XMStoreFloat4(&m_vSword_End, XMVectorSetW(vSwordEnd, 1.f));

    m_pMeshTrail->Update(fTimeDelta);
    //if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
    //    m_pTalk->On_Panel(1);
    m_pTalk->Update_UITransform(m_pTransformCom->Get_State(STATE::POSITION));
    m_pTalk->Update(fTimeDelta);
}

void CElamein::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);
    
    m_pMeshTrail->Late_Update(fTimeDelta);
    m_pTalk->Late_Update(fTimeDelta);
}

void CElamein::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);

    if (COLLISION_LAYER::MAP_STATIC == eLayer)
        m_Data.isWallCrushed = true;
}

void CElamein::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CElamein::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

HRESULT CElamein::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Model_Elamein"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Elamein/Elamein/Elamein.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Elamein_Sword"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Elamein/Elamein_Sword/Elamein_Sword.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Elamein_Shield"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Elamein/Elamein_Shield/Elamein_Shield.dat")), E_FAIL);


    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Elamein_Body"),
        CBody_Elamein::Create(m_pDevice, m_pContext, m_iPrototypeIndex)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Elamein_Shield"),
        CElamein_Shield::Create(m_pDevice, m_pContext, m_iPrototypeIndex)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Elamein_Sword"),
        CElamein_Sword::Create(m_pDevice, m_pContext, m_iPrototypeIndex)), E_FAIL);

    return S_OK;
}

HRESULT CElamein::Ready_ETC()
{
    m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));
    CHECK_NULLPTR(m_pUI_HP, E_FAIL);
    m_pUI_HP->Setting_HP(&m_vHpPos, { 0.f, 0.f }, &m_fCurrentHP, &m_fMaxHP, &m_fCurrentStamina, &m_fMaxStamina);
    m_pGameInstance->Push_PoolObject_ToLayer(m_iPrototypeIndex, TEXT("Layer_UI"), m_pUI_HP);

    m_pController = CAI_Controller_Elamein::Create(this);
    CHECK_NULLPTR(m_pController, E_FAIL);

    m_pBlackBoard = m_pController->Get_BlackBoard();
    Safe_AddRef(m_pBlackBoard);
    m_pBlackBoard->Set_Value<CGameObject*>(m_strName, "Target", m_pTarget);

    return S_OK;
}

HRESULT CElamein::Ready_Components()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 1.25f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CONTROLLER);
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fPenetrationRecoverySpeed = 0.1f;
    tCharVirDesc.fMass = 0.f;
    tCharVirDesc.fMaxStrength = 10.f;
    m_tCollisionDesc.pGameObject = this;
    m_isGhost = true;
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 1.f;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc), E_FAIL);


    _vector vMatScale{}, vMatQuat{}, vMatPos{};

    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = { 0.6f, 0.5f, 0.5f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.bIsTrigger = true;

    _matrix LegMat = XMLoadFloat4x4(m_pLeftLegSocketMatrix) * m_pTransformCom->Get_WorldMatrix();
    for (uint32_t i = 0; i < 3; i++)
        LegMat.r[i] = XMVector3Normalize(LegMat.r[i]);

    XMMatrixDecompose(&vMatScale, &vMatQuat, &vMatPos, LegMat);

    XMStoreFloat3(&BodyDesc.vPos, vMatPos);
    XMStoreFloat4(&BodyDesc.vQuat, vMatQuat);

    BodyDesc.vShapeOffset = _float3(0.2f, -0.f, 0.f);
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_LegBody"), (CComponent**)&m_pLeftLegCom, &BodyDesc);

    BodyDesc.vExtent = { 0.9f, 2.2f, 0.9f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    BodyDesc.bIsTrigger = true;

    _matrix BodyMat = m_pTransformCom->Get_WorldMatrix();
    for (uint32_t i = 0; i < 3; i++)
        BodyMat.r[i] = XMVector3Normalize(BodyMat.r[i]);

    XMMatrixDecompose(&vMatScale, &vMatQuat, &vMatPos, BodyMat);

    XMStoreFloat3(&BodyDesc.vPos, vMatPos);
    XMStoreFloat4(&BodyDesc.vQuat, vMatQuat);

    BodyDesc.vShapeOffset = _float3(-0.f, 0.5f, 0.f);
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_HitBody"), (CComponent**)&m_pHitBodyCom, &BodyDesc);
    m_pHitBodyCom->Activate(true);
    return S_OK;
}

HRESULT CElamein::Ready_PartObjects()
{
    CBody_Elamein::BODY_DESC BodyDesc{};
    BodyDesc.pData = &m_Data;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    CHECK_FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Elamein_Body"), &BodyDesc), E_FAIL);

    m_pBody = dynamic_cast<CBody_Elamein*>(Find_PartObject(TEXT("Part_Body")));
    CHECK_NULLPTR(m_pBody, E_FAIL);
    Safe_AddRef(m_pBody);

    CElamein_Shield::WEAPON_DESC WeaponLDesc{};
    WeaponLDesc.pData = &m_Data;
    WeaponLDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponLDesc.pOwnerTransform = m_pTransformCom;
    WeaponLDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_L");
    CHECK_FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon_L"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Elamein_Shield"), &WeaponLDesc), E_FAIL);

    m_pShield = dynamic_cast<CElamein_Shield*>(Find_PartObject(TEXT("Part_Weapon_L")));
    CHECK_NULLPTR(m_pShield, E_FAIL);
    Safe_AddRef(m_pShield);

    CElamein_Sword::WEAPON_DESC WeaponRDesc{};
    WeaponRDesc.pData = &m_Data;
    WeaponRDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponRDesc.pOwnerTransform = m_pTransformCom;
    WeaponRDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_R");

    CHECK_FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon_R"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Elamein_Sword"), &WeaponRDesc), E_FAIL);

    m_pSword = dynamic_cast<CElamein_Sword*>(Find_PartObject(TEXT("Part_Weapon_R")));
    CHECK_NULLPTR(m_pSword, E_FAIL);
    Safe_AddRef(m_pSword);

    return S_OK;
}

HRESULT CElamein::Ready_AnimEvent()
{
    CModel* pModel = m_pBody->Get_Model();
    pModel->Register_Event("NormalAtk_1_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("NormalAtk_1_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("NormalAtk_1_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("NormalAtk_1_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 4.f); });
    pModel->Register_Event("NormalAtk_1_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 4.f); });
    pModel->Register_Event("NormalAtk_1_3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 4.f); });
    pModel->Register_Event("NormalAtk_1_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; this->m_Data.iAnimIndex = 65; });
    pModel->Register_Event("NormalAtk_1_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; this->m_Data.iAnimIndex = 66; });
    pModel->Register_Event("NormalAtk_1_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("NormalAtk_2_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED; });
    pModel->Register_Event("NormalAtk_2_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED; });
    pModel->Register_Event("NormalAtk_2_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED; });
    pModel->Register_Event("NormalAtk_2_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {LockOnLerp(m_fTimeDelta, 4.f); });
    pModel->Register_Event("NormalAtk_2_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {LockOnLerp(m_fTimeDelta, 4.f); });
    pModel->Register_Event("NormalAtk_2_3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {LockOnLerp(m_fTimeDelta, 4.f); });
    pModel->Register_Event("NormalAtk_2_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; this->m_Data.iAnimIndex = 68; });
    pModel->Register_Event("NormalAtk_2_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; this->m_Data.iAnimIndex = 69; });
    pModel->Register_Event("NormalAtk_2_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("NormalAtk_3_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED; });
    pModel->Register_Event("NormalAtk_3_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("NormalAtk_3_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {LockOnLerp(m_fTimeDelta, 4.f); });
    pModel->Register_Event("NormalAtk_3_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 4.f); });
    pModel->Register_Event("NormalAtk_3_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; this->m_Data.iAnimIndex = 71; });
    pModel->Register_Event("NormalAtk_3_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("ShieldStomp", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED; });
    pModel->Register_Event("ShieldStomp", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("HeadCrusher", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("HeadCrusher", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("HeadCrusher", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("RapidSlash", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("RapidSlash", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("RapidSlash", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("JumpSmash", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED; });
    pModel->Register_Event("JumpSmash", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("GuardCounter", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State = (_uint)ATTACK_BODY::SHILED | (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("GuardCounter", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("Arranged_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("Arranged_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("Arranged_3_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("Arranged_3_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::RIGHT_LEG; });
    pModel->Register_Event("Arranged_3_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAnimIndex = 86; });
    pModel->Register_Event("Arranged_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED; });
    pModel->Register_Event("Arranged_5_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("Arranged_5_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; });

    pModel->Register_Event("Arranged_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; this->m_Data.iAnimIndex = 84; });
    pModel->Register_Event("Arranged_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; this->m_Data.iAnimIndex = 85; });
    pModel->Register_Event("Arranged_3_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });
    pModel->Register_Event("Arranged_3_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });
    pModel->Register_Event("Arranged_4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; this->m_Data.iAnimIndex = 87; });
    pModel->Register_Event("Arranged_5_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });
    pModel->Register_Event("Arranged_5_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.iAttackBody_State = 0; });

    return S_OK;
}

HRESULT CElamein::Ready_MonData()
{
    m_Data.iAnimIndex = 101;
    m_Data.pOwner = this;
    m_Data.fGloggyTime = 3.f;
    m_Data.pCulHp = &m_fCurrentHP;
    m_Data.pMaxHp = &m_fMaxHP;

    m_Data.pCulStamina = &m_fCurrentStamina;
    m_Data.pMaxStamina = &m_fMaxStamina;

    m_Data.fEdgeWidth = 0.2f;
    m_Data.fEdgeColor = { 4.2f, 1.6f, 0.2f, 1.f };
    m_Data.fAttackDamage = m_fAttack;
    return S_OK;
}

void CElamein::Update_UIHp()
{
    m_vHpPos = { m_pHeadMatrix->m[3][0], m_pHeadMatrix->m[3][1], m_pHeadMatrix->m[3][2], 1.f };
    XMStoreFloat4(&m_vHpPos, XMVector4Transform(XMLoadFloat4(&m_vHpPos), m_pTransformCom->Get_WorldMatrix()));
    m_vHpPos.y += 0.5f;
}

void CElamein::Update_Body(_float fTimeDelta)
{
    m_pHitBodyCom->Collision_Active(m_isHit);
    _vector vMatScale{}, vMatQuat{}, vMatPos{};

    if (m_isHit)
    {
        XMMatrixDecompose(&vMatScale, &vMatQuat, &vMatPos, m_pTransformCom->Get_WorldMatrix());
        m_pHitBodyCom->Sync_Update(m_pTransformCom->Get_WorldMatrix());
        m_pHitBodyCom->Update(fTimeDelta, m_pTransformCom->Get_WorldMatrix(), vMatQuat, vMatPos);
    }

    _bool isAttack = m_Data.iAttackBody_State & (_uint)CElamein::ATTACK_BODY::RIGHT_LEG;
    m_pLeftLegCom->Collision_Active(isAttack);

    if (isAttack)
    {
        _matrix TailMat = XMLoadFloat4x4(m_pLeftLegSocketMatrix);
        for (uint32_t i = 0; i < 3; i++)
            TailMat.r[i] = XMVector3Normalize(TailMat.r[i]);
        TailMat *= m_pTransformCom->Get_WorldMatrix();

        XMMatrixDecompose(&vMatScale, &vMatQuat, &vMatPos, TailMat);
        m_pLeftLegCom->Sync_Update(TailMat);
        m_pLeftLegCom->Update(fTimeDelta, TailMat, vMatQuat, vMatPos);
    }
}

void CElamein::Update_MeshTrail()
{
    _vector vSwordStart = XMLoadFloat4(&m_vSword_Start);
    _vector vSwordEnd = XMLoadFloat4(&m_vSword_End);
    m_pMeshTrail->Add_ControlPoint(vSwordEnd, vSwordStart);
}

void CElamein::Rush()
{
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    m_pTransformCom->LookAt(vTargetPos);

    _vector vOffsetPos = XMVectorLerp(vPos, vTargetPos, (m_pBody->Get_CulTrack()) / 13.f);
    m_pTransformCom->Set_State(STATE::POSITION, vOffsetPos);
}

CElamein* CElamein::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CElamein* pInstance = new CElamein(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CElamein"));
    }
    return pInstance;
}

CGameObject* CElamein::Clone(void* pArg)
{
    CElamein* pInstance = new CElamein(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CElamein"));
    }
    return pInstance;
}

void CElamein::Free()
{
    Safe_Release(m_pMeshTrail);
    if (m_pUI_HP != nullptr)
    {
        m_pUI_HP->Set_IsDead(true);
    }
    __super::Free();
    Safe_Release(m_pBody);
    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pSword);
    Safe_Release(m_pShield);
    m_Data.pOwner = nullptr;


    Safe_Release(m_pTalk);
}
