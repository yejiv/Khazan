#include "Viper.h"
#include "ClientInstance.h"
#include "GameInstance.h"
#include "AI_Controller_Viper.h"
#include "BlackBoard.h"
#include "CharacterVirtual.h"
#include "BossHp.h"
#include "Body_Viper.h"
#include "TwinBlade_Viper.h"


CViper::CViper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice, pContext }
{
}

CViper::CViper(const CViper& Prototype)
    :CMonster{ Prototype }
{
}

_float4* CViper::Get_LockOnPosition()
{
    return m_vLockOnPosition;
}

HRESULT CViper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CViper::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

     if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-32.365f, -29.5f, 198.409f, 1.f));
    m_pCharVirCom->Set_Position(XMVectorSet(-32.365f, -29.5f, 198.409f, 1.f));

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;


    m_pController = CAI_Controller_Viper::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    if (nullptr != m_pController)
    {
        m_pController->Get_BlackBoard()->Set_Value(m_strName, "Target", m_pTarget);
        //m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "Phase", ENUM_CLASS(PHASE::PHASE1));
    }

    m_ePhase = PHASE::PHASE1;

    m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Lacirma_ExpGained");

    m_fRecoveryPerSec = 5.f;

    return S_OK;
}

void CViper::Priority_Update(_float fTimeDelta)
{
  /*  CBlackBoard* pBB = m_pController->Get_BlackBoard();
    if (pBB->Get_Value<_bool>(m_strName, "isDetected"))
    {
        CBossHp::BOSSMON_UPDATE_DESC HPDesc{};
        HPDesc.isOpen = true;
        HPDesc.pHpMaxValue = &m_fMaxHP;
        HPDesc.pHpValue = &m_fCurrentHP;
        HPDesc.pStaminaMaxValue = &m_fMaxStamina;
        HPDesc.pStaminaCulValue = &m_fCurrentStamina;
        HPDesc.wstrName = TEXT("바이퍼");


        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &HPDesc);
    }*/

    CContainerObject::Priority_Update(fTimeDelta);
}

void CViper::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);

    if (m_fCurrentHP >= 0.f)
    {
        if (m_isLookAt)
        {
            CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
            if (nullptr == pModel)
                return;
            _float fRatio = pModel->MakeRatio();
            Look_Target_Lerp(fTimeDelta, fRatio, 8.f);
        }
    }

    __super::Update(fTimeDelta);

    m_vLockOnPosition = m_pBody->Get_BonePointEX("Bip001-Spine2");

}

void CViper::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    CContainerObject::Late_Update(fTimeDelta);
}

HRESULT CViper::Render()
{
    return S_OK;
}

void CViper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    //COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);

    //if (COLLISION_LAYER::PLAYER_ATTACK == eLayer)
    //{
    //    _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
    //    _vector vHitDir = XMLoadFloat3(&vContactPoint) - vPosition;

    //    _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
    //    _float fDot = XMVectorGetX(XMVector3Dot(vLook, vHitDir));
    //    _vector vUp = XMVector3Cross(vLook, vHitDir);
    //    _float vUpY = XMVectorGetY(vUp);
    //    DIRECTION_INFO DirInfo{};

    //    if (fDot >= 0.f)
    //    {
    //        DirInfo.Add_Flag(DirInfo.F);
    //    }
    //    else if (fDot < 0.f)
    //    {
    //        DirInfo.Add_Flag(DirInfo.B);
    //    }


    //    else if (vUpY >= 0.f)
    //    {
    //        DirInfo.Add_Flag(DirInfo.R);
    //    }
    //    else
    //    {
    //        DirInfo.Add_Flag(DirInfo.L);

    //    }

    //    // Decal Spawn
    //    _vector vDecalPos = m_pTransformCom->Get_State(STATE::POSITION);
    //    _float fOffset = 2.f;
    //    _float fPosX = XMVectorGetX(vDecalPos);
    //    _float fPosZ = XMVectorGetZ(vDecalPos);
    //    vDecalPos = XMVectorSetX(vDecalPos, m_pGameInstance->Rand(fPosX - fOffset, fPosX + fOffset));
    //    vDecalPos = XMVectorSetZ(vDecalPos, m_pGameInstance->Rand(fPosZ - fOffset, fPosZ + fOffset));
    //    DECAL_DESC Desc{};
    //    Desc.fLifeTime = 8.f;
    //    Desc.vFadeTime = _float2(0.2f, 0.2f);
    //    Desc.eType = static_cast<DECALTYPE>(m_pGameInstance->Rand(0.f, static_cast<_float>(DECALTYPE::END)));
    //    XMStoreFloat3(&Desc.vPosition, vDecalPos);
    //    Desc.vScale = _float3(
    //        m_pGameInstance->Rand(4.f, 8.f),
    //        2.f,
    //        m_pGameInstance->Rand(4.f, 8.f)
    //    );
    //    Desc.vColor = _float3(0.2745f, 0.08f, 0.08f);

    //    m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Decal"), Desc);
    //}
}

