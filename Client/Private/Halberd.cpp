#include "Halberd.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Halberd.h"
#include "Halberd_Weapon.h"

#include "AI_Controller_Halberd.h"

#include "Mon_Hp.h"
#include "MeshTrail.h"

#include "UI_Talk_Danjinjar.h"
#include "Target_BrutalAttack.h"

#include "StateMachine.h"
CHalberd::CHalberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CHalberd::CHalberd(const CHalberd& Prototype)
    :CMonster(Prototype)
{
}

void CHalberd::LockOnLerp(_float fTimeDetla, _float fSpeed)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDetla, fSpeed);
}

void CHalberd::LockOn()  
{
    m_pTransformCom->LookAt(m_pTarget->Get_Position());
}

void CHalberd::BurutalUI_On(_float fTime)
{
    m_pBrutalAttack = nullptr;
    m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
    m_pBrutalAttack->Setting_BrutalAttack(m_vLockOnPosition, fTime);
    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pBrutalAttack);
}

void CHalberd::BurutalUI_Off()
{
    if (m_pBrutalAttack == nullptr)
        return;

    m_pBrutalAttack->Off_BrutalAttack();
    m_pBrutalAttack = nullptr;
}

CHalberd::MONDATA& CHalberd::Get_Data()
{
    return m_Data;
}

void CHalberd::Move_F()
{
    m_pTarget->Get_Position();
    m_pTransformCom->LookAt(m_pTarget->Get_Position());

    _float fWorkSpeed = 4.1f;

    m_pTransformCom->AI_Chase(m_pTarget->Get_Position(), m_fTimeDelta, fWorkSpeed);
}

void CHalberd::Hp_Visivle(_bool isVisivle)
{
    m_pUI_HP->Update_Visible(isVisivle);
}

void CHalberd::Hp_Dead()
{
    m_pUI_HP->Set_IsDead(true);
    m_pUI_HP = nullptr;
}

_bool CHalberd::Check_Ranage(string strKey)
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

_bool CHalberd::Check_Ranage(_float fRange)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));

    if (fRange <= 0)
        return true;

    if (fDist <= fRange)
        return true;
    else
        return false;
}

TARGET_DIR CHalberd::Get_DIR()
{
    return Check_Dir(m_pTransformCom->Get_WorldMatrix(), m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
}

_float CHalberd::Get_TrackPotion()
{
    return m_pBody->Get_CulTrack();
}

const TRAIL_CONFIG& CHalberd::Get_TrailConfig() const
{
    return m_pMeshTrail->Get_TrailConfig();
}

void CHalberd::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    m_pMeshTrail->Set_TrailConfig(Config);
}

_uint CHalberd::Get_NumTrailTextures()
{
    return m_pMeshTrail->Get_NumTrailTextures();
}

ID3D11ShaderResourceView* CHalberd::Get_TrailTexture(_uint iIndex)
{
    return m_pMeshTrail->Get_TrailTexture(iIndex);
}

void CHalberd::Creature_Release()
{
    m_isHit = false;
    m_pHitBodyCom->Collision_Active(m_isHit);

    __super::Creature_Release();
}

HRESULT CHalberd::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CHalberd::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_MonData(), E_FAIL);
    CHECK_FAILED(Ready_ETC(), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(), E_FAIL);
    m_pHeadMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Head");
    m_pBodySocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Spine2");
    m_pLockOnSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Spine2");
    m_vLockOnPosition = &m_vLockOnPos;

    CHECK_FAILED(Ready_AnimEvent(), E_FAIL);
    CHECK_FAILED(Ready_Components(), E_FAIL);

    CMeshTrail::TRAIL_DESC MeshDesc{};
    MeshDesc.iTextureIdx = 20;
    MeshDesc.fLifeTime = 0.4f;
    MeshDesc.iDivisionCount = 10.f;
    //MeshDesc.vColor = _float4(0.3804f, 0.3059f, 0.0667f, 0.5098f);
    //MeshDesc.vSubColor = _float4(0.6863f, 0.0f, 0.0f, 3.9216f);
    MeshDesc.vColor = _float4(0.1f, 0.1f, 0.1f, 0.5098f);
    MeshDesc.vSubColor = _float4(1.f, 1.f, 1.f, 3.9216f);
    m_pMeshTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));

    m_fRecoveryPerSec = 10.f;

    m_vDecalSize[ENUM_CLASS(DECALTYPE::LINEAR)] = { 3.f, 5.f };
    m_vDecalSize[ENUM_CLASS(DECALTYPE::CIRCLE)] = { 4.f, 6.f };
    m_vDecalSize[ENUM_CLASS(DECALTYPE::CURVE)] = { 3.f, 5.f };

    return S_OK;
}

