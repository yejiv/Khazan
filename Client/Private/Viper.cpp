#include "Viper.h"
#include "ClientInstance.h"
#include "GameInstance.h"
#include "AI_Controller_Viper.h"
#include "BlackBoard.h"
#include "CharacterVirtual.h"
#include "BossHp.h"
#include "Body_Viper.h"
#include "TwinBlade_Viper.h"
#include "Body_Cinematic_Viper.h"
#include "Core_Viper.h"
#include "Body_Phase2_Viper.h"
#include "TwinBlade_R_Viper.h"
#include "AS_CutScene_Start_Viper.h"
#include "FSM_Viper.h"
#include "Projectile_Rock_Viper.h"
#include "AS_CutScene_2Phase_Viper.h"
#include "MeshTrail.h"
#include "LineTrail.h"

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

void CViper::Set_Phase(PHASE ePhase)
{
    m_ePhase = ePhase;
    if (m_ePhase == PHASE::PHASE2)
    {
        dynamic_cast<CFSM_Viper*>(dynamic_cast<CAI_Controller_Viper*>(m_pController)->Get_State_Machine())->Get_Phase2_CutScene_Start_Viper()->ViperScene_PullOut(this);
    }
}

void CViper::Set_Weapon_Phase1()
{
    m_pWeapon->Set_IsActive(true);
    m_pCore->Set_IsActive(false);
    m_pP2Weapon->Set_IsActive(false);
}

void CViper::Set_PhaseWeapon_Cinematic()
{
    m_pWeapon->Set_IsActive(false);
    m_pCore->Set_IsActive(true);
    m_pP2Weapon->Set_IsActive(false);

}

void CViper::Set_PhaseWeapon_Phase2()
{
    m_pWeapon->Set_IsActive(false);
    m_pCore->Set_IsActive(false);
    m_pP2Weapon->Set_IsActive(true);
}

void CViper::Set_WeaponOff()
{
    m_pWeapon->Set_IsActive(false);
    m_pCore->Set_IsActive(false);
    m_pP2Weapon->Set_IsActive(false);
}

void CViper::Viper_Land(_fvector vGoalPosition, _float fSpeed)
{
    m_pCharVirCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
    m_pCharVirCom->Start_Dive(vGoalPosition, fSpeed);
}

void CViper::Reset_Viper_Gravity()
{
    m_vGravity = XMVectorSet(0.f, g_fGravity, 0.f, 0.f);
}

CAS_CutScene_Start_Viper* CViper::Get_Viper_CutSceneState()
{
    CFSM_Viper* pFSM = static_cast<CFSM_Viper*>(m_pController->Get_State_Machine());
    CAS_CutScene_Start_Viper* pCutSceneState = pFSM->Get_CutScene_Start_Viper();
    return pCutSceneState;
}

CAS_CutScene_2Phase_Viper* CViper::Get_Phase2_Viper_CutSceneState()
{
    CFSM_Viper* pFSM = static_cast<CFSM_Viper*>(m_pController->Get_State_Machine());
    CAS_CutScene_2Phase_Viper* pCutSceneState = pFSM->Get_Phase2_CutScene_Start_Viper();
    return pCutSceneState;
}

CFSM_Viper* CViper::Get_Viper_FSM()
{
    CFSM_Viper* pFSM = static_cast<CFSM_Viper*>(m_pController->Get_State_Machine());
    return pFSM;
}

TRAIL_CONFIG CViper::Get_TrailConfig() const
{
    if (PHASE::PHASE1 == m_ePhase)
        return m_p1PhaseTrail[ENUM_CLASS(TWINBLADE::LEFT)]->Get_TrailConfig();
    else if (PHASE::PHASE2 == m_ePhase)
        return m_p2PhaseTrail[ENUM_CLASS(TWINBLADE_R::SWORD)]->Get_TrailConfig();

    return TRAIL_CONFIG();
}

void CViper::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    if (PHASE::PHASE1 == m_ePhase)
    {
        for (auto& pTrail : m_p1PhaseTrail)
            pTrail->Set_TrailConfig(Config);
    }
    else if (PHASE::PHASE2 == m_ePhase)
    {
        for (auto& pTrail : m_p2PhaseTrail)
            pTrail->Set_TrailConfig(Config);
    }
}

_uint CViper::Get_NumTrailTextures()
{
    _uint iNumTextrues = { 0 };

    if (PHASE::PHASE1 == m_ePhase)
        iNumTextrues = m_p1PhaseTrail[ENUM_CLASS(TWINBLADE::LEFT)]->Get_NumTrailTextures();
    else if (PHASE::PHASE2 == m_ePhase)
        iNumTextrues = m_p2PhaseTrail[ENUM_CLASS(TWINBLADE_R::SWORD)]->Get_NumTrailTextures();

    return iNumTextrues;
}

ID3D11ShaderResourceView* CViper::Get_TrailTexture(_uint iIndex)
{
    ID3D11ShaderResourceView* pSRV = { nullptr };

    if (PHASE::PHASE1 == m_ePhase)
        pSRV =  m_p1PhaseTrail[ENUM_CLASS(TWINBLADE::LEFT)]->Get_TrailTexture(iIndex);
    else if (PHASE::PHASE2 == m_ePhase)
        pSRV = m_p2PhaseTrail[ENUM_CLASS(TWINBLADE_R::SWORD)]->Get_TrailTexture(iIndex);

    return pSRV;
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

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;

    if (FAILED(Ready_AnimEffectEvent()))
        return E_FAIL;


    m_pController = CAI_Controller_Viper::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    if (nullptr != m_pController)
    {
        m_pController->Get_BlackBoard()->Set_Value(m_strName, "Target", m_pTarget);
    }

    m_ePhase = PHASE::PHASE1;
    //m_ePhase = PHASE::PHASE2;

    m_fRecoveryPerSec = 5.f;

    if (m_ePhase == PHASE::PHASE2)
    {
        //(-30.103f, -29.9f, 188.961f, 1.f)
        Set_PhaseWeapon_Phase2();
        m_pCharVirCom->Set_Position(XMVectorSet(-30.103f, -29.9f, 188.961f, 1.f));
    }

    CMeshTrail::TRAIL_DESC MeshDesc{};
    MeshDesc.iTextureIdx = 8;
    MeshDesc.fLifeTime = 0.3f;
    MeshDesc.iDivisionCount = 10.f;
    MeshDesc.vColor = _float4(2.353f, 1.961f, 1.569f, 1.f);
    MeshDesc.vSubColor = _float4(0.f, 0.f, 0.f, 7.843f);

    /*if (m_ePhase == PHASE::PHASE1)
    {*/
    for (_uint i = 0; i < ENUM_CLASS(TWINBLADE::END); ++i)
        m_p1PhaseTrail[i] = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));
    //}
    /*else if (m_ePhase == PHASE::PHASE2)
    {*/
    for (_uint i = 0; i < ENUM_CLASS(TWINBLADE_R::END); ++i)
        m_p2PhaseTrail[i] = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));
    //}

    CLineTrail::LINE_TRAIL_DESC LineDesc{};
    LineDesc.fOffset = 0.25f;
    LineDesc.fLifeTime = 1.f;
    LineDesc.iDivisionCount = 5.f;
    LineDesc.iTextureIdx = 6;
    LineDesc.vColor = _float4(2.353f, 1.961f, 1.569f, 1.f);
    for (_uint i = 0; i < ENUM_CLASS(EYE::END); ++i)
        m_pLineTrail[i] = dynamic_cast<CLineTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LineTrail"), &LineDesc));

    return S_OK;
}