void CViper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CViper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}



HRESULT CViper::Ready_Components()
{
    //CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    //_float3 vPos{};
    //_float4 vQuat{};

    //XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    //XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    //tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    //tCharVirDesc.vPos = vPos;
    //tCharVirDesc.vQuat = vQuat;
    //tCharVirDesc.vShapeOffset = _float3(0.f, 4.1f, 0.f);
    //tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    //tCharVirDesc.fRadius = 2.f;
    //tCharVirDesc.fHeight = 4.f;
    //tCharVirDesc.fMaxSlopeAngle = 45.f;
    //tCharVirDesc.fMass = 10.f;
    //tCharVirDesc.fMaxStrength = 0.f;
    //tCharVirDesc.fPredictiveContactDistance = 0.3f;
    //tCharVirDesc.iMaxConstraintIterations = 20;
    //tCharVirDesc.fCollisionTolerance = 0.03f;
    //tCharVirDesc.fPenetrationRecoverySpeed = 1.7f;

    //m_tCollisionDesc.pGameObject = this;
    ////pCollDesc.pInfo = ?? // �ۼ��ϱ�
    //tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
    //    TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
    //    return E_FAIL;

    //m_pCharVirCom->Collision_Active(true);

    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.9f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 0.7f;
    tCharVirDesc.fHeight = 0.8f;
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

HRESULT CViper::Ready_PartObjects()
{
    CBody_Viper::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    BodyDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_PartObject_Body_Viper"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Viper*>(pBody);
    Safe_AddRef(m_pBody);


    CTwinBlade_Viper::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pOwner = this;
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pOwnerTransform = m_pTransformCom;
    WeaponDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bone_Wp");

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_PartObject_Weapon_TwinBlade"), &WeaponDesc)))
        return E_FAIL;
  
    CPartObject* pWeapon = Find_PartObject(TEXT("Part_Weapon"));
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pWeapon = dynamic_cast<CTwinBlade_Viper*>(pWeapon);
    Safe_AddRef(m_pWeapon);
    if (nullptr == pWeapon)
        return E_FAIL;


    return S_OK;
}

HRESULT CViper::Ready_Projectiles()
{
   
    return S_OK;
}



void CViper::Grab_Check_Begin()
{
  /*  CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    if (nullptr == pTargetTransform)
        return;
    _float4 vTemp = m_pWeapon->Get_GrabPos();
    _vector vGrabPosition = XMLoadFloat4(&vTemp);
    
    _vector vOffset = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    pTargetTransform->Set_State(STATE::POSITION, vGrabPosition + vOffset);*/

    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    if (nullptr == pTargetTransform)
        return;
    _matrix BoneWorld = m_pBody->Get_BoneMatrix("Bone_Wp_Hold");

    _vector vGrabPosition = BoneWorld.r[3];
    _vector vOffset = XMVectorSet(0.f, 0.f, 0.f, 0.f);
    pTargetTransform->Set_State(STATE::POSITION, vGrabPosition + vOffset);

}

void CViper::Grab_Check_End()
{
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    if (nullptr == pTargetTransform)
        return;

    _vector vGrabPosition = {};

    pTargetTransform->Set_State(STATE::POSITION, vGrabPosition);


}






HRESULT CViper::Ready_AnimEvent()
{

#pragma region WALK

    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;
    pModel->Register_Event("WalkStepEvent", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iStepCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName,"WalkStepCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName,"WalkStepCount", iStepCnt + 1);
        });

    pModel->Register_Event("WalkStepEvent", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            _uint iStepCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "WalkStepCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "WalkStepCount", iStepCnt + 1);
        });

#pragma endregion

