#include "Dragonian_Melee.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Dragonian_Melee.h"
#include "Dragonian_Sword.h"
#include "AI_Controller_Dragonian_Melee.h"

#include "Mon_Hp.h"
#include "UI_Talk_Danjinjar.h"

#include "MeshTrail.h"
#include "Target_BrutalAttack.h"

CDragonian_Melee::CDragonian_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CDragonian_Melee::CDragonian_Melee(const CDragonian_Melee& Prototype)
    :CMonster( Prototype )
{
}

CDragonian_Melee::MONDATA& CDragonian_Melee::Get_Data()
{
    return m_Data;
}

void CDragonian_Melee::LockOnLerp(_float fTimeDetla, _float fSpeed)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDetla, fSpeed);
}

void CDragonian_Melee::LockOn()
{
    m_pTransformCom->LookAt(m_pTarget->Get_Position());
}

void CDragonian_Melee::Hp_Visivle(_bool isVisivle)
{
    m_pUI_HP->Update_Visible(isVisivle);
}

void CDragonian_Melee::Hp_Dead()
{
    m_pUI_HP->Set_IsDead(true);
    m_pUI_HP = nullptr;
}

void CDragonian_Melee::LookAt_Lerp(_float fTimeDelta)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDelta, 0.8f);

}

_bool CDragonian_Melee::Check_AttackRanage(string strKey)
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

_bool CDragonian_Melee::Check_Ranage(_float fRange)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));

    if (fRange <= 0)
        return true;

    if (fDist <= fRange)
        return true;
    else
        return false;
}

TARGET_DIR CDragonian_Melee::Get_DIR()
{
    return Check_Dir(m_pTransformCom->Get_WorldMatrix(), m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
}

void CDragonian_Melee::BurutalUI_On(_float fTime)
{
    m_pBrutalAttack = nullptr;
    m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
    m_pBrutalAttack->Setting_BrutalAttack(m_vLockOnPosition, fTime);
    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pBrutalAttack);
}

void CDragonian_Melee::BurutalUI_Off()
{
    if (m_pBrutalAttack == nullptr)
        return;

    m_pBrutalAttack->Off_BrutalAttack();
    m_pBrutalAttack = nullptr;
}

HRESULT CDragonian_Melee::Initialize_Prototype(_int iLevel)
{

    m_iPrototypeIndex = iLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDragonian_Melee::Initialize_Clone(void* pArg)
{
    DRAGON_MELEE_MONSTER_DESC* pDesc = static_cast<DRAGON_MELEE_MONSTER_DESC*>(pArg);

    m_Data.isMotionSleep = pDesc->isSleep;

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_MonData(), E_FAIL);
    CHECK_FAILED(Ready_ETC(), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(),E_FAIL);
    m_pHeadMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Head");

    m_pLockOnSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("FX_Body_ExpGained");
    m_vLockOnPosition = &m_vLockOnPos;

    CHECK_FAILED(Ready_AnimEvent(),E_FAIL);
    CHECK_FAILED(Ready_Components(),E_FAIL);
    
    CMeshTrail::TRAIL_DESC MeshDesc{};
    MeshDesc.iTextureIdx = 11;
    MeshDesc.fLifeTime = 0.25f;
    MeshDesc.iDivisionCount = 10.f;
    MeshDesc.vColor = _float4(1.58f, 1.788f, 1.592f, 1.f);
    m_pMeshTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));

    m_fRecoveryPerSec = 10.f;

    return S_OK;
}

void CDragonian_Melee::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);

    m_pMeshTrail->Priority_Update(fTimeDelta);
    m_isRequestRecoveryStamina = m_Data.isStamina_Regen;
    if (m_Data.isStamina_Regen)
    {
        Recovery_Stamina(fTimeDelta * 12.f);
        if (m_fCurrentStamina == m_fMaxStamina)
            m_Data.isStamina_Regen = false;
    }
    m_pMeshTrail->Priority_Update(fTimeDelta);
}