void CViper::Priority_Update(_float fTimeDelta)
{
    CBlackBoard* pBB = m_pController->Get_BlackBoard();
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
    }

    CContainerObject::Priority_Update(fTimeDelta);

    if (PHASE::PHASE1 == m_ePhase)
    {
        for (auto& pTrail : m_p1PhaseTrail)
            pTrail->Priority_Update(fTimeDelta);
    }
    else if (PHASE::PHASE2 == m_ePhase)
    {
        for (auto& pTrail : m_p2PhaseTrail)
            pTrail->Priority_Update(fTimeDelta);
    }

    for (auto& pTrail : m_pLineTrail)
        pTrail->Priority_Update(fTimeDelta);
}

void CViper::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
    {
        m_isGhost = true;
    }

    m_pController->Update(this, fTimeDelta);

    if (m_fCurrentHP >= 0.f)
    {
        if (m_isLookAt)
        {
            if (PHASE::PHASE1 == m_ePhase)
            {
                CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
                if (nullptr == pModel)
                    return;
                _float fRatio = pModel->MakeRatio();
                Look_Target_Lerp(fTimeDelta, fRatio, m_fTurnSpeed);

            }
            else if (PHASE::PHASE2 == m_ePhase)
            {
                CModel* pModel = static_cast<CModel*>(m_pPahse2Body->Get_Component(TEXT("Com_Model")));
                if (nullptr == pModel)
                    return;
                _float fRatio = pModel->MakeRatio();
                Look_Target_Lerp(fTimeDelta, fRatio, m_fTurnSpeed);
            }
          
        }
    }

    if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta, INPUT_TYPE::GAMEPLAY))
    {

        if (m_pGameInstance->Key_Down(DIK_T))
        {
            m_ePhase = PHASE::PHASE1;
            Set_Weapon_Phase1();
            Get_Viper_CutSceneState()->Start_CutSceneAnimation();
        }

        if (m_pGameInstance->Key_Down(DIK_Y))
        {

        }

        if (m_pGameInstance->Key_Down(DIK_U))
        {
            m_ePhase = PHASE::CINEMATIC;
            Set_PhaseWeapon_Cinematic();
        }

        else if (m_pGameInstance->Key_Down(DIK_I))
        {
            m_ePhase = PHASE::PHASE2;
            Set_PhaseWeapon_Phase2();

        }
    }

    if (m_pGameInstance->Key_Down(DIK_Z))
   {
       m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-30.103f, -29.9f, 188.961f, 1.f));
       m_pCharVirCom->Set_Position(XMVectorSet(-30.103f, -29.9f, 188.961f, 1.f));
   }

    __super::Update(fTimeDelta);

    if(m_ePhase == PHASE::PHASE1)
        m_vLockOnPosition = m_pBody->Get_BonePointEX("Bip001-Spine2");
    else if(m_ePhase == PHASE::PHASE2)
        m_vLockOnPosition = m_pPahse2Body->Get_BonePointEX("Bip001-Spine2");

    if (PHASE::PHASE1 == m_ePhase)
    {
        for (auto& pTrail : m_p1PhaseTrail)
            pTrail->Update(fTimeDelta);
    }
    else if (PHASE::PHASE2 == m_ePhase)
    {
        for (auto& pTrail : m_p2PhaseTrail)
            pTrail->Update(fTimeDelta);
    }

    for (auto& pTrail : m_pLineTrail)
        pTrail->Update(fTimeDelta);

    // Test
    //  FX_1PhaseTrail();
    //  FX_2PhaseHandTrail();
    //  FX_2PhaseSwordTrail();
    FX_2PhaseEyeTrail();

   if (m_pGameInstance->Key_Down(DIK_P))
    {
        _vector vPos = m_pP2Weapon->Get_BladeTipPos();
        tmpIdx = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::VIPER), TEXT("Grap"), vPos);
    }
    if (m_pGameInstance->Key_Down(DIK_O))
    {
        m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::VIPER), TEXT("Grap"), tmpIdx);
    }
}

