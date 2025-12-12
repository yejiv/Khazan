#include "Imp_Range.h"
#include "CharacterVirtual.h"
#include "Body_Imp_Range.h"
#include "Imp_Wand.h"
#include "AI_Controller_Imp_Range.h"
#include "Projectile_Imp_MagicBall.h"
#include "Projectile_Boomarang.h"
#include "GameInstance.h"
#include "Mon_HP.h"
#include "ClientInstance.h"
#include "Amount.h"

CImp_Range::CImp_Range(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{pDevice,pContext}
{
}

CImp_Range::CImp_Range(const CImp_Range& Prototype)
    :CMonster{Prototype}
{
}

_float4* CImp_Range::Get_LockOnPosition()
{
    return nullptr;
}

HRESULT CImp_Range::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CImp_Range::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;

    if (FAILED(Ready_SFX()))
        return E_FAIL;

      m_pController = CAI_Controller_Imp_Range::Create(this);
      if (nullptr == m_pController)
          return E_FAIL;

      if (nullptr != m_pController)
          m_pController->Get_BlackBoard()->Set_Value(m_strName, "Target", m_pTarget);

      m_MagicBalls.resize(3, nullptr);
     
      return S_OK;
}

void CImp_Range::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);

    if (m_fCurrentHP <= 0.f && !m_isDeadFlag)
    {
        CClientInstance::GetInstance()->Add_SkillExp(10.f);
        static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, 100);
        m_isDeadFlag = true;
    }

}

void CImp_Range::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);

    if (m_fCurrentHP > 0.f)
    {
        if (m_isLookAt)
        {
            CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
            if (nullptr == pModel)
                return;
            _float fRatio = pModel->MakeRatio();
            Look_Target_Lerp(fTimeDelta, fRatio, 10.f);
        }
    }
  
    __super::Update(fTimeDelta);

    m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Body_ExpGained");

    if (m_isDissolve)
        m_fDecreaseAlpha += fTimeDelta * 0.7f;

    if (m_fDecreaseAlpha >= 1.f)
    {
        Creature_Release();
    }

}

void CImp_Range::Late_Update(_float fTimeDelta)
{
    if (!m_isDetected)
    {

        CBlackBoard* pBB = m_pController->Get_BlackBoard();
        if (pBB->Get_Value<_bool>(m_strName, "isDetected"))
        {
            m_isDetected = true;

            m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));

            if (m_pUI_HP != nullptr)
            {
                m_pUI_HP->Setting_HP(m_vLockOnPosition, { 0.f, 30.f }, &m_fCurrentHP, &m_fMaxHP, &m_fCurrentStamina, &m_fMaxStamina);
                m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pUI_HP);
            }
        }
    }


    CContainerObject::Late_Update(fTimeDelta);
}

HRESULT CImp_Range::Render()
{
    return S_OK;
}

void CImp_Range::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (eLayer == COLLISION_LAYER::PLAYER_ATTACK)
        int a = 10;

}

void CImp_Range::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CImp_Range::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CImp_Range::Ready_Components()
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

    m_tImp_RangeColliderDesc.pGameObject = this;
    m_tImp_RangeColliderDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    m_tImp_RangeColliderDesc.strName = TEXT("Imp_Range_Collider");
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tImp_RangeColliderDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CImp_Range::Ready_PartObjects()
{
    CBody_Imp_Range::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    BodyDesc.pOwner = this;
    BodyDesc.pDissolve = &m_isDissolve;
    BodyDesc.pDecreaseAlpha = &m_fDecreaseAlpha;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Range_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Imp_Range*>(pBody);
    Safe_AddRef(m_pBody);


    CImp_Wand::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pOwner = this;
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pOwnerTransform = m_pTransformCom;
    WeaponDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_R");
    WeaponDesc.pDissolve = &m_isDissolve;
    WeaponDesc.pDecreaseAlpha = &m_fDecreaseAlpha;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Range_Wand"), &WeaponDesc)))
        return E_FAIL;

    CPartObject* pWeapon = Find_PartObject(TEXT("Part_Weapon"));
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pWeapon = dynamic_cast<CImp_Wand*>(pWeapon);
    Safe_AddRef(m_pWeapon);
    if (nullptr == pWeapon)
        return E_FAIL;

    return S_OK;
}

HRESULT CImp_Range::Ready_Projectiles()
{
    CProjectile_Imp_MagicBall::PROJECTILE_DESC Desc{};
    Desc.fDamage = 10.f;
    Desc.fSpeedPerSec = 30.f;
    Desc.fLifeTime = 6.f;
    Desc.fRotationPerSec = 180.f;

    m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Imp_Magic_Ball"),
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Imp_MagicBall"), &Desc, 9);

    CProjectile_Boomarang::BOOMARANG_DESC BommarangDesc{};
    BommarangDesc.fDamage = 10.f;
    BommarangDesc.fSpeedPerSec = 10.f;
    BommarangDesc.fLifeTime = 6.f;
    BommarangDesc.fRotationPerSec = 180.f;
    BommarangDesc.pTarget = m_pTarget;

    m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Imp_Boomarang"),
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Imp_Boomarang"), &BommarangDesc, 5);

    return S_OK;
}