void CDragonian_Melee::Update(_float fTimeDelta)
{
    m_fTimeDelta = fTimeDelta;

    if (m_Data.fAttackCool >= 0.f)
        m_Data.fAttackCool -= fTimeDelta;

    if (m_Data.fAnimDeley >= 0.f)
        m_Data.fAnimDeley -= fTimeDelta;

    _float4x4 LockOnMatrix{};
    XMStoreFloat4x4(&LockOnMatrix, XMLoadFloat4x4(m_pLockOnSocketMatrix) * m_pTransformCom->Get_WorldMatrix());
    m_vLockOnPos = { LockOnMatrix._41, LockOnMatrix._42, LockOnMatrix._43, 1.f };

    _float4x4 m_vSwordMat = m_pWeapon->Get_CombindMat();
    _vector vClawLRight = XMVector3Normalize({ m_vSwordMat._11, m_vSwordMat._12, m_vSwordMat._13 });
    _vector vClawLUp = XMVector3Normalize({ m_vSwordMat._21, m_vSwordMat._22, m_vSwordMat._23 });
    _vector vClawLLook = XMVector3Normalize({ m_vSwordMat._31, m_vSwordMat._32, m_vSwordMat._33 });


    _vector vSwordPos = { m_vSwordMat._41, m_vSwordMat._42, m_vSwordMat._43 };
    _vector vSwordStart = vSwordPos - vClawLUp * 0.45f + vClawLRight * 0.25f;
    _vector vSwordEnd = vSwordPos + vClawLUp * 1.1f - vClawLRight * 0.2f;
    XMStoreFloat4(&m_vSword_Start, XMVectorSetW(vSwordStart, 1.f));
    XMStoreFloat4(&m_vSword_End, XMVectorSetW(vSwordEnd, 1.f));

    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);
    Update_UIHp();
    Update_WalkSpeed();

    m_pMeshTrail->Update(fTimeDelta);
}

void CDragonian_Melee::Late_Update(_float fTimeDelta)
{
    if (m_Data.isSearch)
    {
        m_pBodyComp->Collision_Active(false);
    }
    else
    {
        _matrix WeaponWorld = m_pTransformCom->Get_WorldMatrix();

        _vector vScale, vQuat, vPos;
        XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

        m_pBodyComp->Sync_Update(WeaponWorld);
        m_pBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);
    }
    CContainerObject::Late_Update(fTimeDelta);

    m_pMeshTrail->Late_Update(fTimeDelta);
}

void CDragonian_Melee::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
    if (m_Data.eHitType == HITREACTION::BRUTAL_ATTACK)
        ++m_Data.iBrutalHit;

    switch (m_pGameInstance->Rand(1, 5))
    {
    case 1:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Dmg_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    case 2:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Dmg_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    case 3:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Dmg_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    case 4:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Dmg_04 (SFX).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    default:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Dmg_05 (SFX).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    }

    __super::Take_Damage(fDamage, eHitreaction, pGameObject);
}

const TRAIL_CONFIG& CDragonian_Melee::Get_TrailConfig() const
{
    return m_pMeshTrail->Get_TrailConfig();
}

void CDragonian_Melee::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    m_pMeshTrail->Set_TrailConfig(Config);
}

_uint CDragonian_Melee::Get_NumTrailTextures()
{
    return m_pMeshTrail->Get_NumTrailTextures();
}

ID3D11ShaderResourceView* CDragonian_Melee::Get_TrailTexture(_uint iIndex)
{
    return m_pMeshTrail->Get_TrailTexture(iIndex);
}

void CDragonian_Melee::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (pMyDesc->iObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER_SEARCH))
    {
        m_Data.isSearch = true;
    }
}

void CDragonian_Melee::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CDragonian_Melee::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

HRESULT CDragonian_Melee::Ready_Prototype()
{
    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Melee"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Melee/Dragonian_Melee/Dragonian_Melee.dat"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Sword"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Melee/DragonSword/DragonSword.dat"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Melee_Body"),
        CBody_Dragonian_Melee::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Sword"),
        CDragonian_Sword::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDragonian_Melee::Ready_ETC()
{
    m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));
    CHECK_NULLPTR(m_pUI_HP, E_FAIL);
    m_pUI_HP->Setting_HP(&m_vHpPos, { 0.f, 0.f }, &m_fCurrentHP, &m_fMaxHP, &m_fCurrentStamina, &m_fMaxStamina);
    m_pGameInstance->Push_PoolObject_ToLayer(m_iPrototypeIndex, TEXT("Layer_UI"), m_pUI_HP);

    m_pController = CAI_Controller_Dragonian_Melee::Create(this);
    CHECK_NULLPTR(m_pController, E_FAIL);

    m_pBlackBoard = m_pController->Get_BlackBoard();
    Safe_AddRef(m_pBlackBoard);
    m_pBlackBoard->Set_Value<CGameObject*>(m_strName, "Target", m_pTarget);

    return S_OK;
}