void CViper::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    CContainerObject::Late_Update(fTimeDelta);

    if (PHASE::PHASE1 == m_ePhase)
    {
        for (auto& pTrail : m_p1PhaseTrail)
            pTrail->Late_Update(fTimeDelta);
    }
    else if (PHASE::PHASE2 == m_ePhase)
    {
        for (auto& pTrail : m_p2PhaseTrail)
            pTrail->Late_Update(fTimeDelta);
    }

    for (auto& pTrail : m_pLineTrail)
        pTrail->Late_Update(fTimeDelta);
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
    //    Desc.iRandomTexture = true;      
    //
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

    m_tViperCollisionDesc.pGameObject = this;
    m_tViperCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    m_tViperCollisionDesc.strName = TEXT("Viper_Collider");
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tViperCollisionDesc;

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


    CBody_Cinematic_Viper::BODY_DESC CinematicBodyDesc{};
    CinematicBodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    CinematicBodyDesc.pOwnerTransform = m_pTransformCom;
    CinematicBodyDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body_Cinematic"), ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_PartObject_Body_Cinematic_Viper"), &CinematicBodyDesc)))
        return E_FAIL;

    CPartObject* pCinematicBody = Find_PartObject(TEXT("Part_Body_Cinematic"));
    if (nullptr == pCinematicBody)
        return E_FAIL;

    m_pCinematicBody = dynamic_cast<CBody_Cinematic_Viper*>(pCinematicBody);
    Safe_AddRef(m_pCinematicBody);


    CCore_Viper::WEAPON_DESC CoreDesc{};
    CoreDesc.pOwner = this;
    CoreDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    CoreDesc.pOwnerTransform = m_pTransformCom;
    CoreDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Bone_Wp");

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Core"), ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_PartObject_Weapon_Core"), &CoreDesc)))
        return E_FAIL;

    CPartObject* pCore = Find_PartObject(TEXT("Part_Core"));
    if (nullptr == pCore)
        return E_FAIL;

    m_pCore = dynamic_cast<CCore_Viper*>(pCore);
    Safe_AddRef(m_pCore);
    if (nullptr == pCore)
        return E_FAIL;


    CBody_Phase2_Viper::BODY_DESC Phase2BodyDesc{};
    Phase2BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Phase2BodyDesc.pOwnerTransform = m_pTransformCom;
    Phase2BodyDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body_Phase2"), ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_PartObject_Body_Phase2_Viper"), &Phase2BodyDesc)))
        return E_FAIL;

    CPartObject* pPhase2Body = Find_PartObject(TEXT("Part_Body_Phase2"));
    if (nullptr == pPhase2Body)
        return E_FAIL;

    m_pPahse2Body = dynamic_cast<CBody_Phase2_Viper*>(pPhase2Body);
    Safe_AddRef(m_pPahse2Body);




    CTwinBlade_R_Viper::WEAPON_DESC P2WeaponDesc{};
    P2WeaponDesc.pOwner = this;
    P2WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    P2WeaponDesc.pOwnerTransform = m_pTransformCom;
    P2WeaponDesc.pSocketMatrix = m_pPahse2Body->Get_BoneMatrix_Ptr("Bone_Wp");
    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_P2Weapon"), ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_PartObject_Weapon_TwinBlade_R"), &P2WeaponDesc)))
        return E_FAIL;
    CPartObject* pP2Weapon = Find_PartObject(TEXT("Part_P2Weapon"));
    if (nullptr == pP2Weapon)
        return E_FAIL;
    m_pP2Weapon = dynamic_cast<CTwinBlade_R_Viper*>(pP2Weapon);
    Safe_AddRef(m_pP2Weapon);
    if (nullptr == pP2Weapon)
        return E_FAIL;


    return S_OK;
}

HRESULT CViper::Ready_Projectiles()
{
    CProjectile_Rock_Viper::PROJECTILE_DESC Desc{};
    Desc.fDamage = 20.f;
    Desc.fSpeedPerSec = 50.f;
    Desc.fLifeTime = 3.f;
    Desc.fRotationPerSec = 180.f;
    m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Projectile_Viper_Rock"),
        ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_Rock"), &Desc, 5);

   
    m_pThrowMatrix = m_pPahse2Body->Get_BoneMatrix_Ptr("Bip001-L-Hand");
    
    
    return S_OK;
}



void CViper::Grab_Check_Begin()
{
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

void CViper::Set_ViperPosition(_fvector vPosition)
{
    m_pTransformCom->Set_State(STATE::POSITION, vPosition);
    m_pCharVirCom->Set_Position(vPosition);
    m_pCharVirCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
}

void CViper::Set_Teleport(_fvector vPos)
{
    m_pCharVirCom->Teleport(vPos, m_pTransformCom->Get_Rotation_Quat(), m_pTransformCom);
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
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            m_pCharVirCom->Jump(50.f,7.f);
        });


    pModel->Register_Event("P1_JumpStop", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 1.f, 0.1f, 0.25f);
        });


    pModel->Register_Event("P1_Landing", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            m_pCharVirCom->Start_Dive(vTargetPos,80.f);

            m_pWeapon->Set_OnAttackCollision(true);


        });


    pModel->Register_Event("P1_Landing", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(false);

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
            m_fTurnSpeed = 10.f;
        });


    pModel->Register_Event("StingGrab_Start", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision_L(false);
            m_fTurnSpeed = 8.f;
        });

    pModel->Register_Event("StingGrab_Hold", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            m_isLookAt = false;
            Grab_Check_Begin();
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

#pragma region DOWN
    pModel->Register_Event("CutScene_Down_Conceal_W", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            Set_WeaponOff();
        });
#pragma endregion


#pragma region LOOKING_CORE

    //pModel->Register_Event("Looking_Core", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
    //    {
    //        CTransform* pCoreTransform = static_cast<CTransform*>(m_pCore->Get_Component(TEXT("Com_Transform")));
    //        m_pCore->Set_IsActive(true);
    //        pCoreTransform->Rotation(XMConvertToRadians(-90.f), XMConvertToRadians(180.f), XMConvertToRadians(-90.f));
    //    });

    pModel->Register_Event("Remove_Core", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pCore->Set_IsActive(false);
        });


#pragma endregion

#pragma region START_CINEMATIC_JUMP


    pModel->Register_Event("Viper_StartCutScene_Jump_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            
             m_pCharVirCom->Set_Gravity(0.f);
             m_pCharVirCom->Set_Velocity(XMVectorSet(0.f,0.f,0.f,0.f));
             m_vGravity = XMVectorSet(0.f, 0.f, 0.f, 0.f);

             //m_vGravity = XMVectorSet(0.f, g_fGravity, 0.f, 0.f);
                //m_pCharVirCom->Start_Dive(vPosition,50.f);

        });


#pragma endregion

