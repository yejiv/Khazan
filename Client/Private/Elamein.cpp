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

#include "Target_BrutalAttack.h"
#include "StateMachine.h"

CElamein::CElamein(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CElamein::CElamein(const CElamein& Prototype)
    :CMonster(Prototype)
{
}

void CElamein::Start_DefaultRadialBlur()
{
    RADIAL_BLUR_DESC RBDesc{};
    RBDesc.vCenterUV = _float2(0.5f, 0.5f);
    RBDesc.fSampleRadius = 0.05f;
    RBDesc.vMaskRadius = _float2(0.f, 0.3f);
    RBDesc.fExponent = 1.f;
    RBDesc.iNumSamples = 16;
    RBDesc.fAttenuation = 0.1f;

    //이거 3개만 건드리셈 - 이단비
    RBDesc.fStrength = 0.5f;       // Target Strength(0 ~ 1) -> 이 강도를 최대값으로 사용하여 보간 적용됨 1로 갈수록 진해짐
    RBDesc.fDuration = 2.f;        // 시간
    RBDesc.vFadeTime = _float2(0.25f, 0.5f);    //페이드 인, 아웃 시간
    m_pGameInstance->Start_RadialBlur(RBDesc);
}

void CElamein::Start_DefaultVignette()      
{
    VIGNETTE_CONFIG Config{};
    Config.eMode = VIGNETTE_CONFIG::SMOOTH_SMOOTH;
    Config.vColor = _float3(0.4f, 0.f, 0.66f);
    Config.fPower = 3.5f;
    Config.fIntensity = 1.f; 
    Config.fMaxIntensity = 2.5f;
    m_pGameInstance->Start_VignetteAnimation(1.32f, Config);  //시간(매개변수1)
}

void CElamein::LockOnLerp(_float fTimeDetla, _float fSpeed)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDetla, fSpeed);
}

void CElamein::LockOn()  
{
    m_pTransformCom->LookAt(m_pTarget->Get_Position());
}

void CElamein::BurutalUI_On(_float fTime)
{
    m_pBrutalAttack = nullptr;
    m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
    m_pBrutalAttack->Setting_BrutalAttack(m_vLockOnPosition, fTime);
    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), TEXT("Layer_UI"), m_pBrutalAttack);
}

void CElamein::BurutalUI_Off()
{
    if (m_pBrutalAttack == nullptr)
        return;

    m_pBrutalAttack->Off_BrutalAttack();
    m_pBrutalAttack = nullptr;
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
    m_pUI_HP->Update_Visible(false);
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
    TARGET_DIR eDir = Get_DIR();
    if (*m_Data.pCulStamina <= 0.f)
    {
        __super::Take_Damage(fDamage, eHitreaction, pGameObject);
        Damage_Sound();
        return;
    }

    if (m_Data.eHitType != HITREACTION::BRUTAL_ATTACK)
    {
        if (eDir == TARGET_DIR::F || eDir == TARGET_DIR::FL || eDir == TARGET_DIR::FR)
        {
            if (m_Data.fDodgeCool <= 0.f && !m_pController->Get_State_Machine()->Check_Flag(ENUM_CLASS(MONSTATE::LOCKON) && m_Data.eHitType != HITREACTION::BRUTAL_ATTACK))
            {
                m_pController->AI_ApplyDamage(pGameObject, fDamage, ENUM_CLASS(eHitreaction), 3.f);
                return;
            }
        }
    }
    if (!m_Data.isGuard)
    {
        __super::Take_Damage(fDamage, eHitreaction, pGameObject);
        Damage_Sound();
    }
    else
    {
        m_pController->AI_ApplyDamage(pGameObject, fDamage, ENUM_CLASS(eHitreaction), 3.f);
    }

}

const TRAIL_CONFIG& CElamein::Get_TrailConfig() const
{
    return m_pMeshTrail->Get_TrailConfig();
}

void CElamein::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    m_pMeshTrail->Set_TrailConfig(Config);
}

_uint CElamein::Get_NumTrailTextures()
{
    return m_pMeshTrail->Get_NumTrailTextures();
}

ID3D11ShaderResourceView* CElamein::Get_TrailTexture(_uint iIndex)
{
    return m_pMeshTrail->Get_TrailTexture(iIndex);
}