void CHalberd::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);

    m_pMeshTrail->Priority_Update(fTimeDelta);
    m_isRequestRecoveryStamina = m_Data.isStamina_Regen;
    if (m_Data.isStamina_Regen)
    {
        Recovery_Stamina(fTimeDelta * 60.f);
        if (m_fCurrentStamina == m_fMaxStamina)
            m_Data.isStamina_Regen = false;
    }
}

void CHalberd::Update(_float fTimeDelta)
{
    m_fTimeDelta = fTimeDelta;

    if (m_Data.fAttackCool > 0.f)
        m_Data.fAttackCool -= fTimeDelta;

    if (m_Data.fSpecial_AttackCool > 0.f)
        m_Data.fSpecial_AttackCool -= fTimeDelta;

    if (m_Data.fLong_AttackCool > 0.f)
        m_Data.fLong_AttackCool -= fTimeDelta;

    _float4x4 LockOnMatrix{};
    XMStoreFloat4x4(&LockOnMatrix, XMLoadFloat4x4(m_pLockOnSocketMatrix) * m_pTransformCom->Get_WorldMatrix());
    m_vLockOnPos = { LockOnMatrix._41, LockOnMatrix._42, LockOnMatrix._43, 1.f };

    _float4x4 SwordMatrix = m_pWeapon->Get_CombindMat();
    _vector vUp = { SwordMatrix._21, SwordMatrix._22, SwordMatrix._23 };
    _vector vSwordPos = { SwordMatrix._41, SwordMatrix._42, SwordMatrix._43 };

    _vector vSwordStart = vSwordPos + XMVector3Normalize(vUp) * 1.3f;
    _vector vSwordEnd = vSwordPos + XMVector3Normalize(vUp) * 3.f;
    XMStoreFloat4(&m_vSword_Start, XMVectorSetW(vSwordStart, 1.f));
    XMStoreFloat4(&m_vSword_End, XMVectorSetW(vSwordEnd, 1.f));

    m_pMeshTrail->Update(fTimeDelta);
    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);
    Update_UIHp();
    Update_Body(fTimeDelta);
}

void CHalberd::Late_Update(_float fTimeDelta)
{
    if (m_Data.isSearch)
        m_pBodyComp->Collision_Active(false);

    CContainerObject::Late_Update(fTimeDelta);
    
    m_pMeshTrail->Late_Update(fTimeDelta);

    if (!m_Data.isSearch)
        return;

    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CHalberd::Render()
{
    m_pBody->Render();
    m_pWeapon->Render();
    return S_OK;
}

void CHalberd::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
    if (m_Data.eHitType == HITREACTION::BRUTAL_ATTACK)
    {
        switch (m_pGameInstance->Rand(1, 3))
        {
        case 1:
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_damage_l_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f);
            break;
        case 2:
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_damage_l_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f);
            break;
        default:
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_damage_l_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f);
            break;
        }
    }
    else
    {
        switch (m_pGameInstance->Rand(1, 3))
        {
        case 1:
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_damage_s_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f);
            break;
        case 2:
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_damage_s_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f);
            break;
        default:
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_damage_s_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f);
            break;
        }
    }
    __super::Take_Damage(fDamage, eHitreaction, pGameObject);
}

void CHalberd::KnockBack(_vector vDir, _float fPower, _float fLoss)
{
    m_isKnockBack = true;
    m_fKnockBackDir = vDir;
    m_fKnockBackPower = fPower * 0.8f;
    m_fKnockBackLoss = fLoss;
}

void CHalberd::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);

    if (COLLISION_LAYER::MAP_STATIC == eLayer)
        m_Data.isWallCrushed = true;

    if (pMyDesc->iObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER_SEARCH))
    {
        m_Data.isSearch = true;
    }
}

void CHalberd::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CHalberd::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

HRESULT CHalberd::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Model_Halberd"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Halberd/Halberd/Halberd.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Weapon_Halberd"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Halberd/Weapon_Halberd/Weapon_Halberd.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Halberd_Body"),
        CBody_Halberd::Create(m_pDevice, m_pContext, m_iPrototypeIndex)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Halberd_Weapon"),
        CHalberd_Weapon::Create(m_pDevice, m_pContext, m_iPrototypeIndex)), E_FAIL);

    return S_OK;
}