HRESULT CDragonian_Melee::Ready_Components()
{    
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 1.35f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fPenetrationRecoverySpeed = 0.1f;
    
    m_tCollisionDesc.pGameObject = this;
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    //캡슐 Desc
    tCharVirDesc.fRadius = 1.2f;
    tCharVirDesc.fHeight = 0.4f;
    
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc), E_FAIL);

    CBody::BODY_BOXSHAPE_DESC SearchBodyDesc{};
     SearchBodyDesc.vExtent = { 50.f, 50.f, 50.f };
    SearchBodyDesc.eMotion = EMotionType::Kinematic;
    SearchBodyDesc.eQuality = EMotionQuality::Discrete;
    SearchBodyDesc.eShapeType = SHAPE::BOX;
    SearchBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER_SEARCH);
    SearchBodyDesc.bIsTrigger = true;
    XMStoreFloat3(&SearchBodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&SearchBodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
    SearchBodyDesc.vShapeOffset = _float3(0.f, 0.5f, 0.f);
    m_tSearchCollisionDesc.pGameObject = this;
    SearchBodyDesc.pCollisionDesc = &m_tSearchCollisionDesc;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_Body_Search"), (CComponent**)&m_pBodyComp, &SearchBodyDesc), E_FAIL);


    return S_OK;
}

HRESULT CDragonian_Melee::Ready_PartObjects()
{
    CBody_Dragonian_Melee::BODY_DESC BodyDesc{};
    BodyDesc.pData = &m_Data;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Melee_Body"), &BodyDesc)))
        return E_FAIL;
    
    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Dragonian_Melee*>(pBody);
    Safe_AddRef(m_pBody);

    CDragonian_Sword::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pOwnerTransform = m_pTransformCom;
    WeaponDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_R");
    WeaponDesc.pData = &m_Data;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Sword"), &WeaponDesc)))
        return E_FAIL;

    CPartObject* pWeapon = Find_PartObject(TEXT("Part_Weapon"));
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pWeapon = dynamic_cast<CDragonian_Sword*>(pWeapon);
    Safe_AddRef(m_pWeapon);

    return S_OK;
}