#pragma region START_CINNEMATIC_LANDING

    pModel->Register_Event("ViperStartCutScene_Jump", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            
            _vector vPosition = XMVectorSet(-31.938f, -29.986f, 190.162f, 1.f);
            m_pCharVirCom->Jump_ToTarget(vPosition,10.f,80.f);
        });

    pModel->Register_Event("ViperStartCutScene_Jump", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            _vector vPosition = XMVectorSet(-31.938f, -29.986f, 210.162f, 1.f);
            m_pCharVirCom->Start_Dive(vPosition, 20);
        });


#pragma endregion



    CModel* pP2Model = static_cast<CModel*>(m_pPahse2Body->Get_Component(TEXT("Com_Model")));
    if (nullptr == pP2Model)
        return E_FAIL;



    //=============================================================PHASE2======================================================//


#pragma region HANDSTOMP

    pP2Model->Register_Event("HandStomp_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("HandStomp_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            
        });


    pP2Model->Register_Event("HandStomp_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);
        });

    pP2Model->Register_Event("HandStomp_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 바디 오른손 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pPahse2Body->Set_OnAttackCollision(false);

        });
#pragma endregion

#pragma region HANDSTOMPSTR
    pP2Model->Register_Event("HandStompStr_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("HandStompStr_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           
        });


    pP2Model->Register_Event("HandStompStr_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("HandStompStr_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 바디 오른손 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pPahse2Body->Set_OnAttackCollision(false);

        });
#pragma endregion

#pragma region HANDSWING2HIT
    pP2Model->Register_Event("HandSwing2Hit_Look1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;

        });

    pP2Model->Register_Event("HandSwing2Hit_Look1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
        });


    pP2Model->Register_Event("HandSwing2Hit_Look2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;

        });

    pP2Model->Register_Event("HandSwing2Hit_Look2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           
        });

    pP2Model->Register_Event("HandSwing2Hit_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("HandSwing2Hit_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 바디 오른손 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pPahse2Body->Set_OnAttackCollision(false);
        });

    pP2Model->Register_Event("HandSwing2Hit_Look3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("HandSwing2Hit_Look3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           
        });
    pP2Model->Register_Event("HandSwing2Hit_Attack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("HandSwing2Hit_Attack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 바디 오른손 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pPahse2Body->Set_OnAttackCollision(false);

        });


#pragma endregion

#pragma region HANDUPPER
    pP2Model->Register_Event("HandUpperLook1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("HandUpperLook1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           
        });


    pP2Model->Register_Event("HandUpperAttack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);
        });

    pP2Model->Register_Event("HandUpperAttack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 바디 오른손 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pPahse2Body->Set_OnAttackCollision(false);

        });
#pragma endregion

#pragma region DASHUPPER
    pP2Model->Register_Event("DashUpperLook", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("DashUpperLook", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            
        });


    pP2Model->Register_Event("DashUpperAttack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("DashUpperAttack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 바디 오른손 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pPahse2Body->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("DashUpperLook2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("DashUpperLook2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            
        });


    pP2Model->Register_Event("DashUpperAttack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("DashUpperAttack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 바디 오른손 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pPahse2Body->Set_OnAttackCollision(false);

        });


#pragma endregion

#pragma region DASHUPPERSTR
    pP2Model->Register_Event("DashUpperStrLook", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("DashUpperStrLook", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           
        });


    pP2Model->Register_Event("DashUpperStrAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("DashUpperStrAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 바디 오른손 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pPahse2Body->Set_OnAttackCollision(false);


        });
#pragma endregion

#pragma region FAKERUNATTACK
    pP2Model->Register_Event("FakeRunAttackLook", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
            m_isGhost = true;
        });

    pP2Model->Register_Event("FakeRunAttackLook", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            
            m_isGhost = false;
        });


    pP2Model->Register_Event("FakeRunAttackAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pP2Model->Register_Event("FakeRunAttackAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_pP2Weapon->Set_OnAttackCollision(false);

           
        });

    pP2Model->Register_Event("FakeRunAttackAttack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pP2Model->Register_Event("FakeRunAttackAttack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pP2Weapon->Set_OnAttackCollision(false);

        });



#pragma endregion

#pragma region SLASHDOUBLE
    pP2Model->Register_Event("SlashDoubleLook1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("SlashDoubleLook1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            
        });


    pP2Model->Register_Event("SlashDoubleAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("SlashDoubleAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pP2Weapon->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("SlashDoubleLook2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("SlashDoubleLook2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           
        });


    pP2Model->Register_Event("SlashDoubleAttack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("SlashDoubleAttack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pP2Weapon->Set_OnAttackCollision(false);

        });



#pragma endregion

#pragma region SLASHSTOMP
    pP2Model->Register_Event("SlashStompLook1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("SlashStompLook1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           
        });


    pP2Model->Register_Event("SlashStompAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pP2Model->Register_Event("SlashStompAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pP2Weapon->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("SlashStompLook2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("SlashStompLook2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
           
        });


    pP2Model->Register_Event("SlashStompAttack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pP2Model->Register_Event("SlashStompAttack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pP2Weapon->Set_OnAttackCollision(false);

        });


    pP2Model->Register_Event("SlashStompLook3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("SlashStompLook3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
        });


    pP2Model->Register_Event("SlashStompAttack3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pP2Model->Register_Event("SlashStompAttack3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pP2Weapon->Set_OnAttackCollision(false);

        });



#pragma endregion


#pragma region BACKJUMP_1

    pP2Model->Register_Event("BackJump_1Attack_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 20.f;
        });

    pP2Model->Register_Event("BackJump_1Attack_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
        });


    pP2Model->Register_Event("BackJump_1Attack_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("BackJump_1Attack_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(false);

        });

#pragma endregion

#pragma region BACKJUMP_2

    pP2Model->Register_Event("BackJump_2Attack_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 20.f;
        });

    pP2Model->Register_Event("BackJump_2Attack_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
        });


    pP2Model->Register_Event("BackJump_2Attack_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("BackJump_2Attack_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 ON
            m_pP2Weapon->Set_OnAttackCollision(false);


        });

#pragma endregion