HRESULT CHalberd::Ready_ETC()
{
    m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));
    CHECK_NULLPTR(m_pUI_HP, E_FAIL);
    m_pUI_HP->Setting_HP(&m_vHpPos, { 0.f, 0.f }, &m_fCurrentHP, &m_fMaxHP, &m_fCurrentStamina, &m_fMaxStamina);
    m_pGameInstance->Push_PoolObject_ToLayer(m_iPrototypeIndex, TEXT("Layer_UI"), m_pUI_HP);

    m_pController = CAI_Controller_Halberd::Create(this);
    CHECK_NULLPTR(m_pController, E_FAIL);

    m_pBlackBoard = m_pController->Get_BlackBoard();
    Safe_AddRef(m_pBlackBoard);
    m_pBlackBoard->Set_Value<CGameObject*>(m_strName, "Target", m_pTarget);
    return S_OK;

}

HRESULT CHalberd::Ready_Components()
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
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 1.f;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc), E_FAIL);

    CBody::BODY_BOXSHAPE_DESC BodyDesc{};

    BodyDesc.vExtent = { 1.2f, 2.2f, 1.2f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    BodyDesc.bIsTrigger = true;

    _matrix BodyMat = m_pTransformCom->Get_WorldMatrix();
    for (uint32_t i = 0; i < 3; i++)
        BodyMat.r[i] = XMVector3Normalize(BodyMat.r[i]);
    _vector vMatScale{}, vMatQuat{}, vMatPos{};
    XMMatrixDecompose(&vMatScale, &vMatQuat, &vMatPos, BodyMat);

    XMStoreFloat3(&BodyDesc.vPos, vMatPos);
    XMStoreFloat4(&BodyDesc.vQuat, vMatQuat);
    m_tHitCollisionDesc.pGameObject = this;
    BodyDesc.vShapeOffset = _float3(-0.f, 1.f, 0.f);
    BodyDesc.pCollisionDesc = &m_tHitCollisionDesc;

    CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_HitBody"), (CComponent**)&m_pHitBodyCom, &BodyDesc);
    m_pHitBodyCom->Activate(true);


    CBody::BODY_BOXSHAPE_DESC SearchBodyDesc{};
  SearchBodyDesc.vExtent = { 50.f, 50.f, 50.f };
    SearchBodyDesc.eMotion = EMotionType::Kinematic;
    SearchBodyDesc.eQuality = EMotionQuality::Discrete;
    SearchBodyDesc.eShapeType = SHAPE::BOX;
    SearchBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER_SEARCH);
    SearchBodyDesc.bIsTrigger = true;
    XMStoreFloat3(&SearchBodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&SearchBodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
    m_tSearchCollisionDesc.pGameObject = this;
    SearchBodyDesc.vShapeOffset = _float3(0.f, 0.5f, 0.f);
    SearchBodyDesc.pCollisionDesc = &m_tSearchCollisionDesc;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_Body_Search"), (CComponent**)&m_pBodyComp, &SearchBodyDesc), E_FAIL);

    return S_OK;
}

HRESULT CHalberd::Ready_PartObjects()
{
    CBody_Halberd::BODY_DESC BodyDesc{};
    BodyDesc.pData = &m_Data;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    CHECK_FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Halberd_Body"), &BodyDesc), E_FAIL);

    m_pBody = dynamic_cast<CBody_Halberd*>(Find_PartObject(TEXT("Part_Body")));
    CHECK_NULLPTR(m_pBody, E_FAIL);
    Safe_AddRef(m_pBody);

    CHalberd_Weapon::WEAPON_DESC WeaponLDesc{};
    WeaponLDesc.pData = &m_Data;
    WeaponLDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponLDesc.pOwnerTransform = m_pTransformCom;
    WeaponLDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_R");
    CHECK_FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon_R"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Halberd_Weapon"), &WeaponLDesc), E_FAIL);

    m_pWeapon = dynamic_cast<CHalberd_Weapon*>(Find_PartObject(TEXT("Part_Weapon_R")));
    CHECK_NULLPTR(m_pWeapon, E_FAIL);
    Safe_AddRef(m_pWeapon);

    return S_OK;
}