HRESULT CImp_Range::Ready_AnimEvent()
{
    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

#pragma region MagicBall

    pModel->Register_Event("CastSpell1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = true;
        Cast_MagicBall(0);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Cast (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 15.f);

        });


    pModel->Register_Event("CastSpell2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        Cast_MagicBall(1);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Cast (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 15.f);

        });


    pModel->Register_Event("CastSpell3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = true;
        Cast_MagicBall(2);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Cast (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 10.f);

        });

    pModel->Register_Event("ShotSpell1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = false;
        Shoot_MagicBall(0);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Shot (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 10.f);

        });


    pModel->Register_Event("ShotSpell2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = false;
        Shoot_MagicBall(1);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Shot (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 10.f);

        });


    pModel->Register_Event("ShotSpell3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = false;
        Shoot_MagicBall(2);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Shot (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 10.f);

        });


#pragma endregion

#pragma region Boomarang
    pModel->Register_Event("CastBoomarang", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        
        Cast_Boomarang();
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_Boomerang_Cast (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 15.f);

        });
    pModel->Register_Event("HoldBoomarang", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
       
            Hold_Boomarang();
        });
    pModel->Register_Event("ShootBoomarang", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        Shoot_Boomarang();
        if (nullptr == m_pTarget)
            Look_Target();
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_Boomerang_Shot (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 10.f);

        });

#pragma endregion

    return S_OK;
}

HRESULT CImp_Range::Ready_SFX()
{

    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

#pragma region MOVEMENT

    pModel->Register_Event("IR_SFX_Walk_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_Move(); });
    pModel->Register_Event("IR_SFX_Walk_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { SFX_Move(); });

#pragma endregion

#pragma region HIT

    pModel->Register_Event("IR_SFX_NormalHit_B", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_HIT(1); });
    pModel->Register_Event("IR_SFX_NormalHit_UF", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_HIT(1); });
    pModel->Register_Event("IR_SFX_NormalHit_L", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_HIT(1); });
    pModel->Register_Event("IR_SFX_NormalHit_R", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_HIT(1); });


    pModel->Register_Event("IR_SFX_StringlHit_U", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_HIT(2); });
    pModel->Register_Event("CA_M_DIWiz_DamageStrong_L_F", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_HIT(2); });
    pModel->Register_Event("IR_SFX_StringlHit_L", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_HIT(2); });
    pModel->Register_Event("IR_SFX_StringlHit_R", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_HIT(2); });



#pragma endregion

#pragma region DEAD
    pModel->Register_Event("IR_SFX_Dead", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_DEAD(); });
#pragma endregion

#pragma region REALIZE
    pModel->Register_Event("IM_SFX_Realize_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_REALIZE(); });
#pragma endregion


#pragma region SLEEP
    pModel->Register_Event("IR_SFX_Sleep_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {SFX_SLEEP();  });
    pModel->Register_Event("IR_SFX_Sleep_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { SFX_SLEEP(); });
    pModel->Register_Event("IR_SFX_Sleep_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_SLEEP(); });
    pModel->Register_Event("IR_SFX_Sleep_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { SFX_SLEEP(); });
#pragma endregion


    return S_OK;
}

void CImp_Range::SFX_Move()
{
    _uint iSoundIndex = m_pGameInstance->Rand(0, 3);

    if (iSoundIndex == 0)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpSword_Movement_S_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::MOVE)), 40.f);
    else if (iSoundIndex == 1)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpSword_Movement_S_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::MOVE)), 40.f);
    else if (iSoundIndex == 2)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpSword_Movement_S_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::MOVE)), 40.f);

}

void CImp_Range::SFX_HIT(_uint iHitIndex)
{
    _uint iSoundIndex = m_pGameInstance->Rand(0, 3);

    if (iHitIndex == 1)
    {
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Dmg_M_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::HITVO)), 8.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Dmg_M_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::HITVO)), 8.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Dmg_M_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::HITVO)), 8.f);
    }

    
    else if(iHitIndex == 2)
    {
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Dmg_L_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::HITVO)), 8.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Dmg_L_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::HITVO)), 8.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Dmg_L_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::HITVO)), 8.f);
    }


}