#pragma region JUMPATTACK

    pP2Model->Register_Event("P2_JumpAttack_Start", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            m_pCharVirCom->Jump(50.f, 7.f);
            m_isLookAt = true;
            m_fTurnSpeed = 30.f;
        });

    pP2Model->Register_Event("P2_JumpAttack_End", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            // 타겟 위치 + 타겟 룩 * offset
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            _vector vTargetLook = pTargetTransform->Get_State(STATE::LOOK);
            vTargetLook = XMVector3Normalize(vTargetLook);
            _float fOffset = 30.f;
            _vector vLandPos = vTargetPos + vTargetLook * fOffset;
            m_pCharVirCom->Start_Dive(vLandPos, 80.f);

        });


    pP2Model->Register_Event("P2_JumpAttack_End", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;

            m_pCharVirCom->Start_Dive(vTargetPos, 80.f);

        });



    pP2Model->Register_Event("P2_JumpAttack_Attack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

            // 왼손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);

            
        });

    pP2Model->Register_Event("P2_JumpAttack_Attack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);


            // 왼손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("P2_JumpAttack_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("P2_JumpAttack_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {


        });

    pP2Model->Register_Event("P2_JumpAttack_Attack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

           // 오른손 무기 콜라이더ON
            m_pP2Weapon->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("P2_JumpAttack_Attack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

            // 오른손 무기 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pP2Weapon->Set_OnAttackCollision(false);

        });



#pragma endregion

#pragma region THROWROCK

    pP2Model->Register_Event("P2_ThrowRock_Start", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            Pick_ViperRock();
            m_isLookAt = true;
            m_fTurnSpeed = 10.f;
        });

    pP2Model->Register_Event("P2_ThrowRock_Loop", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            Throw_ViperRock();
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;

        });

    pP2Model->Register_Event("P2_ThrowRock_Loop", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            Hold_ViperRock();
        });


#pragma endregion

#pragma region P2_SIDEJUMP_L
    pP2Model->Register_Event("P2_SideJumpL_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this,pP2Model]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 30.f;

        });

    pP2Model->Register_Event("P2_SideJumpL_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;

        });


#pragma endregion

#pragma region P2_SIDEJUMP_R
    pP2Model->Register_Event("P2_SideJumpR_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 30.f;
        });


    pP2Model->Register_Event("P2_SideJumpR_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;

        });



#pragma endregion


#pragma region P2_VIPER_DASH_DRIFT

    pP2Model->Register_Event("DashDrift_Pause", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            m_isGhost = true;
            m_isLookAt = true;
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 1.f, 0.1f, 0.25f);
            m_pPahse2Body->Set_OnAttackCollision(true);
        });


    pP2Model->Register_Event("DashDrift_Pause", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            m_isLookAt = false;
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName,"isP2_Dash_Abort", true);
            m_pPahse2Body->Set_OnAttackCollision(true);

        });



#pragma endregion



#pragma region P2_SWINGCOMBO
    pP2Model->Register_Event("P2_SideJumpR_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 12.f;

            
        });
    pP2Model->Register_Event("SwingCombo_Attack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_ComboMove", true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
            m_pP2Weapon->Set_OnAttackCollision(true);


        });
    pP2Model->Register_Event("SwingCombo_Attack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;

            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_ComboMove", false);
            m_pP2Weapon->Set_OnAttackCollision(false);



        });
    pP2Model->Register_Event("SwingCombo_Look2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 12.f;
        });
    pP2Model->Register_Event("SwingCombo_Attack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_ComboMove", true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
            m_pPahse2Body->Set_OnAttackCollision(true);


        });
    pP2Model->Register_Event("SwingCombo_Attack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_ComboMove", false);
            m_pPahse2Body->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("SwingCombo_Attack3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {

            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_ComboMove", true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
            m_pPahse2Body->Set_OnAttackCollision(true);


        });
    pP2Model->Register_Event("SwingCombo_Attack3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;

            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_ComboMove", false);
            m_pPahse2Body->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("SwingCombo_Look4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 12.f;

        });
    pP2Model->Register_Event("SwingCombo_Attack4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_ComboMove", true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

            m_pPahse2Body->Set_OnAttackCollision(true);

        });
    pP2Model->Register_Event("SwingCombo_Attack4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_isGhost = true;
            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_ComboMove", false);
            m_pPahse2Body->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("SwingCombo_Ghost", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            m_isGhost = true;
        });

    pP2Model->Register_Event("SwingCombo_Condition", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            _bool isBerserker = pBB->Get_Value<_bool>(m_strName, "is_Berserker");
            if (!isBerserker)
            {
                pBB->Set_Value(m_strName, "isP2_Combo_Abort", true);
                m_isGhost = false;
            }
        });

    pP2Model->Register_Event("SwingCombo_Rush", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_Rush", true);
            m_isGhost = true;
            CTransform* pTargetTransform = m_pTarget->Get_Transform();
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            m_pTransformCom->LookAt(vTargetPos);
            m_pTransformCom->Set_SpeedPerSec(0.65f);
            
        });

    pP2Model->Register_Event("SwingCombo_Rush", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            CBlackBoard* pBB = m_pController->Get_BlackBoard();
            pBB->Set_Value(m_strName, "is_P2_Rush", false);
            m_pTransformCom->Set_SpeedPerSec(3.f);
            m_pPahse2Body->Set_OnAttackCollision(true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });



    pP2Model->Register_Event("SwingCombo_Ghost2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            m_isGhost = false;
            m_pPahse2Body->Set_OnAttackCollision(false);

        });


#pragma endregion



    return S_OK;

}