HRESULT CHalberd::Ready_AnimEvent()
{
    CModel* pModel = m_pBody->Get_Model();
    //디폴트 공격 1
    pModel->Register_Event("Whirlwind_0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("Whirlwind_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("Whirlwind_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("Whirlwind_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });

    pModel->Register_Event("Whirlwind_0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {/*Update_MeshTrail();*/ LockOnLerp(m_fTimeDelta, 2.5f); });
    pModel->Register_Event("Whirlwind_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {/*Update_MeshTrail();*/ LockOnLerp(m_fTimeDelta, 2.5f); });
    pModel->Register_Event("Whirlwind_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {/*Update_MeshTrail();*/ LockOnLerp(m_fTimeDelta, 2.5f); });
    pModel->Register_Event("Whirlwind_3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {/*Update_MeshTrail();*/ LockOnLerp(m_fTimeDelta, 2.5f); });

    pModel->Register_Event("Whirlwind_0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });
    pModel->Register_Event("Whirlwind_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });
    pModel->Register_Event("Whirlwind_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });
    pModel->Register_Event("Whirlwind_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });

    pModel->Register_Event("Trail0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("Trail1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("Trail2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("Trail3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });

    //디폴트 공격 2
    pModel->Register_Event("Swing_0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                               m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                               m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("Swing_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                               m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                               m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("Swing_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                               m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                               m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    
    pModel->Register_Event("Swing_0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 2.5f); });
    pModel->Register_Event("Swing_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 2.5f); });
    pModel->Register_Event("Swing_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); });

    pModel->Register_Event("Swing_0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; m_Data.iAnimIndex = 65; });
    pModel->Register_Event("Swing_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; m_Data.iAnimIndex = 66; });
    pModel->Register_Event("Swing_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });
    
    //디폴트 공격 3
    pModel->Register_Event("Pierce_0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_pierce_cast_start_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("Pierce_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true;  LockOnLerp(m_fTimeDelta, 2.5f);
                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_pierce_cast_end_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    
    pModel->Register_Event("Pierce_0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail();; });
    pModel->Register_Event("Pierce_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); });
    
    pModel->Register_Event("Pierce_0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; m_Data.iAnimIndex = 52; LockOnLerp(m_fTimeDelta, 1.5f); });
    pModel->Register_Event("Pierce_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });
    
    //디폴트 공격 4
    pModel->Register_Event("EmpireSwing_0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f);});
    pModel->Register_Event("EmpireSwing_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("EmpireSwing_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });

    pModel->Register_Event("EmpireSwing_0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail();  LockOnLerp(m_fTimeDelta, 2.5f);});
    pModel->Register_Event("EmpireSwing_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail();  LockOnLerp(m_fTimeDelta, 2.5f);});
    pModel->Register_Event("EmpireSwing_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); });

    pModel->Register_Event("EmpireSwing_0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; m_Data.iAnimIndex = 5; });
    pModel->Register_Event("EmpireSwing_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; m_Data.iAnimIndex = 6; });
    pModel->Register_Event("EmpireSwing_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });

    //러쉬 공격 1
    pModel->Register_Event("ChargeAttack_0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true; 
                                                                                      m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_chargeattack_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f);
                                                                                      m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("ChargeAttack_0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); });
    pModel->Register_Event("ChargeAttack_0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });
    //러쉬 공격 2    
    pModel->Register_Event("SprintSwing_0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.isAttack_Collinder = true;
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_swing_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1));
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_atk_l_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("ChargeAttack_0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Update_MeshTrail(); });
    pModel->Register_Event("SprintSwing_0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.isAttack_Collinder = false; });

    //NoBattle
    pModel->Register_Event("Search_Sound", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_demonmutated_a_foley_basic_search_c_01 (SFX).wav"), Get_Position(), Get_SoundChannel(3)); });
    pModel->Register_Event("Search_End", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_picaroonssword_n_basic_search_end_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 1.5f); });
    pModel->Register_Event("Search_End_Rustle", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_basicb_rustle_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(4)); });

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

    pModel->Register_Event("Swing_Move_01", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Swing_Move_02", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Swing_Move_03", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });

    pModel->Register_Event("Rush_Move_01", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("Rush_Move_02", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });

    //Effect
    

    pModel->Register_Event("Stamp0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Call_StampFX(); });
    pModel->Register_Event("Stamp1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Call_StampFX(); });
    pModel->Register_Event("Stamp2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Call_StampFX(); });

    pModel->Register_Event("Sting0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Call_StingFX(); });
    pModel->Register_Event("Sting0_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_StingFX(); });
    pModel->Register_Event("Sting1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Call_StingFX(); });
    pModel->Register_Event("Sting1_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_StingFX(); });

    return S_OK;
}

HRESULT CHalberd::Ready_MonData()
{
    m_Data.iAnimIndex = 101;
    m_Data.pOwner = this;
    m_Data.fGloggyTime = 3.f;
    m_Data.pCulHp = &m_fCurrentHP;
    m_Data.pMaxHp = &m_fMaxHP;

    m_Data.pCulStamina = &m_fCurrentStamina;
    m_Data.pMaxStamina = &m_fMaxStamina;

    m_Data.fEdgeWidth = 0.05f;
    m_Data.fEdgeColor = { 1.3f, 0.75f, 0.f, 1.f };
    m_Data.fAttackDamage = m_fAttack;
    return S_OK;
}

void CHalberd::Update_UIHp()
{
    m_vHpPos = { m_pHeadMatrix->m[3][0], m_pHeadMatrix->m[3][1], m_pHeadMatrix->m[3][2], 1.f };
    XMStoreFloat4(&m_vHpPos, XMVector4Transform(XMLoadFloat4(&m_vHpPos), m_pTransformCom->Get_WorldMatrix()));
    m_vHpPos.y += 0.5f;
}

void CHalberd::Update_Body(_float fTimeDelta)
{
    m_pHitBodyCom->Collision_Active(m_isHit);
    _vector vMatScale{}, vMatQuat{}, vMatPos{};

    if (m_isHit)
    {
        XMMatrixDecompose(&vMatScale, &vMatQuat, &vMatPos, m_pTransformCom->Get_WorldMatrix());
        m_pHitBodyCom->Sync_Update(m_pTransformCom->Get_WorldMatrix());
        m_pHitBodyCom->Update(fTimeDelta, m_pTransformCom->Get_WorldMatrix(), vMatQuat, vMatPos);
    }


}

void CHalberd::Update_MeshTrail()
{
    _vector vSwordStart = XMLoadFloat4(&m_vSword_Start);
    _vector vSwordEnd = XMLoadFloat4(&m_vSword_End);
    m_pMeshTrail->Add_ControlPoint(vSwordEnd, vSwordStart);
}

void CHalberd::Move_Sound()
{
    switch (m_pGameInstance->Rand(1, 5))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_01 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_02 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_03 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_04 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_05 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    }

    switch (m_pGameInstance->Rand(1, 6))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_01 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_02 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_03 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_04 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 5:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_05 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_06 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    }
}

void CHalberd::Run_Sound()
{
    switch (m_pGameInstance->Rand(1, 5))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_01 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_02 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_03 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_04 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_f_05 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    }

    switch (m_pGameInstance->Rand(1, 6))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_01 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_02 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_03 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_04 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 5:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_05 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_empirehalberd_foley_walk_rustle_06 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    }
}

void CHalberd::Call_StampFX()
{
    _float4x4 SwordMatrix = m_pWeapon->Get_CombindMat();
    _vector vUp = { SwordMatrix._21, SwordMatrix._22, SwordMatrix._23 };
    _vector vPos = { SwordMatrix._41, SwordMatrix._42, SwordMatrix._43 };
    vPos += XMVector3Normalize(vUp) * 2.1f;
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Stamp"), vPos);
}

void CHalberd::Call_StingFX()
{
    _float4x4 Swordfloat4x4 = m_pWeapon->Get_CombindMat();
    _matrix SwordMatrix = XMLoadFloat4x4(&Swordfloat4x4);
    m_iFXidx = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Halberd_Weapon_Wind"), SwordMatrix, SwordMatrix.r[3]);
}

void CHalberd::Update_StingFX()
{
    _float4x4 Swordfloat4x4 = m_pWeapon->Get_CombindMat();
    _matrix SwordMatrix = XMLoadFloat4x4(&Swordfloat4x4);    
    m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Halberd_Weapon_Wind"), m_iFXidx, SwordMatrix, SwordMatrix.r[3]);
}

void CHalberd::Rush()
{
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    m_pTransformCom->LookAt(vTargetPos);

    _vector vOffsetPos = XMVectorLerp(vPos, vTargetPos, (m_pBody->Get_CulTrack()) / 13.f);
    m_pTransformCom->Set_State(STATE::POSITION, vOffsetPos);
}

CHalberd* CHalberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CHalberd* pInstance = new CHalberd(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CHalberd"));
    }
    return pInstance;
}

CGameObject* CHalberd::Clone(void* pArg)
{
    CHalberd* pInstance = new CHalberd(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CHalberd"));
    }
    return pInstance;
}

void CHalberd::Free()
{
    if (m_pBrutalAttack != nullptr)
        m_pBrutalAttack->Off_BrutalAttack();

    if (m_pUI_HP != nullptr)
        m_pUI_HP->Set_IsDead(true);

    __super::Free();

    Safe_Release(m_pMeshTrail);
    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pBody);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pHitBodyCom);
    m_Data.pOwner = nullptr;

    Safe_Release(m_pBodyComp);
}