void CElamein::Creature_Release()
{
    m_isHit = false;
    m_pHitBodyCom->Collision_Active(m_isHit);
    m_isGhost = true;
    m_isActive = false;
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
    m_pLockOnSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Spine2");
    m_vLockOnPosition = &m_vLockOnPos;

    CHECK_FAILED(Ready_AnimEvent(), E_FAIL);
    CHECK_FAILED(Ready_Components(), E_FAIL);

    CMeshTrail::TRAIL_DESC MeshDesc{};
    MeshDesc.iTextureIdx = 20;
    MeshDesc.fLifeTime = 0.4f;
    MeshDesc.iDivisionCount = 10.f;
    MeshDesc.vColor = _float4(1.705f, 1.705f, 1.705f, 1.f);
    m_pMeshTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));

    m_fRecoveryPerSec = 100.f;
    m_pGameInstance->Subscribe_Event<EVENT_RESPOWN>(ENUM_CLASS(EVENT_TYPE::RESPOWN), [&](const EVENT_RESPOWN& e) {ReSpown(); });

    m_vDecalSize[ENUM_CLASS(DECALTYPE::LINEAR)] = { 3.f, 5.f };
    m_vDecalSize[ENUM_CLASS(DECALTYPE::CIRCLE)] = { 4.f, 6.f };
    m_vDecalSize[ENUM_CLASS(DECALTYPE::CURVE)] = { 3.f, 5.f };

    return S_OK;
}

void CElamein::Priority_Update(_float fTimeDelta)
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
}

void CElamein::Update(_float fTimeDelta)
{
    //m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
    m_fTimeDelta = fTimeDelta;

    if (m_Data.fAttackCool > 0.f)
        m_Data.fAttackCool -= fTimeDelta;

    if (m_Data.fGuardCool > 0.f)
        m_Data.fGuardCool -= fTimeDelta;

    if (m_Data.fSpecial_AttackCool > 0.f)
        m_Data.fSpecial_AttackCool -= fTimeDelta;

    if (m_Data.fLong_AttackCool > 0.f)
        m_Data.fLong_AttackCool -= fTimeDelta;

    if (m_Data.fDodgeCool > 0.f)
        m_Data.fDodgeCool -= fTimeDelta;

    _float4x4 LockOnMatrix{};
    XMStoreFloat4x4(&LockOnMatrix, XMLoadFloat4x4(m_pLockOnSocketMatrix) * m_pTransformCom->Get_WorldMatrix());
    m_vLockOnPos = { LockOnMatrix._41, LockOnMatrix._42, LockOnMatrix._43, 1.f };

    _float4x4 SwordMatrix = m_pSword->Get_CombindMat();
    _vector vUp = { SwordMatrix._21, SwordMatrix._22, SwordMatrix._23 };
    _vector vSwordPos = { SwordMatrix._41, SwordMatrix._42, SwordMatrix._43 };

    _vector vSwordStart = vSwordPos + XMVector3Normalize(vUp) * 0.1f;
    _vector vSwordEnd = vSwordPos + XMVector3Normalize(vUp) * 1.f;
    XMStoreFloat4(&m_vSword_Start, XMVectorSetW(vSwordStart, 1.f));
    XMStoreFloat4(&m_vSword_End, XMVectorSetW(vSwordEnd, 1.f));
   
    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);
    Update_UIHp();
    Update_Body(fTimeDelta);

    m_pMeshTrail->Update(fTimeDelta);
}

