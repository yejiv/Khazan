#include "Dragonian_Rampage.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Dragonian_Rampage.h"
#include "Dragonian_Claw_L.h"
#include "Dragonian_Claw_R.h"

#include "AI_Controller_Dragonian_Rampage.h"

#include "Mon_Hp.h"
#include "MeshTrail.h"

#include "Target_BrutalAttack.h"
CDragonian_Rampage::CDragonian_Rampage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CDragonian_Rampage::CDragonian_Rampage(const CDragonian_Rampage& Prototype)
    :CMonster(Prototype)
{
}

void CDragonian_Rampage::LockOnLerp(_float fTimeDetla, _float fSpeed)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDetla, fSpeed);
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
    m_pUI_HP = nullptr;
}

_bool CDragonian_Rampage::Check_Ranage(string strKey)
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

_bool CDragonian_Rampage::Check_Ranage(_float fRange)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));

    if (fRange <= 0)
        return true;

    if (fDist <= fRange)
        return true;
    else
        return false;
}

TARGET_DIR CDragonian_Rampage::Get_DIR()
{
    return Check_Dir(m_pTransformCom->Get_WorldMatrix(), m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
}

void CDragonian_Rampage::BurutalUI_On(_float fTime)
{
    m_pBrutalAttack = nullptr;
    m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
    m_pBrutalAttack->Setting_BrutalAttack(m_vLockOnPosition, fTime);
    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pBrutalAttack);
}

void CDragonian_Rampage::BurutalUI_Off()
{
    if (m_pBrutalAttack == nullptr)
        return;

    m_pBrutalAttack->Off_BrutalAttack();
    m_pBrutalAttack = nullptr;
}

void CDragonian_Rampage::Creature_Release()
{
    m_pHitBodyCom->Collision_Active(false);

    __super::Creature_Release();
}

HRESULT CDragonian_Rampage::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDragonian_Rampage::Initialize_Clone(void* pArg)
{
    DRAGON_RAMPAGE_MONSTER_DESC* pDesc = static_cast<DRAGON_RAMPAGE_MONSTER_DESC*>(pArg);

    m_Data.isMotionSleep = pDesc->isSleep;

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_MonData(), E_FAIL);
    CHECK_FAILED(Ready_ETC(), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(), E_FAIL);
    m_pHeadMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Head");
    m_pBodySocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Spine2");
    m_pLockOnSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("FX_Body_ExpGained");
    m_vLockOnPosition = &m_vLockOnPos;

    CHECK_FAILED(Ready_AnimEvent(), E_FAIL);
    CHECK_FAILED(Ready_Components(), E_FAIL);

    for (_uint i = 0; i < ENUM_CLASS(CLAW::END); ++i)
    {
        CMeshTrail::TRAIL_DESC MeshDesc{};
        MeshDesc.iTextureIdx = 2;
        MeshDesc.fLifeTime = 0.6f;
        MeshDesc.iDivisionCount = 10.f;
        MeshDesc.vColor = _float4(2.176f, 1.824f, 1.176f, 1.f);
        m_pMeshTrail[i] = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));
    }

    m_fRecoveryPerSec = 10.f;

    return S_OK;
}

void CDragonian_Rampage::Priority_Update(_float fTimeDelta)
{
    if (!m_Data.isPageChange && *m_Data.pCulHp <= *m_Data.pMaxHp * 0.4f)
    {
        m_Data.isPageChange = true;
        m_Data.isLockOn = true;
    }
    CContainerObject::Priority_Update(fTimeDelta);

    for (auto& pMeshTrail : m_pMeshTrail)
        pMeshTrail->Priority_Update(fTimeDelta);

    m_isRequestRecoveryStamina = m_Data.isStamina_Regen;
    if (m_Data.isStamina_Regen)
    {
        Recovery_Stamina(fTimeDelta * 12.f);
        if (m_fCurrentStamina == m_fMaxStamina)
            m_Data.isStamina_Regen = false;
    }
}

void CDragonian_Rampage::Update(_float fTimeDelta)
{
    //m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
    m_fTimeDelta = fTimeDelta;

    if (m_Data.fAttackCool >= 0.f)
        m_Data.fAttackCool -= fTimeDelta;

    _float4x4 LockOnMatrix{};
    XMStoreFloat4x4(&LockOnMatrix, XMLoadFloat4x4(m_pLockOnSocketMatrix) * m_pTransformCom->Get_WorldMatrix());
    m_vLockOnPos = { LockOnMatrix._41, LockOnMatrix._42, LockOnMatrix._43, 1.f };


    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);
    Update_UIHp();
    Update_Body(fTimeDelta);

    Update_WeaponPos();
    for (auto& pMeshTrail : m_pMeshTrail)
        pMeshTrail->Update(fTimeDelta);
    //  Update_MeshTrail_R();
    //  Update_MeshTrail_L();
}