void CImp_Range::SFX_DEAD()
{
    _uint iSoundIndex = m_pGameInstance->Rand(0, 3);

    if (iSoundIndex == 0)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Die_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::DEAD)), 8.f);
    else if (iSoundIndex == 1)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Die_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::DEAD)), 8.f);
    else if (iSoundIndex == 2)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Die_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::DEAD)), 8.f);
    
}

void CImp_Range::SFX_REALIZE()
{
    _uint iSoundIndex = m_pGameInstance->Rand(0, 3);

    if (iSoundIndex == 0)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Breath_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::REALIZE)), 10.f);
    else if (iSoundIndex == 1)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Breath_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::REALIZE)), 10.f);
    else if (iSoundIndex == 2)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Breath_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::REALIZE)), 10.f);
}

void CImp_Range::SFX_SLEEP()
{
    _uint iSoundIndex = m_pGameInstance->Rand(0, 3);

    if (iSoundIndex == 0)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Laugh_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::REALIZE)), 10.f);
    else if (iSoundIndex == 1)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Laugh_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::REALIZE)), 10.f);
    else if (iSoundIndex == 2)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_DemonImp_Laugh_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::REALIZE)), 10.f);
}


void CImp_Range::Cast_MagicBall(_uint iIndex)
{
    m_isCastMagicBall = true;

    _float4x4 TempMatrix = m_pWeapon->Get_CombinedMatrix();
    _matrix matWorld = XMLoadFloat4x4(&TempMatrix);
    _vector vOffset = {};
    if (0 == iIndex)
        vOffset = XMVectorSet(1.f, 1.5f, 0.f, 0.f);
    else if (1 == iIndex)
        vOffset = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    else if (2 == iIndex)
        vOffset = XMVectorSet(-1.f,1.5f,0.f,0.f);

    _vector vTempSpawnPosition = matWorld.r[3] + vOffset;

    _float3 vSpawnPoint{};

    XMStoreFloat3(&vSpawnPoint, vTempSpawnPosition);

    CGameObject* pGameObject = m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Imp_MagicBall"));
    if (nullptr == pGameObject)
        return;

    m_MagicBalls[iIndex] = static_cast<CProjectile_Imp_MagicBall*>(pGameObject);
    if (m_MagicBalls[iIndex] == nullptr)
        return;

    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_MagicBalls[iIndex]->Set_SpawnDir(vNormalize);
    m_MagicBalls[iIndex]->Set_IsActive(false);
    m_MagicBalls[iIndex]->Set_Visible(true);
    m_MagicBalls[iIndex]->Set_SpanwPoint(vSpawnPoint);
    m_MagicBalls[iIndex]->Reset();

    m_pGameInstance->Push_PoolObject_ToLayer(
        ENUM_CLASS(LEVEL::HEINMACH),
        TEXT("Layer_Imp_MagicBall"),
        m_MagicBalls[iIndex]
    );

    if(iIndex == 0)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Cast_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 15.f);
    else if(iIndex == 1)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Cast_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 15.f);
    else if(iIndex == 2)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Cast_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 15.f);

}

void CImp_Range::Shoot_MagicBall(_uint iIndex)
{
    if (m_MagicBalls[iIndex] == nullptr)
        return;

    m_isCastMagicBall = false;

    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vTargetLoc = pTargetTransform->Get_State(STATE::POSITION);

    _float4x4 TempMatrix = m_pWeapon->Get_CombinedMatrix();
    _matrix matWorld = XMLoadFloat4x4(&TempMatrix);
    _vector vOffset = {};
    if (0 == iIndex)
        vOffset = XMVectorSet(1.f, 1.5f, 0.f, 0.f);
    else if (1 == iIndex)
        vOffset = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    else if (2 == iIndex)
        vOffset = XMVectorSet(-1.f, 1.5f, 0.f, 0.f);

    _vector vTempSpawnPosition = matWorld.r[3] + vOffset;

    _float3 vSpawnPoint{};

    XMStoreFloat3(&vSpawnPoint, vTempSpawnPosition);

    XMStoreFloat3(&vSpawnPoint, vTempSpawnPosition);
    _vector vDir = vTargetLoc - XMLoadFloat3(&vSpawnPoint);
    vDir = XMVector3Normalize(vDir);
    _float3 vSpawnDir{};
    XMStoreFloat3(&vSpawnDir, vDir);

    CProjectile_Imp_MagicBall* pMagicBall = m_MagicBalls[iIndex];
    if (pMagicBall == nullptr)
        return;

    pMagicBall->Set_SpanwPoint(vSpawnPoint);
    pMagicBall->Set_SpawnDir(vSpawnDir);
    pMagicBall->Reset();
    pMagicBall->Set_IsActive(true);
    pMagicBall->Fire_Projectile();

    if (iIndex == 0)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Shot_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 15.f);
    else if (iIndex == 1)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Shot_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 15.f);
    else if (iIndex == 2)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Shot_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 15.f);

}