void CElamein::Late_Update(_float fTimeDelta)
{
    if (m_Data.isSearch)
        m_pBodyComp->Collision_Active(false);

    CContainerObject::Late_Update(fTimeDelta);
    
    m_pMeshTrail->Late_Update(fTimeDelta);

    if (!m_Data.isSearch)
        return;

    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CElamein::Render()
{
    m_pBody->Render();
    m_pShield->Render();
    m_pSword->Render();
    return S_OK;
}

void CElamein::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);

    if (COLLISION_LAYER::MAP_STATIC == eLayer)
        m_Data.isWallCrushed = true;

    if (COLLISION_LAYER::PLAYER == eLayer && ENUM_CLASS(COLLISION_LAYER::MONSTER) == pMyDesc->iObjectLayer)
    {
        m_pController->AI_React_Collision(pDesc, iOtherObjectLayer, this);
    }

    if (pMyDesc->iObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER_SEARCH))
    {
        m_Data.isSearch = true;
    }
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

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Elamein_Sword"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Elamein/Elamein_Sword/Elamein_Sword.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Elamein_Shield"),
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
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 1.f;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc), E_FAIL);


    _vector vMatScale{}, vMatQuat{}, vMatPos{};
    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
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
    m_tHitCollisionDesc.pGameObject = this;
    BodyDesc.vShapeOffset = _float3(-0.f, 0.5f, 0.f);
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
    pModel->Register_Event("NormalAtk_1_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; 
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalattack01_a_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_normalatk01_rustle_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2), 3.f); 
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("NormalAtk_1_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalattack01_b_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_normalatk01_rustle_b_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2), 3.f); 
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("NormalAtk_1_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalattack01_c_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_normalatk01_rustle_c_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2), 3.f); 
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("NormalAtk_1_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 4.f); m_pTransformCom->Go_Straight(m_fTimeDelta); });
    pModel->Register_Event("NormalAtk_1_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 4.f); m_pTransformCom->Go_Straight(m_fTimeDelta); });
    pModel->Register_Event("NormalAtk_1_3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 4.f); m_pTransformCom->Go_Straight(m_fTimeDelta);});
    pModel->Register_Event("NormalAtk_1_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; m_Data.iAnimIndex = 65; });
    pModel->Register_Event("NormalAtk_1_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; m_Data.iAnimIndex = 66; });
    pModel->Register_Event("NormalAtk_1_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("NormalAtk_2_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED;
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalatk02_a_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_normalatk02_rustle_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Shield_Swish_S_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("NormalAtk_2_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED;
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalatk02_b_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_normalatk02_rustle_b_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Shield_Swish_S_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("NormalAtk_2_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED;
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalatk02_b_02 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_normalatk02_rustle_c_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Shield_Swish_S_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("NormalAtk_2_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { LockOnLerp(m_fTimeDelta, 4.f); m_pTransformCom->Go_Straight(m_fTimeDelta);});
    pModel->Register_Event("NormalAtk_2_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { LockOnLerp(m_fTimeDelta, 4.f); m_pTransformCom->Go_Straight(m_fTimeDelta);});
    pModel->Register_Event("NormalAtk_2_3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { LockOnLerp(m_fTimeDelta, 4.f); m_pTransformCom->Go_Straight(m_fTimeDelta);});
    pModel->Register_Event("NormalAtk_2_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; m_Data.iAnimIndex = 68; });
    pModel->Register_Event("NormalAtk_2_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; m_Data.iAnimIndex = 69; });
    pModel->Register_Event("NormalAtk_2_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("NormalAtk_3_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED;
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalatk03_a_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_normalatk03_rustle_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Shield_Swish_S_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("NormalAtk_3_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalatk03_a_02 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_normalatk03_rustle_b_01 (SFX).wav"), Get_Position(), Get_SoundChannel(2), 3.f);
                                                                                     m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("NormalAtk_3_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { LockOnLerp(m_fTimeDelta, 4.f); m_pTransformCom->Go_Straight(m_fTimeDelta); });
    pModel->Register_Event("NormalAtk_3_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); LockOnLerp(m_fTimeDelta, 4.f); m_pTransformCom->Go_Straight(m_fTimeDelta); });
    pModel->Register_Event("NormalAtk_3_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; m_Data.iAnimIndex = 71; });
    pModel->Register_Event("NormalAtk_3_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("ShieldStomp", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED; 
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_shieldstomptier02_a_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_shieldstomp_impact_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("ShieldStomp", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("HeadCrusher", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_normalattack01_a_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                   m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("HeadCrusher", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("HeadCrusher", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("RapidSlash", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_rapidslash_a_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("RapidSlash", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("RapidSlash", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("JumpSmash", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED;
                                                                                 m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_shieldstomptier02_a_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                 m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_shieldstomp_impact_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("JumpSmash", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("GuardCounter", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State = (_uint)ATTACK_BODY::SHILED | (_uint)ATTACK_BODY::SWORD; });
    pModel->Register_Event("GuardCounter", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("GuardCounter", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    pModel->Register_Event("Arranged_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD; Set_EnchantTrail();
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("Arranged_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("Arranged_3_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("Arranged_3_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::RIGHT_LEG;
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Shield_Swish_S_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("Arranged_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SHILED;
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Shield_Swish_S_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("Arranged_5_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("Arranged_5_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::SWORD;
                                                                                  m_pGameInstance->PlaySoundOnce(TEXT("Mon_Elamein_Sword_Stab_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });

    pModel->Register_Event("Arranged_3_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAnimIndex = 86;});
    pModel->Register_Event("Arranged_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("Arranged_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("Arranged_3_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("Arranged_5_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });
    pModel->Register_Event("Arranged_5_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail(); });

    pModel->Register_Event("Arranged_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; m_Data.iAnimIndex = 84; });
    pModel->Register_Event("Arranged_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; m_Data.iAnimIndex = 85; });
    pModel->Register_Event("Arranged_3_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });
    pModel->Register_Event("Arranged_3_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });
    pModel->Register_Event("Arranged_4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; m_Data.iAnimIndex = 87; });
    pModel->Register_Event("Arranged_5_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });
    pModel->Register_Event("Arranged_5_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; Set_DefaultTrail(); });


    //MoveSound
    pModel->Register_Event("Move_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Move_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("Move_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });

    pModel->Register_Event("Run_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("Run_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });

    //Effect
    pModel->Register_Event("Wind_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Smoke_Small"), m_pTransformCom->Get_State(STATE::POSITION)); });

    pModel->Register_Event("HeadCrusher_Jump_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Jump"), m_pTransformCom->Get_State(STATE::POSITION));});
    pModel->Register_Event("HeadCrusher_Jump_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Land_Sword"), XMLoadFloat4(&m_vSword_End)); });

    pModel->Register_Event("NormalAtk_Jump_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Jump"), m_pTransformCom->Get_State(STATE::POSITION)); });
    pModel->Register_Event("NormalAtk_Jump_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _float4x4 SwordMatrix = m_pSword->Get_CombindMat();
        _vector vSwordPos = XMVectorSet(SwordMatrix._41, SwordMatrix._42, SwordMatrix._43, 1.f);
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Land_Sword"), vSwordPos);
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Smoke_Small"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("RapidSlash_Dust_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Smoke_Small"), m_pTransformCom->Get_State(STATE::POSITION)); });

    pModel->Register_Event("Enchant_Sound", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _float4x4 SwordMatrix = m_pSword->Get_CombindMat();
        _vector pos = XMVectorSet(SwordMatrix._41, SwordMatrix._42, SwordMatrix._43, 1.f);  
        _vector vUp = XMVectorSet(SwordMatrix._21, SwordMatrix._22, SwordMatrix._23, 0.f);
        pos += XMVector3Normalize(vUp) * 0.45f;  
        m_iFXIdx = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Spark_Loop"), pos); });
    pModel->Register_Event("Enchant_Sound", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _float4x4 SwordMatrix = m_pSword->Get_CombindMat();
        _vector pos = XMVectorSet(SwordMatrix._41, SwordMatrix._42, SwordMatrix._43, 1.f);
        _vector vUp = XMVectorSet(SwordMatrix._21, SwordMatrix._22, SwordMatrix._23, 0.f);
        pos += XMVector3Normalize(vUp) * 0.45f;  
        m_pGameInstance->Update_Effect_Position(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Spark_Loop"), m_iFXIdx, pos); });
    pModel->Register_Event("Enchant_Sound", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Spark_Loop"));});
    
    pModel->Register_Event("ShieldStomp_Land_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _float4x4 SwordMatrix = m_pShield->Get_CombindMat();
        _vector pos = XMVectorSet(SwordMatrix._41, SwordMatrix._42, SwordMatrix._43, 1.f);
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Land_Shield"), pos); });
    
    pModel->Register_Event("ShieldThrow_Land_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _float4x4 SwordMatrix = m_pShield->Get_CombindMat();
        _vector pos = XMVectorSet(SwordMatrix._41, SwordMatrix._42, SwordMatrix._43, 1.f);
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Land_Shield"), pos);
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Smoke_Small"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    //범수야도와줘라
    //pModel->Register_Event("SphereWind_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    _matrix SwordMatrix = m_pSword->Get_Transform()->Get_WorldMatrix();
    //    m_iFXIdx = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Sword_Wind"), SwordMatrix, XMLoadFloat4(&m_vSword_End)); });
    //
    //pModel->Register_Event("SphereWind_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
    //    _matrix SwordMatrix = m_pSword->Get_Transform()->Get_WorldMatrix();
    //    m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Sword_Wind"), m_iFXIdx, SwordMatrix, XMLoadFloat4(&m_vSword_End)); });
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

    m_Data.fEdgeWidth = 0.05f;
    m_Data.fEdgeColor = { 1.3f, 0.75f, 0.f, 1.f };
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
}

void CElamein::Update_MeshTrail()
{
    _vector vSwordStart = XMLoadFloat4(&m_vSword_Start);
    _vector vSwordEnd = XMLoadFloat4(&m_vSword_End);
    m_pMeshTrail->Add_ControlPoint(vSwordEnd, vSwordStart);
}

void CElamein::Set_DefaultTrail()
{
    TRAIL_CONFIG Config{};
    Config.fLifeTime = 0.25f;
    Config.iTextureIdx = 8;
    Config.iDivisionCount = 10;
    Config.vColor = _float4(1.7058f, 1.7058f, 1.7058f, 1.f);
    m_pMeshTrail->Set_TrailConfig(Config);
}

void CElamein::Set_EnchantTrail()
{
    TRAIL_CONFIG Config{};
    Config.fLifeTime = 0.25f;
    Config.iTextureIdx = 8;
    Config.iDivisionCount = 10;
    Config.vColor = _float4(2.455f, 1.937f, 2.784f, 1.f);
    m_pMeshTrail->Set_TrailConfig(Config);
}

void CElamein::Move_Sound()
{
    switch (m_pGameInstance->Rand(1, 6))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_footstep_01 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_footstep_02 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_footstep_03 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_footstep_04 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    case 5:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_footstep_05 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_footstep_06 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 1.5f);             break;
    }

    switch (m_pGameInstance->Rand(1, 6))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_rustle_01 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_rustle_02 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_rustle_03 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_rustle_04 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 5:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_rustle_05 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_walk_rustle_06 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    }
}

void CElamein::Run_Sound()
{
    switch (m_pGameInstance->Rand(1, 8))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_footstep_01 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 2.5f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_footstep_02 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 2.5f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_footstep_03 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 2.5f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_footstep_04 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 2.5f);             break;
    case 5:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_footstep_05 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 2.5f);             break;
    case 6:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_footstep_06 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 2.5f);             break;
    case 7:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_footstep_07 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 2.5f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_footstep_08 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 2.5f);             break;
    }

    switch (m_pGameInstance->Rand(1, 8))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_rustle_01 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_rustle_02 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_rustle_03 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_rustle_04 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 5:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_rustle_05 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 6:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_rustle_06 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    case 7:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_rustle_07 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_run_rustle_08 (SFX).wav"), Get_Position(), Get_SoundChannel(4), 1.5f);             break;
    }
}

void CElamein::Damage_Sound()
{
    switch (m_pGameInstance->Rand(1, 5))
    {
    case 1:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_dmgstrong_01 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    case 2:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_dmgstrong_02 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    case 3:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_dmgstrong_03 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    case 4:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_dmgstrong_04 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    default:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_dmgstrong_05 (Korean(KR)).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    }
}

void CElamein::ReSpown()
{
    if (!m_isGhost)
        return;
    m_Data.isSleep = true;
    *m_Data.pCulHp = *m_Data.pMaxHp;
    *m_Data.pCulStamina = *m_Data.pMaxStamina;
    m_Data.fDecreaseAlpha = 0.f;

    m_isHit = true;
    m_pHitBodyCom->Collision_Active(m_isHit);
    m_isGhost = false;
    m_isActive = true;
    m_pTransformCom->Set_WorldMatrix_4x4(m_OriginMat);
    m_pController->Get_BlackBoard()->Set_Value(m_strName, "isDetected", false);

    m_Data.eHitType = HITREACTION::END;
    m_Data.iBrutalHit = 0;
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
    if (m_pBrutalAttack != nullptr)
        m_pBrutalAttack->Off_BrutalAttack();

    Safe_Release(m_pMeshTrail);
    if (m_pBrutalAttack != nullptr)
        m_pBrutalAttack->Off_BrutalAttack();

    if (m_pUI_HP != nullptr)
        m_pUI_HP->Set_IsDead(true);

    __super::Free();
    Safe_Release(m_pBody);
    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pSword);
    Safe_Release(m_pShield);
    Safe_Release(m_pBodyComp);
    m_Data.pOwner = nullptr;

}