void CDragonian_Rampage::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);

    for (auto& pMeshTrail : m_pMeshTrail)
        pMeshTrail->Late_Update(fTimeDelta);
}

void CDragonian_Rampage::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
    if (m_Data.eHitType == HITREACTION::BRUTAL_ATTACK)
        ++m_Data.iBrutalHit;

    switch (m_pGameInstance->Rand(1, 3))
    {
    case 1:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_E_Dmg_L_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    case 2:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_E_Dmg_L_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    default:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_E_Dmg_L_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0));
        break;
    }

    __super::Take_Damage(fDamage, eHitreaction, pGameObject);
}

const TRAIL_CONFIG& CDragonian_Rampage::Get_TrailConfig() const
{
    return m_pMeshTrail[0]->Get_TrailConfig();
}

void CDragonian_Rampage::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    for (auto& pMeshTrail : m_pMeshTrail)
        pMeshTrail->Set_TrailConfig(Config);
}

_uint CDragonian_Rampage::Get_NumTrailTextures()
{
    return m_pMeshTrail[0]->Get_NumTrailTextures();
}

ID3D11ShaderResourceView* CDragonian_Rampage::Get_TrailTexture(_uint iIndex)
{
    return m_pMeshTrail[0]->Get_TrailTexture(iIndex);
}

void CDragonian_Rampage::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);

    if (COLLISION_LAYER::MAP_STATIC == eLayer)
        m_Data.isWallCrushed = true;
}

void CDragonian_Rampage::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CDragonian_Rampage::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