HRESULT CViper::Ready_AnimEffectEvent()
{
    // ======================================== 1 Phase ========================================

    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

    pModel->Register_Event("Roar_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Roar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), m_iRotFX_Idx);
        });

    pModel->Register_Event("Devour_Start_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Rot_Start"), m_pTransformCom->Get_State(STATE::POSITION));        
        });

    pModel->Register_Event("Devour_Start_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Rot_Start"), m_iRotFX_Idx);
        });

    pModel->Register_Event("Devour_End_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Rot_Loop"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Devour_End_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Rot_Loop"), m_iRotFX_Idx);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Rot_End"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Grab_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _float4x4 combinedMatrix = m_pWeapon->Get_CombinedMatrix();
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&combinedMatrix));
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Grap"), rot, m_pWeapon->Get_LeftSwordTip());
        });
     
    pModel->Register_Event("Grab_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _float4x4 combinedMatrix = m_pWeapon->Get_CombinedMatrix();
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&combinedMatrix));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Grap"), m_iRotFX_Idx, rot, m_pWeapon->Get_LeftSwordTip());
        });

    pModel->Register_Event("Grab_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Grap"), m_iRotFX_Idx); 
        });

    //cutscene - 1p
        // m_pPahse2Body->Get_BoneMatrix_Ptr("Bip001-L-Hand");

    pModel->Register_Event("Start_Hurt_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //위치 왼쪽 팔로 변경 - 임시로 왼손에 해놓음
        m_iBloodFX_Idx[2] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        });

    pModel->Register_Event("Start_Hurt_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        //위치 왼쪽 팔로 변경 - 임시로 왼손에 해놓음
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[2], m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        });

    pModel->Register_Event("Start_Hurt2_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        //위치 왼쪽 팔로 변경 - 임시로 왼손에 해놓음
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[2], m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        });

    pModel->Register_Event("Start_Hurt2_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        //위치 왼쪽 팔로 변경 - 임시로 왼손에 해놓음
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[2]);
        });

    pModel->Register_Event("Breath0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath5_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath6_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath7_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath8_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath9_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath10_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath11_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath12_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath13_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath14_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("GrabSword_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //칼 위치로 갱신 필요"Bip001-L-Hand"
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire3"), m_pBody->Get_BoneMatrix("Bip001-R-Hand").r[3]);
        //m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0]);
        });

    pModel->Register_Event("GSFire0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        //_float4x4 mat = m_pCore->Get_CombinedMatrix();
        //_vector rot = Decompose_Rotation(XMLoadFloat4x4(&mat));
        //m_iFireFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), rot, XMLoadFloat4x4(&mat).r[3]);
        m_iFireFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), m_pCore->Get_CoreCenter());
        });
    pModel->Register_Event("GSFire0_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _float4x4 mat = m_pCore->Get_CombinedMatrix();
        //_vector rot = Decompose_Rotation(XMLoadFloat4x4(&mat));
        //m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), m_iFireFX_Idx, rot, XMLoadFloat4x4(&mat).r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), m_iFireFX_Idx, m_pCore->Get_CoreCenter());
        });
    pModel->Register_Event("GSFire1_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _float4x4 mat = m_pCore->Get_CombinedMatrix();
        //_vector rot = Decompose_Rotation(XMLoadFloat4x4(&mat));
        //m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), m_iFireFX_Idx, rot, XMLoadFloat4x4(&mat).r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), m_iFireFX_Idx, m_pCore->Get_CoreCenter());
        });
    pModel->Register_Event("GSFire1_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { 
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), m_iFireFX_Idx);
        });

    pModel->Register_Event("CutsceneTrail_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        //칼 위치로 갱신 필요"Bip001-L-Hand"
        // 
        //m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_scream_cutscene"), m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        // 
        //m_iBloodFX_Idx[0] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pBody->Get_BoneMatrix("Bip001-L-Hand").r[3]);
        //여기서 파티클들 휘몰아쳐야됨

        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle_Blust"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Sting_BloodLoop_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iBloodFX_Idx[0] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        m_iBodyParticleFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Sting_BloodLoop_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0], m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx, m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Sting_BloodLoop2_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0], m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx, m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Sting_BloodLoop2_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0]);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx);
        });


    pModel->Register_Event("BloodMouth0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("BloodMouth1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("BloodMouth2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("BloodMouth3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("BloodMouth4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("BloodMouth5_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle_Blust"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("HurtRoar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle_Blust"), m_pTransformCom->Get_State(STATE::POSITION));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("mist4"), m_pBody->Get_BoneMatrix("Bip001-L-Hand").r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_CutSceen_Land"), m_pBody->Get_BoneMatrix("Bip001-L-Hand").r[3]);
        });

    /////=========================================================== [ 2P ] ================================================================================ ////


    // MeshTrail
    pModel->Register_Event("Quick2Hit_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("Slow2Hit_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("Slow3Hit_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("SlashBackJump_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("ThrowBlade_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("TurnAttack_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("JumpSmash_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("Divour_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("Combo5Hit_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("StingGrabStart_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("StingGrabSuccess_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("StingGrabFail_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("StingSlashLoop01_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("StingSlashLoop02_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });
    pModel->Register_Event("StingSlashEnd_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_1PhaseTrail(); });

    // ======================================== 2 Phase ========================================
    
    pModel = static_cast<CModel*>(m_pPahse2Body->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

    pModel->Register_Event("Hand_Swing_Double_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrailLR"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("Hand_Swing_Double_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrailRL"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Hand_Stomp_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        });
    
    pModel->Register_Event("Hand_Stomp_Strong_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land_Big"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        });
    
    pModel->Register_Event("Sword_Swing_Double_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailRL"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Sword_Swing_Double_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailLR"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Hand_Upper_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrail_Up"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Hand_DashUpper_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrail_Up"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Hand_DashUpper_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land_Big"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        });
    
    pModel->Register_Event("Hand_DashUpper_STR_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrail_Up"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Back_Step_Trail_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailLR"), rot,  m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Sword_Swing_Stamp_Trail_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailRL"), rot, m_pTransformCom->Get_State(STATE::POSITION));
    
        });
    
    pModel->Register_Event("Sword_Swing_Stamp_Trail_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailLR"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Sword_Swing_Stamp_Fin_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land"), 칼 위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land_Big"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    //pModel->Register_Event("FakeRun_Trail0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailRL"), rot, m_pTransformCom->Get_State(STATE::POSITION));
    //    });
    //
    //pModel->Register_Event("FakeRun_Trail1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailLR"), rot, m_pTransformCom->Get_State(STATE::POSITION));
    //    });
    
    pModel->Register_Event("SwingCombo_Trail0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailRL"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("SwingCombo_Trail1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailLR"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("SwingRound_Rotate_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Rot_Start"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("SwingRound_Rotate_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Rot_Start"), m_iRotFX_Idx);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Rot_End"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    //pModel->Register_Event("SwingRound_Rotate_Point_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    });
    
    pModel->Register_Event("SwingRound_Land_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
          //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land"), 창 위치);
          m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("JumpAtk_Roar_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("JumpAtk_Roar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), m_iRotFX_Idx);
        });
    
    pModel->Register_Event("JumpAtk_Jump_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
          m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land_Big"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        });
    
    pModel->Register_Event("JumpAtk_Stamp_RightHand_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SphereTrail_V"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("JumpAtk_Stamp_RightHand_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        });
    
    pModel->Register_Event("Roar_2Phase_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream_hight"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Roar_2Phase_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream_hight"), m_iRotFX_Idx);
        });
    
    pModel->Register_Event("DashDrift_Roar_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("DashDrift_Roar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), m_iRotFX_Idx);
        });
    
    pModel->Register_Event("Swing_Combo_Roar_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Swing_Combo_Roar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), m_iRotFX_Idx);
        });
    
    pModel->Register_Event("Swing_Combo_SpearTrail_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearTrailRL"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Swing_Combo_HandTrail_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrailLR"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Swing_Combo_HandTrail_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrailRL"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Swing_Combo_HandTrail2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrailLR"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Swing_Combo_HandTrail2_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("HandTrailLR"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("CutSceenFoot_R0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_R1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_R2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_R3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_R4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_R5_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_R6_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_R7_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_R8_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("CutSceenFoot_L0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_L1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_L2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_L3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_L4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_L5_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_L6_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("CutSceenFoot_L7_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("GrabSwrod_Walk_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 칼위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Soward_Particle_red"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Hand").r[3]);
        });

    pModel->Register_Event("Breath15_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pPahse2Body->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath16_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pPahse2Body->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath17_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pPahse2Body->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath18_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pPahse2Body->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath19_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pPahse2Body->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath20_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pPahse2Body->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath21_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pPahse2Body->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Breath22_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pPahse2Body->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Cutscene_LastRoar_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Cutscene_LastRoar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), m_iRotFX_Idx);
        });


    pModel = static_cast<CModel*>(m_pCinematicBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

    pModel->Register_Event("StartEnvi_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //휘몰아치는 눈 루프 아닌거 -> 왜 없어짐?
        //아래 위치 모두 맵 중심으로 바꿔야함!
        //휘몰아치는 눈 계속 켜는 거
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_snow2"), m_pTransformCom->Get_State(STATE::POSITION)); 
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_ChangeSnow"), m_pTransformCom->Get_State(STATE::POSITION)); 
        //mist1번 (단발)
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("mist1"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Start_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 뭐 몸에서 파티클 더 나오거나 하기 
        //소용돌이
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Tornado"), m_pTransformCom->Get_State(STATE::POSITION));
                //여기 팔 몇몇군데에서 피 루프 시작
        m_iBloodFX_Idx[0] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pCinematicBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        m_iBloodFX_Idx[1]= m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist001").r[3]);
        m_iBloodFX_Idx[2] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist").r[3]);
        });

    pModel->Register_Event("Start_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0], m_pCinematicBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[1], m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist001").r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[2], m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist").r[3]);

        });
    
    pModel->Register_Event("Start_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Tornado"), m_iRotFX_Idx);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0]);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[1]);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[2]); 
                //위의 팔 피 루프 끝내기
        });

    pModel->Register_Event("Hurt_Arm0_S_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_once"), m_pCinematicBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        });

    pModel->Register_Event("Hurt_Arm1_E_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_once"), m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist001").r[3]);
        });

    pModel->Register_Event("Hurt_Arm3_B_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_once"), m_pCinematicBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        });

    pModel->Register_Event("Hurt_Arm2_S_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_once"), m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist").r[3]);
        });
    
    pModel->Register_Event("Hurt_Arm4_S_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_once"), m_pCinematicBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        });
    
    pModel->Register_Event("Hurt_Arm5_E_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_once"), m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist001").r[3]);
        });
    pModel->Register_Event("Hurt_Arm6_E_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_once"), m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist").r[3]);
        });
    // MeshTrail
    pModel->Register_Event("MeshTrail_Hand0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand4", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand5", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand6", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand7", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand8", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand9", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand10", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    //  pModel->Register_Event("MeshTrail_Hand11", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    //  pModel->Register_Event("MeshTrail_Hand12", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    //  pModel->Register_Event("MeshTrail_Hand13", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    //  pModel->Register_Event("MeshTrail_Hand14", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand15", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand16", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand17", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand18", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand19", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand20", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });
    pModel->Register_Event("MeshTrail_Hand21", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseHandTrail(); });

    pModel->Register_Event("MeshTrail_SW0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW4", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW5", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW6", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW7", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW8", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW9", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW10", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });
    pModel->Register_Event("MeshTrail_SW11", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_2PhaseSwordTrail(); });

    return S_OK;
}