HRESULT CDragonian_Melee::Ready_AnimEvent()
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

    pModel->Register_Event("DoubleSwing_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; Attack_Sound(true); });
    pModel->Register_Event("DoubleSwing_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; Attack_Sound(false); });
    pModel->Register_Event("DoubleSwing_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; Attack_Sound(true); });
    pModel->Register_Event("DoubleSwing_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; Attack_Sound(false); });

    pModel->Register_Event("DoubleSwing_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });
    pModel->Register_Event("DoubleSwing_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });
    pModel->Register_Event("DoubleSwing_3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });
    pModel->Register_Event("DoubleSwing_4", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });

    pModel->Register_Event("DoubleSwing_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });
    pModel->Register_Event("DoubleSwing_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });
    pModel->Register_Event("DoubleSwing_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });
    pModel->Register_Event("DoubleSwing_4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });

    pModel->Register_Event("HardSmash_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; Attack_Sound(true);});
    pModel->Register_Event("HardSmash_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; Attack_Sound(false);});

    pModel->Register_Event("HardSmash_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });
    pModel->Register_Event("HardSmash_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });

    pModel->Register_Event("HardSmash_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });
    pModel->Register_Event("HardSmash_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });

    pModel->Register_Event("Doubble_Sound_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianwarrior_hardsmash_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 5.f); });
    pModel->Register_Event("Doubble_Sound_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianwarrior_hardsmash_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 5.f); });

    //MoveSound
    pModel->Register_Event("Lock_B_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Lock_B_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Lock_F_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Lock_F_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Lock_L_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Lock_L_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Lock_R_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Lock_R_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });


    pModel->Register_Event("Turn_L_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Turn_L_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Turn_R_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Turn_R_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Turn_L_180_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Turn_L_180_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Turn_R_180_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Turn_R_180_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });

    pModel->Register_Event("DoubleATK_Move_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("DoubleATK_Move_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("DoubleATK_Move_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("DoubleATK_Move_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("DoubleATK_Move_5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("DoubleATK_Move_6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("DoubleATK_Move_7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });

    pModel->Register_Event("HardSmash_Move_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("HardSmash_Move_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("HardSmash_Move_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("HardSmash_Move_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("HardSmash_Move_5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("HardSmash_Move_6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });

    pModel->Register_Event("StandEnd_Sound", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianwarrior_hardsmash_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 5.f); });

    return S_OK;
}

HRESULT CDragonian_Melee::Ready_MonData()
{
    m_Data.pOwner = this;
    m_Data.fGloggyTime = 3.f;
    m_Data.pCulHp = &m_fCurrentHP;
    m_Data.pMaxHp = &m_fMaxHP;

    m_Data.pCulStamina = &m_fCurrentStamina;
    m_Data.pMaxStamina = &m_fMaxStamina;

    m_Data.fEdgeWidth = 0.1f;
    m_Data.fEdgeColor = { 4.2f, 1.6f, 0.2f, 1.f };

    return S_OK;
}

void CDragonian_Melee::Update_UIHp()
{
    m_vHpPos = { m_pHeadMatrix->m[3][0], m_pHeadMatrix->m[3][1], m_pHeadMatrix->m[3][2], 1.f };
    XMStoreFloat4(&m_vHpPos, XMVector4Transform(XMLoadFloat4(&m_vHpPos), m_pTransformCom->Get_WorldMatrix()));
    m_vHpPos.y += 0.5f;
}

void CDragonian_Melee::Update_WalkSpeed()
{
    if (m_Data.isSlowWalk)
        m_Data.fWarkSpeed = 0.7f;
    else
        m_Data.fWarkSpeed = 1.6f;
}

void CDragonian_Melee::Update_MeshTrail()
{
    _vector vSwordStart = XMLoadFloat4(&m_vSword_Start);
    _vector vSwordEnd = XMLoadFloat4(&m_vSword_End);
    m_pMeshTrail->Add_ControlPoint(vSwordEnd, vSwordStart);
}

void CDragonian_Melee::Attack_Move()
{
    LockOnLerp(m_fTimeDelta, 3.f);
    Get_Transform()->Go_Straight(m_fTimeDelta);
}

void CDragonian_Melee::Attack_Sound(_bool isASound)
{
    if (isASound)
    {
        switch (m_pGameInstance->Rand(1, 3))
        {
        case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_sword_swish_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1));             break;
        case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_sword_swish_a_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1));             break;
        default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_sword_swish_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1));             break;
        }
    }
    else
    {
        switch (m_pGameInstance->Rand(1, 3))
        {
        case 1:         m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_sword_swish_b_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1));         break;
        case 2:         m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_sword_swish_b_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1));         break;
        default:        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_sword_swish_b_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1));         break;
        }
    }

    switch (m_pGameInstance->Rand(1, 5))
    {
    case 1:             m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2));              break;
    case 2:             m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_02 (SFX).wav"), Get_Position(), Get_SoundChannel(2));              break;
    case 3:             m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_03 (SFX).wav"), Get_Position(), Get_SoundChannel(2));              break;
    case 4:             m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_04 (SFX).wav"), Get_Position(), Get_SoundChannel(2));              break;
    default:            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_05 (SFX).wav"), Get_Position(), Get_SoundChannel(2));              break;
    }
}

void CDragonian_Melee::Move_Sound()
{
    switch (m_pGameInstance->Rand(1, 6))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_walk_01 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_walk_02 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_walk_03 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_walk_04 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    case 5:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_walk_05 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_walk_06 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    }
}

CDragonian_Melee* CDragonian_Melee::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CDragonian_Melee* pInstance = new CDragonian_Melee(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CDragonian_Melee"));
    }
    return pInstance;
}

CGameObject* CDragonian_Melee::Clone(void* pArg)
{
    CDragonian_Melee* pInstance = new CDragonian_Melee(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CDragonian_Melee"));
    }
    return pInstance;
}

void CDragonian_Melee::Free()
{
    Safe_Release(m_pMeshTrail);
    m_Data.pOwner = nullptr;
    if (m_pBrutalAttack != nullptr)
        m_pBrutalAttack->Off_BrutalAttack();

    if (m_pUI_HP != nullptr)
        m_pUI_HP->Set_IsDead(true);
    __super::Free();
    Safe_Release(m_pBody);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pBodyComp);
}