HRESULT CDragonian_Rampage::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Rampage"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Rampage/Dragonian_Rampage/Dragonian_Rampage.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_DragonClaw_R"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Rampage/DragonClaw_R/DragonClaw_R.dat")),E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_DragonClaw_L"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Rampage/DragonClaw_L/DragonClaw_L.dat")),E_FAIL);


    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Rampagee_Body"),
        CBody_Dragonian_Rampage::Create(m_pDevice, m_pContext, m_iPrototypeIndex)),E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Claw_L"),
        CDragonian_Claw_L::Create(m_pDevice, m_pContext, m_iPrototypeIndex)),E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Claw_R"),
        CDragonian_Claw_R::Create(m_pDevice, m_pContext, m_iPrototypeIndex)),E_FAIL);

    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_ETC()
{
    m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));
    CHECK_NULLPTR(m_pUI_HP, E_FAIL);
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
    tCharVirDesc.vShapeOffset = _float3(0.f, 1.75f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CONTROLLER);
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fPenetrationRecoverySpeed = 0.1f;

    m_tCollisionDesc.pGameObject = this;
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;
    
    tCharVirDesc.fRadius = 1.6f;
    tCharVirDesc.fHeight = 0.5f;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc), E_FAIL);


    _vector vMatScale{}, vMatQuat{}, vMatPos{};

    CBody::BODY_BOXSHAPE_DESC BodyDesc{};

    BodyDesc.vExtent = { 2.2f, 1.f, 1.f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    BodyDesc.bIsTrigger = true;

    _matrix BodyMat = XMLoadFloat4x4(m_pBodySocketMatrix) * m_pTransformCom->Get_WorldMatrix();
    for (uint32_t i = 0; i < 3; i++)
        BodyMat.r[i] = XMVector3Normalize(BodyMat.r[i]);

    XMMatrixDecompose(&vMatScale, &vMatQuat, &vMatPos, BodyMat);

    XMStoreFloat3(&BodyDesc.vPos, vMatPos);
    XMStoreFloat4(&BodyDesc.vQuat, vMatQuat);

    BodyDesc.vShapeOffset = _float3(-0.5f, -0.f, 0.f);
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_HitBody"), (CComponent**)&m_pHitBodyCom, &BodyDesc);
    m_pHitBodyCom->Activate(true);
    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_PartObjects()
{
    CBody_Dragonian_Rampage::BODY_DESC BodyDesc{};
    BodyDesc.pData = &m_Data;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    CHECK_FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Rampagee_Body"), &BodyDesc), E_FAIL);

    m_pBody = dynamic_cast<CBody_Dragonian_Rampage*>(Find_PartObject(TEXT("Part_Body")));
    CHECK_NULLPTR(m_pBody, E_FAIL);
    Safe_AddRef(m_pBody);

    CDragonian_Claw_L::WEAPON_DESC WeaponLDesc{};
    WeaponLDesc.pData = &m_Data;
    WeaponLDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponLDesc.pOwnerTransform = m_pTransformCom;
    WeaponLDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-L-Forearm");
    CHECK_FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon_L"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Claw_L"), &WeaponLDesc), E_FAIL);

    m_pClaw_L = dynamic_cast<CDragonian_Claw_L*>(Find_PartObject(TEXT("Part_Weapon_L")));
    CHECK_NULLPTR(m_pClaw_L, E_FAIL);
    Safe_AddRef(m_pClaw_L);

    CDragonian_Claw_R::WEAPON_DESC WeaponRDesc{};
    WeaponRDesc.pData = &m_Data;
    WeaponRDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponRDesc.pOwnerTransform = m_pTransformCom;
    WeaponRDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-R-Forearm");

    CHECK_FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon_R"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Claw_R"), &WeaponRDesc), E_FAIL);

    m_pClaw_R = dynamic_cast<CDragonian_Claw_R*>(Find_PartObject(TEXT("Part_Weapon_R")));
    CHECK_NULLPTR(m_pClaw_R, E_FAIL);
    Safe_AddRef(m_pClaw_R);

    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_AnimEvent()
{
    CModel* pModel = m_pBody->Get_Model();

    pModel->Register_Event("Tail_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State = (_uint)ATTACK_BODY::TAIL; m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_tailatk_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("Tail_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });

    //JumpClaw ENTER
    pModel->Register_Event("JumpCraw_Loar_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragoniantwinaxe_jump_atk_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 3.f); });
    pModel->Register_Event("JumpCraw_Loar_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragoniantwinaxe_jump_atk_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 3.f); });

    pModel->Register_Event("JumpClaw_1_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State = (_uint)ATTACK_BODY::HAND_R | (_uint)ATTACK_BODY::HAND_L;});
    pModel->Register_Event("JumpClaw_1_Attack_Sound", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragoniantwinaxe_jump_atk_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_jumpclaw_swish_a_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    //JumpClaw CONTINUE
    pModel->Register_Event("JumpClaw_1_Attack", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_R(); Update_MeshTrail_L(); });
    pModel->Register_Event("JumpClaw_1_Move", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Jump_Move_1(); });
    pModel->Register_Event("JumpClaw_2_Move", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Jump_Move_2(); });
    //JumpClaw EXIT
    pModel->Register_Event("JumpClaw_1_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });


    //DoubleCraw ENTER
    pModel->Register_Event("DoubleCraw_Loar_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianclaw_doubleclaw_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 3.f); });

    pModel->Register_Event("DoubleClaw_F_E_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_L;
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianclaw_doubleclaw_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_doubleclaw_swish_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("DoubleClaw_F_3_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_R;
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianclaw_doubleclaw_a_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_doubleclaw_swish_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("DoubleClaw_F_3_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_L;
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_doubleclaw_swish_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("DoubleClaw_F_E_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_R;
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_doubleclaw_swish_a_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("DoubleClaw_F_2_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_R;
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianclaw_doubleclaw_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 3.f);
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_doubleclaw_swish_a_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("DoubleClaw_F_2_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_L;
                                                                                        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_doubleclaw_swish_a_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    //DoubleCraw CONTINUE
    pModel->Register_Event("DoubleClaw_F_E_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_L(); });
    pModel->Register_Event("DoubleClaw_F_3_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_R(); });
    pModel->Register_Event("DoubleClaw_F_3_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_L(); });
    pModel->Register_Event("DoubleClaw_F_E_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_R(); });
    pModel->Register_Event("DoubleClaw_F_2_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_R(); });
    pModel->Register_Event("DoubleClaw_F_2_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_L(); });
    
    //DoubleCraw EXIT
    pModel->Register_Event("DoubleClaw_F_E_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_L; });
    pModel->Register_Event("DoubleClaw_F_E_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_R;  });
    pModel->Register_Event("DoubleClaw_F_3_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_R; });
    pModel->Register_Event("DoubleClaw_F_3_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_L;  });
    pModel->Register_Event("DoubleClaw_F_2_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_R; });
    pModel->Register_Event("DoubleClaw_F_2_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_L;  });

    //ChainCraw ENTER
    pModel->Register_Event("ChainCraw_Loar_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianclaw_doubleclaw_a_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0), 3.f); });
    pModel->Register_Event("ChainCraw_F_Attack_1_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_L;
                                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_chainclaw_swish_c_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("ChainCraw_F_Attack_1_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_R;
                                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_chainclaw_swish_c_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f); });
    pModel->Register_Event("ChainCraw_F_Attack_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_R;
                                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_chainclaw_swish_c_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f);
                                                                                            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_01 (SFX).wav"), Get_Position(), Get_SoundChannel(0)); });
    pModel->Register_Event("ChainCraw_F_Attack_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_L; 
                                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_chainclaw_swish_c_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f);
                                                                                            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_02 (SFX).wav"), Get_Position(), Get_SoundChannel(0)); });
    pModel->Register_Event("ChainCraw_F_Attack_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State |= (_uint)ATTACK_BODY::HAND_R;
                                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_chainclaw_swish_d_01 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f);
                                                                                            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_03 (SFX).wav"), Get_Position(), Get_SoundChannel(0)); });
    pModel->Register_Event("ChainCraw_F_Attack_2_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAttackBody_State = (_uint)ATTACK_BODY::HAND_R | (_uint)ATTACK_BODY::HAND_L;
                                                                                                m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonianclaw_chainclaw_swish_d_02 (SFX).wav"), Get_Position(), Get_SoundChannel(1), 3.f);
                                                                                            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Atk_S_04 (SFX).wav"), Get_Position(), Get_SoundChannel(0)); });
    pModel->Register_Event("NextEvent_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {m_Data.iAnimIndex = 15; });
    //ChainCraw CONTINUE
    pModel->Register_Event("ChainCraw_F_Attack_1_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_L(); });
    pModel->Register_Event("ChainCraw_F_Attack_1_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_R(); });
    pModel->Register_Event("ChainCraw_F_Attack_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_R(); });
    pModel->Register_Event("ChainCraw_F_Attack_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_L(); });
    pModel->Register_Event("ChainCraw_F_Attack_3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_R(); });
    pModel->Register_Event("ChainCraw_F_Attack_2_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Update_MeshTrail_R(); Update_MeshTrail_L(); });
    
    //ChainCraw EXIT
    pModel->Register_Event("ChainCraw_F_Attack_1_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_L; });
    pModel->Register_Event("ChainCraw_F_Attack_1_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_R; m_Data.iAnimIndex = 16;  });
    pModel->Register_Event("ChainCraw_F_Attack_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_R; });
    pModel->Register_Event("ChainCraw_F_Attack_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_L; });
    pModel->Register_Event("ChainCraw_F_Attack_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State &= ~(_uint)ATTACK_BODY::HAND_R; });
    pModel->Register_Event("ChainCraw_F_Attack_2_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {m_Data.iAttackBody_State = 0; });



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


    pModel->Register_Event("JumpCraw_Move_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("JumpCraw_Move_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("JumpCraw_Move_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });


    pModel->Register_Event("ChainCraw_Move_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("ChainCraw_Move_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("ChainCraw_Move_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("ChainCraw_Move_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("ChainCraw_Move_5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("ChainCraw_Move_6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });
    pModel->Register_Event("ChainCraw_Move_7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Move_Sound(); });

    pModel->Register_Event("DoubleCraw_Run_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_8", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_9", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_10", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_11", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });
    pModel->Register_Event("DoubleCraw_Run_12", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Run_Sound(); });

    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_MonData()
{
    m_Data.pOwner = this;
    m_Data.fGloggyTime = 7.f;
    m_Data.pCulHp = &m_fCurrentHP;
    m_Data.pMaxHp = &m_fMaxHP;

    m_Data.pCulStamina = &m_fCurrentStamina;
    m_Data.pMaxStamina = &m_fMaxStamina;

    m_Data.fEdgeWidth = 0.1f;
    m_Data.fEdgeColor = { 4.2f, 1.6f, 0.2f, 1.f };
    return S_OK;
}

void CDragonian_Rampage::Update_UIHp()
{
    m_vHpPos = { m_pHeadMatrix->m[3][0], m_pHeadMatrix->m[3][1], m_pHeadMatrix->m[3][2], 1.f };
    XMStoreFloat4(&m_vHpPos, XMVector4Transform(XMLoadFloat4(&m_vHpPos), m_pTransformCom->Get_WorldMatrix()));
    m_vHpPos.y += 0.5f;
}

void CDragonian_Rampage::Update_Body(_float fTimeDelta)
{
    _vector vMatScale{}, vMatQuat{}, vMatPos{};
    _matrix HitMat = XMLoadFloat4x4(m_pBodySocketMatrix);
    for (uint32_t i = 0; i < 3; i++)
        HitMat.r[i] = XMVector3Normalize(HitMat.r[i]);
    HitMat *= m_pTransformCom->Get_WorldMatrix();

    XMMatrixDecompose(&vMatScale, &vMatQuat, &vMatPos, HitMat);
    m_pHitBodyCom->Sync_Update(HitMat);
    m_pHitBodyCom->Update(fTimeDelta, HitMat, vMatQuat, vMatPos);

}

void CDragonian_Rampage::Jump_Move_1()
{
    LockOnLerp(m_fTimeDelta, 3.f);
    m_pTransformCom->Go_Straight(2.f * m_fTimeDelta);
}

void CDragonian_Rampage::Jump_Move_2()
{
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);
    m_pTransformCom->LookAt(vTargetPos);

    _vector vOffsetPos = XMVectorLerp(vPos, vTargetPos, (m_pBody->Get_CulTrack() - 60.f) / 106.f);
    m_pTransformCom->Set_State(STATE::POSITION, vOffsetPos);
}

void CDragonian_Rampage::Update_WeaponPos()
{
    _float4x4 m_vClawLMat = m_pClaw_L->Get_CombindMat();
    _vector vClawLRight = XMVector3Normalize({ m_vClawLMat._11, m_vClawLMat._12, m_vClawLMat._13 });
    _vector vClawLUp = XMVector3Normalize({ m_vClawLMat._21, m_vClawLMat._22, m_vClawLMat._23 });
    _vector vClawLLook = XMVector3Normalize({ m_vClawLMat._31, m_vClawLMat._32, m_vClawLMat._33 });
    _vector vClawLPos = { m_vClawLMat._41, m_vClawLMat._42, m_vClawLMat._43 };

    _vector vClawLStart = vClawLPos - vClawLRight * 1.5f;
    _vector vClawLEnd = vClawLPos - vClawLRight * 1.85f - vClawLUp * 0.6f + vClawLLook * 0.25f;
    XMStoreFloat4(&m_vClawL_1_Start, XMVectorSetW(vClawLStart, 1.f));
    XMStoreFloat4(&m_vClawL_1_End, XMVectorSetW(vClawLEnd, 1.f));

    vClawLStart = vClawLPos - vClawLRight * 1.5f - vClawLUp * 0.3f - vClawLLook * 0.2f;
    vClawLEnd = vClawLPos - vClawLRight * 1.85f - vClawLUp * 0.72f - vClawLLook * 0.11f;
    XMStoreFloat4(&m_vClawL_2_Start, XMVectorSetW(vClawLStart, 1.f));
    XMStoreFloat4(&m_vClawL_2_End, XMVectorSetW(vClawLEnd, 1.f));

    vClawLStart = vClawLPos - vClawLRight * 1.5f + vClawLUp * 0.f + vClawLLook * 0.4f;
    vClawLEnd = vClawLPos - vClawLRight * 1.85f - vClawLUp * 0.5f + vClawLLook * 0.58f;
    XMStoreFloat4(&m_vClawL_3_Start, XMVectorSetW(vClawLStart, 1.f));
    XMStoreFloat4(&m_vClawL_3_End, XMVectorSetW(vClawLEnd, 1.f));

    m_vClawLMat = m_pClaw_R->Get_CombindMat();
    vClawLRight = XMVector3Normalize({ m_vClawLMat._11, m_vClawLMat._12, m_vClawLMat._13 });
    vClawLUp = XMVector3Normalize({ m_vClawLMat._21, m_vClawLMat._22, m_vClawLMat._23 });
    vClawLLook = XMVector3Normalize({ m_vClawLMat._31, m_vClawLMat._32, m_vClawLMat._33 });
    vClawLPos = { m_vClawLMat._41, m_vClawLMat._42, m_vClawLMat._43 };
    vClawLStart = vClawLPos - vClawLRight * 1.5f;
    vClawLEnd = vClawLPos - vClawLRight * 2.1f + vClawLUp * 0.6f + vClawLLook * 0.25f;
    XMStoreFloat4(&m_vClawR_1_Start, XMVectorSetW(vClawLStart, 1.f));
    XMStoreFloat4(&m_vClawR_1_End, XMVectorSetW(vClawLEnd, 1.f));

    vClawLStart = vClawLPos - vClawLRight * 1.6f + vClawLUp * 0.2f - vClawLLook * 0.35f;
    vClawLEnd = vClawLPos - vClawLRight * 1.86f + vClawLUp * 0.7f - vClawLLook * 0.25f;
    XMStoreFloat4(&m_vClawR_2_Start, XMVectorSetW(vClawLStart, 1.f));
    XMStoreFloat4(&m_vClawR_2_End, XMVectorSetW(vClawLEnd, 1.f));

    vClawLStart = vClawLPos - vClawLRight * 1.6f - vClawLUp * 0.f + vClawLLook * 0.45f;
    vClawLEnd = vClawLPos - vClawLRight * 1.9f + vClawLUp * 0.5f + vClawLLook * 0.55f;
    XMStoreFloat4(&m_vClawR_3_Start, XMVectorSetW(vClawLStart, 1.f));
    XMStoreFloat4(&m_vClawR_3_End, XMVectorSetW(vClawLEnd, 1.f));
}

void CDragonian_Rampage::Update_MeshTrail_R()
{
    _vector vClawStart_1 = XMLoadFloat4(&m_vClawR_1_Start);
    _vector vClawEnd_1 = XMLoadFloat4(&m_vClawR_1_End);
    m_pMeshTrail[ENUM_CLASS(CLAW::RIGHT_1)]->Add_ControlPoint(vClawStart_1, vClawEnd_1);

    _vector vClawStart_2 = XMLoadFloat4(&m_vClawR_2_Start);
    _vector vClawEnd_2 = XMLoadFloat4(&m_vClawR_2_End);
    m_pMeshTrail[ENUM_CLASS(CLAW::RIGHT_2)]->Add_ControlPoint(vClawStart_2, vClawEnd_2);

    _vector vClawStart_3 = XMLoadFloat4(&m_vClawR_3_Start);
    _vector vClawEnd_3 = XMLoadFloat4(&m_vClawR_3_End);
    m_pMeshTrail[ENUM_CLASS(CLAW::RIGHT_3)]->Add_ControlPoint(vClawStart_3, vClawEnd_3);
}

void CDragonian_Rampage::Update_MeshTrail_L()
{
    _vector vClawStart_1 = XMLoadFloat4(&m_vClawL_1_Start);
    _vector vClawEnd_1 = XMLoadFloat4(&m_vClawL_1_End);
    m_pMeshTrail[ENUM_CLASS(CLAW::LEFT_1)]->Add_ControlPoint(vClawStart_1, vClawEnd_1);

    _vector vClawStart_2 = XMLoadFloat4(&m_vClawL_2_Start);
    _vector vClawEnd_2 = XMLoadFloat4(&m_vClawL_2_End);
    m_pMeshTrail[ENUM_CLASS(CLAW::LEFT_2)]->Add_ControlPoint(vClawStart_2, vClawEnd_2);

    _vector vClawStart_3 = XMLoadFloat4(&m_vClawL_3_Start);
    _vector vClawEnd_3 = XMLoadFloat4(&m_vClawL_3_End);
    m_pMeshTrail[ENUM_CLASS(CLAW::LEFT_3)]->Add_ControlPoint(vClawStart_3, vClawEnd_3);
}

void CDragonian_Rampage::Move_Sound()
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

void CDragonian_Rampage::Run_Sound()
{
    switch (m_pGameInstance->Rand(1, 6))
    {
    case 1:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_run_dirt_01 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    case 2:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_run_dirt_02 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    case 3:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_run_dirt_03 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    case 4:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_run_dirt_04 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    case 5:           m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_run_dirt_05 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    default:          m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_dragonian_foley_run_dirt_06 (SFX).wav"), Get_Position(), Get_SoundChannel(3), 3.f);             break;
    }
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
    for (_uint i = 0; i < ENUM_CLASS(CLAW::END); ++i)
        Safe_Release(m_pMeshTrail[i]);
    
    if (m_pBrutalAttack != nullptr)
        m_pBrutalAttack->Off_BrutalAttack();

    if (m_pUI_HP != nullptr)
        m_pUI_HP->Set_IsDead(true);

    __super::Free();
    Safe_Release(m_pBody);
    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pClaw_L);
    Safe_Release(m_pClaw_R);
    Safe_Release(m_pHitBodyCom);
    m_Data.pOwner = nullptr;
}