void CViper::Pick_ViperRock()
{
    _float3 vSpawnPoint{};
    _matrix ConvertMatrix = XMLoadFloat4x4(m_pThrowMatrix);
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    _matrix MulMatrix = ConvertMatrix * WorldMatrix;
    _float4x4 ThrowMatrix{};
    XMStoreFloat4x4(&ThrowMatrix, MulMatrix);
    vSpawnPoint.x = ThrowMatrix.m[3][0];
    vSpawnPoint.y = ThrowMatrix.m[3][1];
    vSpawnPoint.z = ThrowMatrix.m[3][2];

    CGameObject* pGameObject = m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_Rock"));
    if (nullptr == pGameObject)
        return;

    m_pRock = static_cast<CProjectile_Rock_Viper*>(pGameObject);
    if (m_pRock == nullptr)
        return;

    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pRock->Set_SpawnDir(vNormalize);
    m_pRock->Set_IsActive(false);   // 던지지 않음
    m_pRock->Set_Visible(true);     // 보이게
    m_pRock->Set_SpanwPoint(vSpawnPoint);
    m_pRock->Reset();

    m_pGameInstance->Push_PoolObject_ToLayer(
        ENUM_CLASS(LEVEL::VIPER),
        TEXT("Layer_Viper_Rock"),
        m_pRock
    );
}