void CImp_Range::Cast_Boomarang()
{
    m_isCastBoomarange = true;

    _float4 vTempSpawnPoint = *m_pBody->Get_BonePointEX("Weapon_L");
    CGameObject* pGameObject = m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Imp_Boomarang"));
    if (nullptr == pGameObject)
        return;

    _float3 vSpawnPoint = _float3(vTempSpawnPoint.x, vTempSpawnPoint.y, vTempSpawnPoint.z);

    m_pBoomarang = static_cast<CProjectile_Boomarang*>(pGameObject);
    if (m_pBoomarang == nullptr)
        return;

    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pBoomarang->Set_SpawnDir(vNormalize);
    m_pBoomarang->Set_IsActive(false);   
    m_pBoomarang->Set_Visible(true);
    m_pBoomarang->Set_SpanwPoint(vSpawnPoint);
    m_pBoomarang->Reset();

    m_pGameInstance->Push_PoolObject_ToLayer(
        ENUM_CLASS(LEVEL::HEINMACH),
        TEXT("Layer_Imp_Boomarang"),
        m_pBoomarang
    );

    m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_Boomerang_Obj_Cast (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 20.f);

}

void CImp_Range::Cast_Failed()
{

    if (m_isCastBoomarange && nullptr != m_pBoomarang)
    {
        m_pBoomarang->Set_IsActive(false);
        m_pBoomarang->Set_IsDead(true);
        m_pBoomarang->StopBoomarangSound();

    }
    else if(m_isCastMagicBall)
    {
        for (auto& pMagicBall : m_MagicBalls)
        {
            if (nullptr != pMagicBall)
            {
                pMagicBall->Set_IsDead(true);
                pMagicBall->Set_IsActive(false);
                pMagicBall->StopSound();

            }

        }
    }
  

    m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_Boomerang_Fail (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 1.5f);
}

void CImp_Range::Hold_Boomarang()
{
    if (nullptr == m_pBoomarang)
        return;
  
    _float4 vTempSpawnPoint = *m_pBody->Get_BonePointEX("Weapon_L");
    _float3 vSpawnPoint = _float3(vTempSpawnPoint.x, vTempSpawnPoint.y, vTempSpawnPoint.z);
    
    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pBoomarang->Set_SpawnDir(vNormalize);
    m_pBoomarang->Set_SpanwPoint(vSpawnPoint);
    m_pBoomarang->Reset();


}

void CImp_Range::Shoot_Boomarang()
{
    if (m_pBoomarang == nullptr)
        return;

    m_isCastBoomarange = false;

    CTransform* pTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vTargetLoc = pTransform->Get_State(STATE::POSITION);
    _float4 vTempSpawnPoint = *m_pBody->Get_BonePointEX("Weapon_L");
    _float3 vSpawnPoint = _float3(vTempSpawnPoint.x, vTempSpawnPoint.y, vTempSpawnPoint.z);
    vTargetLoc = XMVectorSetY(vTargetLoc, XMLoadFloat3(&vSpawnPoint).m128_f32[1]);
    _vector vDir = vTargetLoc - XMLoadFloat3(&vSpawnPoint);
    vDir = XMVector3Normalize(vDir);
    _float3 vSpawnDir{};
    XMStoreFloat3(&vSpawnDir, vDir);

    m_pBoomarang->Set_SpanwPoint(vSpawnPoint);
    m_pBoomarang->Set_SpawnDir(vSpawnDir);
    m_pBoomarang->Reset();
    m_pBoomarang->Set_IsActive(true);
    m_pBoomarang->Fire_Projectile();

    m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_Boomerang_Obj_Shot (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 20.f);
}

void CImp_Range::HPUI_Dead()
{
    m_pUI_HP->Update_Visible(false);
    m_pUI_HP->Set_IsDead(true);
}

void CImp_Range::Dissolve_On()
{
    m_isDissolve = true;
}

CImp_Range* CImp_Range::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImp_Range* pInstance = new CImp_Range(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CImp_Range"));
    }
    return pInstance;
}

CGameObject* CImp_Range::Clone(void* pArg)
{
    CImp_Range* pInstance = new CImp_Range(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CImp_Range"));
    }
    return pInstance;
}

void CImp_Range::Free()
{
    Safe_Release(m_pBody);
    Safe_Release(m_pWeapon);
    for (_uint i = 0; i < m_MagicBalls.size(); i++)
    {
        if (m_MagicBalls[i])
            m_MagicBalls[i]->Set_IsDead(true);
    }
       
    m_MagicBalls.clear();
    
    if (m_pBoomarang)
        m_pBoomarang->Set_IsDead(true);

    if (m_pUI_HP)
        m_pUI_HP->Set_IsDead(true);

    __super::Free();
}