#pragma region QUICK2HIT
   
    pModel->Register_Event("Quick2Hit_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("Quick2Hit_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });


    pModel->Register_Event("Quick2Hit_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("Quick2Hit_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });


#pragma endregion

#pragma region SLOW2HIT

    pModel->Register_Event("Slow2Hit_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("Slow2Hit_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });


    pModel->Register_Event("Slow2Hit_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("Slow2Hit_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });



#pragma endregion

#pragma region SLOW3HIT

    pModel->Register_Event("P1_Slow3Hit_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("P1_Slow3Hit_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });


    pModel->Register_Event("P1_Slow3Hit_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("P1_Slow3Hit_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("P1_Slow3Hit_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("P1_Slow3Hit_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });

#pragma endregion

#pragma region STINGSLASH

    pModel->Register_Event("P1_Sting_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision_R(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
        });

    pModel->Register_Event("P1_Sting_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });


    pModel->Register_Event("P1_Sting_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision_R(true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pModel->Register_Event("P1_Sting_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);


        });

    pModel->Register_Event("P1_StingSlash", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("P1_StingSlash", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });



#pragma endregion

#pragma region THROWBLADE

    pModel->Register_Event("ThrowBlade_Start", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("ThrowBlade_Start", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(false);

        });


    pModel->Register_Event("ThrowBlade_End", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("ThrowBlade_End", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {

            m_pWeapon->Set_OnAttackCollision(false);

        });



#pragma endregion

#pragma region TURNATTACk

    pModel->Register_Event("TrunAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("TrunAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });

#pragma endregion

#pragma region BACKJUMPSLASH

    pModel->Register_Event("BackJumpMovement", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "BackJump", true);
        });

    pModel->Register_Event("BackJumpMovement", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "BackJump", false);
        });


    pModel->Register_Event("BackJumpAttack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("BackJumpAttack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 콜라이더 끄기  
            m_pWeapon->Set_OnAttackCollision(false);

        });

#pragma endregion

#pragma region JUMPSMASH

    pModel->Register_Event("P1_JumpStart", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            //m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "P1_JumpStart", true);
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            m_pCharVirCom->Jump(70.f,10.f);
        });


    pModel->Register_Event("P1_JumpStop", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.3f, 0.5f, 2.f);

        });


    pModel->Register_Event("P1_Landing", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "P1_LandStart", true);
        });

    pModel->Register_Event("P1_Landing", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "P1_LandStart", false);
        });


#pragma endregion

#pragma region DEVOUR

    pModel->Register_Event("P1_SpinStart", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "P1_SpinStart", true);
        });

    pModel->Register_Event("P1_SpinStart", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "P1_SpinStart", false);
        });


#pragma endregion

#pragma region STINGGRAB

    pModel->Register_Event("StingGrab_Start", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision_L(true);
        });


    pModel->Register_Event("StingGrab_Start", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision_L(false);
        });

    pModel->Register_Event("StingGrab_Hold", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            Grab_Check_Begin();
        });

    pModel->Register_Event("StingGrab_Hold", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           // 플레이어 날리기
        });


#pragma endregion

#pragma region THROWMAINTAIN
    

    pModel->Register_Event("ThrowMainTain", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
           
        });


    pModel->Register_Event("ThrowMainTain", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            
        });


#pragma endregion
  
#pragma region 5HITCombo


    pModel->Register_Event("5Hit_Slash_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

            
        });

    pModel->Register_Event("5Hit_Slash_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("5Hit_Slash_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
        });

    pModel->Register_Event("5Hit_Slash_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);
        });

    pModel->Register_Event("5Hit_Slash_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
        });

    pModel->Register_Event("5Hit_Slash_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);
        });

    pModel->Register_Event("5Hit_Slash_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
        });

    pModel->Register_Event("5Hit_Slash_4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);
        });

    pModel->Register_Event("5Hit_Slash_5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
        });

    pModel->Register_Event("5Hit_Slash_5", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);
        });

 
#pragma endregion



    return S_OK;

}

CViper* CViper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CViper* pInstance = new CViper(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CViper"));
    }
    return pInstance;
}

CGameObject* CViper::Clone(void* pArg)
{
    CViper* pInstance = new CViper(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CViper"));
    }
    return pInstance;
}

void CViper::Free()
{
    Safe_Release(m_pBody);
    Safe_Release(m_pWeapon);
    __super::Free();
}