void CViper::Hold_ViperRock()
{
    if (nullptr == m_pRock)
        return;

    _float3 vSpawnPoint{};
    _matrix ConvertMatrix = XMLoadFloat4x4(m_pThrowMatrix);
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    _matrix MulMatrix = ConvertMatrix * WorldMatrix;
    _float4x4 ThrowMatrix{};
    XMStoreFloat4x4(&ThrowMatrix, MulMatrix);
    vSpawnPoint.x = ThrowMatrix.m[3][0];
    vSpawnPoint.y = ThrowMatrix.m[3][1];
    vSpawnPoint.z = ThrowMatrix.m[3][2];

    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pRock->Set_SpawnDir(vNormalize);
    m_pRock->Set_SpanwPoint(vSpawnPoint);
    m_pRock->Reset();
    m_pRock->Enter_State(CProjectile::IDLE);
}

void CViper::Throw_ViperRock()
{
    if (m_pRock == nullptr)
        return;

    CTransform* pTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vOffset = XMVectorSet(-0.f, -0.01f, 0.f, 0.f);
    _vector vTargetLoc = pTransform->Get_State(STATE::POSITION) + vOffset;

    _float3 vSpawnPoint{};
    _matrix ConvertMatrix = XMLoadFloat4x4(m_pThrowMatrix);
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    _matrix MulMatrix = ConvertMatrix * WorldMatrix;
    _float4x4 ThrowMatrix{};
    XMStoreFloat4x4(&ThrowMatrix, MulMatrix);
    vSpawnPoint.x = ThrowMatrix.m[3][0];
    vSpawnPoint.y = ThrowMatrix.m[3][1];
    vSpawnPoint.z = ThrowMatrix.m[3][2];


    _vector vDir = vTargetLoc - XMLoadFloat3(&vSpawnPoint);
    vDir = XMVector3Normalize(vDir);
    _float3 vSpawnDir{};
    XMStoreFloat3(&vSpawnDir, vDir);

    m_pRock->Set_SpanwPoint(vSpawnPoint);
    m_pRock->Set_SpawnDir(vSpawnDir);
    m_pRock->Reset();
    m_pRock->Set_IsActive(true);
    m_pRock->Fire_Projectile();
    m_pRock->Enter_State(CProjectile::LOOP);

    //CModel* pModel = static_cast<CModel*>(m_pRock->Get_Component(TEXT("Com_Model")));
    //pModel->Set_Animation(2);

}

_vector CViper::Decompose_Rotation(_matrix W, _vector localRot, _vector offset)
{ 
    _vector S, Q, T;

    _matrix Local_Rotation = XMMatrixRotationQuaternion(localRot);
    W = XMMatrixMultiply(Local_Rotation, W);

    if (!XMMatrixDecompose(&S, &Q, &T, W))
    {
        XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);
        _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));

        _matrix RotationMatrix(
            r0,
            r1,
            r2,
            offset
        );
        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }

    return Q;
}

void CViper::FX_1PhaseTrail()
{
    _vector vSwordLeftStart = m_pWeapon->Get_LeftSwordStartPos();
    _vector vSwordLeftEnd = m_pWeapon->Get_LeftSwordTip();
    m_p1PhaseTrail[ENUM_CLASS(TWINBLADE::LEFT)]->Add_ControlPoint(vSwordLeftEnd, vSwordLeftStart);

    _vector vSwordRightStart = m_pWeapon->Get_RightSwordStartPos();
    _vector vSwordRightEnd = m_pWeapon->Get_RightSwordTip();
    m_p1PhaseTrail[ENUM_CLASS(TWINBLADE::RIGHT)]->Add_ControlPoint(vSwordRightEnd, vSwordRightStart);
}

void CViper::FX_2PhaseHandTrail()
{
    _vector vFingerPos = m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger12").r[3];
    _vector vFingerRight = m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger12").r[0];
    _vector vHandStart = vFingerPos - vFingerRight * 20.f;
    _vector vHandEnd = vFingerPos + vFingerRight * 40.f;
    m_p2PhaseTrail[ENUM_CLASS(TWINBLADE_R::FINGER0)]->Add_ControlPoint(vHandEnd, vHandStart);

    vFingerPos = m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger22").r[3];
    vFingerRight = m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger22").r[0];
    vHandStart = vFingerPos - vFingerRight * 20.f;
    vHandEnd = vFingerPos + vFingerRight * 55.f;
    m_p2PhaseTrail[ENUM_CLASS(TWINBLADE_R::FINGER1)]->Add_ControlPoint(vHandEnd, vHandStart);

    vFingerPos = m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger32").r[3];
    vFingerRight = m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger32").r[0];
    vHandStart = vFingerPos - vFingerRight * 20.f;
    vHandEnd = vFingerPos + vFingerRight * 40.f;
    m_p2PhaseTrail[ENUM_CLASS(TWINBLADE_R::FINGER2)]->Add_ControlPoint(vHandEnd, vHandStart);
}

void CViper::FX_2PhaseSwordTrail()
{
    _vector vSwordStart = m_pP2Weapon->Get_BladeStartPos();
    _vector vSwordEnd = m_pP2Weapon->Get_BladeTipPos();
    m_p2PhaseTrail[ENUM_CLASS(TWINBLADE_R::SWORD)]->Add_ControlPoint(vSwordEnd, vSwordStart);
}

void CViper::FX_2PhaseEyeTrail()
{
    _vector vLeftEyePos = m_pPahse2Body->Get_BoneMatrix("Bone_eye_L").r[3];
    _vector vRightEyePos = m_pPahse2Body->Get_BoneMatrix("Bone_eye_R").r[3];

    m_pLineTrail[ENUM_CLASS(EYE::LEFT)]->Add_ControlPoint(vLeftEyePos);
    m_pLineTrail[ENUM_CLASS(EYE::RIGHT)]->Add_ControlPoint(vRightEyePos);
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
    for (auto& pTrail : m_pLineTrail)
        Safe_Release(pTrail);

    for (auto& pTrail : m_p2PhaseTrail)
        Safe_Release(pTrail);

    for (auto& pTrail : m_p1PhaseTrail)
        Safe_Release(pTrail);
    
    Safe_Release(m_pBody);
    Safe_Release(m_pCinematicBody);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pCore);
    Safe_Release(m_pPahse2Body);
    Safe_Release(m_pP2Weapon);

    if (m_pRock)
        m_pRock->Set_IsDead(true);

    __super::Free();
}
