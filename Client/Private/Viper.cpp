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
#include "SkySphere.h"
#include "CloudSphere.h"

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

    // 2Phase 시네마틱 셰이더 세팅
    //  Viper_Cinematic_ShaderSettings();

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


    
    for (_uint i = 0; i < ENUM_CLASS(TWINBLADE::END); ++i)
        m_p1PhaseTrail[i] = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));
    
    
    for (_uint i = 0; i < ENUM_CLASS(TWINBLADE_R::END); ++i)
        m_p2PhaseTrail[i] = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));
    

    CLineTrail::LINE_TRAIL_DESC LineDesc{};
    LineDesc.fOffset = 0.25f;
    LineDesc.fLifeTime = 1.f;
    LineDesc.iDivisionCount = 5.f;
    LineDesc.iTextureIdx = 6;
    LineDesc.vColor = _float4(2.353f, 1.961f, 1.569f, 1.f);
    for (_uint i = 0; i < ENUM_CLASS(EYE::END); ++i)
        m_pLineTrail[i] = dynamic_cast<CLineTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LineTrail"), &LineDesc));

    m_bLoopFX_Flag = false;

    //Set_JustGuardCallBack();


    return S_OK;
}

void CViper::Priority_Update(_float fTimeDelta)
{
    CBlackBoard* pBB = m_pController->Get_BlackBoard();

    if (pBB->Get_Value<_bool>(m_strName, "isDetected") && !m_isUIHp)
    {
        if (m_ePhase == PHASE::PHASE1)
        {
            m_isUIHp = true;
            CBossHp::BOSSMON_UPDATE_DESC HPDesc{};
            HPDesc.isOpen = true;
            HPDesc.pHpMaxValue = &m_fMaxHP;
            HPDesc.pHpValue = &m_fCurrentHP;
            HPDesc.pStaminaMaxValue = &m_fMaxStamina;
            HPDesc.pStaminaCulValue = &m_fCurrentStamina;
            HPDesc.wstrName = TEXT("바이퍼");
            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &HPDesc);

        }
        
        else if (m_ePhase == PHASE::PHASE2)
        {
            m_isUIHp = true;
            CBossHp::BOSSMON_UPDATE_DESC HPDesc{};
            HPDesc.isOpen = true;
            HPDesc.pHpMaxValue = &m_fMaxHP;
            HPDesc.pHpValue = &m_fCurrentHP;
            HPDesc.pStaminaMaxValue = &m_fMaxStamina;
            HPDesc.pStaminaCulValue = &m_fCurrentStamina;
            HPDesc.wstrName = TEXT("진 : 전격의 바이퍼 4세");
            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &HPDesc);


        }

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
    // Test
    //  if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
    //  {
    //      CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
    //      //  Viper_2PhaseBerserker_ShaderSettings();
    //      Spawn_BloodDecal();
    //  }

    if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
    {
        if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
        {
            Spawn_EmissiveCrackDecal(m_pTransformCom->Get_State(STATE::POSITION));
            CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
            //  Viper_2PhaseBerserker_ShaderSettings();
        }
    }

    if (m_pGameInstance->Key_Pressing(DIK_RSHIFT, fTimeDelta))
    {
        if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
        {
            //  CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
            //  Viper_2PhaseBerserkerEnd_ShaderSettings();
            Start_Thunder(0.6f, 6);
        }
    }

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

    m_isBerserker = m_pController->Get_BlackBoard()->Get_Value<_bool>(m_strName, "is_Berserker");

    if (m_isBerserker)
        FX_2PhaseEyeTrail();

    for (auto& pTrail : m_pLineTrail)
        pTrail->Update(fTimeDelta);

    // Test
    //  FX_1PhaseTrail();
    //  FX_2PhaseHandTrail();
    //  FX_2PhaseSwordTrail();
    // 이후 2페이즈 광전사 모드일 때부터 호출

    TRAIL_CONFIG Config{};
    Config.fLifeTime = 1.f;
    Config.iTextureIdx = 6;
    Config.iDivisionCount = 5;
    Config.vColor = _float4(3.353f, 2.961f, 1.569f, 1.f);
    Config.vSubColor = _float4(0.f, 0.f, 0.f, 0.f);
    m_pLineTrail[ENUM_CLASS(EYE::LEFT)]->Set_TrailConfig(Config);
    m_pLineTrail[ENUM_CLASS(EYE::RIGHT)]->Set_TrailConfig(Config);

   //if (m_pGameInstance->Key_Down(DIK_P))
   // {
   //     _vector vPos = m_pP2Weapon->Get_BladeTipPos();
   //     tmpIdx = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::VIPER), TEXT("Grap"), vPos);
   // }
   // if (m_pGameInstance->Key_Down(DIK_O))
   // {
   //     m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::VIPER), TEXT("Grap"), tmpIdx);
   // }


    if (m_bLoopFX_Flag)
    {
        if (m_ePhase == PHASE::PHASE1)
            m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx, m_pBody->Get_BoneMatrix("Bip001-Spine").r[3]);
        else if (m_ePhase == PHASE::PHASE2)
            m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx, m_pPahse2Body->Get_BoneMatrix("Bip001-Spine").r[3]);
        else
            m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx, m_pCinematicBody->Get_BoneMatrix("Bip001-Spine").r[3]);
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
    //    Desc.eType = static_cast<DECALTYPE>(m_pGameInstance->Rand(0.f, static_cast<_float>(DECALTYPE::EMISSIVE)));
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
    CTransform* pTargetTransform = static_cast<CTransform*>(
        m_pTarget->Get_Component(TEXT("Com_Transform")));
    if (nullptr == pTargetTransform)
        return;

    CCreature* pTarget = static_cast<CCreature*>(m_pTarget);

    CPartObject* pTargetBody = static_cast<CPartObject*>(pTarget->Find_PartObject(TEXT("Part_Body")));
    if (nullptr == pTargetBody)
        return;

    CModel* pTargetModel = static_cast<CModel*>(pTargetBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pTargetModel)
        return;
    

    // 그랩 애니메이션 실행시 위치 뼈
    _float4x4* BoneLocal = m_pBody->Get_BoneMatrix_Ptr("Bone_Wp_Hold");

    // 플레이어 명치 위치 정도 뼈
    //Part_Body
    _float4x4* TargetBoneLocal = pTargetModel->Get_BoneMatrix("FX_Body_ExpGained");

    _matrix OwnerBoneWorld = XMLoadFloat4x4(BoneLocal) * m_pTransformCom->Get_WorldMatrix();
    _matrix TargetBoneWorld = XMLoadFloat4x4(TargetBoneLocal) * pTargetTransform->Get_WorldMatrix();

    _vector vGrabPos = OwnerBoneWorld.r[3];
    _vector vTargetBoenPos = TargetBoneWorld.r[3];

    _vector vDelta = vGrabPos - vTargetBoenPos;

    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);

    vTargetPos += vDelta;
  
    pTargetTransform->Set_State(STATE::POSITION, vTargetPos);

    _vector vStabLook = XMVector3Normalize(OwnerBoneWorld.r[2]);

    _matrix TargetRot = XMMatrixRotationX(XMConvertToRadians(-30.f)) * XMMatrixRotationY(-30.f);
    

    _vector vTargetLook =
        XMVector3Normalize(XMVector3TransformNormal(vStabLook, TargetRot));

    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vTargetLook));
    vUp = XMVector3Cross(vTargetLook, vRight);

    pTargetTransform->Set_State(STATE::LOOK, vTargetLook);
    pTargetTransform->Set_State(STATE::RIGHT, vRight);
    pTargetTransform->Set_State(STATE::UP, vUp);


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


        });

    pModel->Register_Event("P1_StingSlash", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

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


    pModel->Register_Event("P1_TurnAttack0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision_R(true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pModel->Register_Event("P1_TurnAttack0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });




    pModel->Register_Event("TrunAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision_R(true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);


        });

    pModel->Register_Event("TrunAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });

#pragma endregion

#pragma region BACKJUMPSLASH

    pModel->Register_Event("P1_SlahsBackJumpAttack0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);

        });


    pModel->Register_Event("BackJumpMovement", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "BackJump", true);

        });

    pModel->Register_Event("BackJumpMovement", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "BackJump", false);
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);
        });


#pragma endregion

#pragma region JUMPSMASH

    pModel->Register_Event("P1_JumpStart", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);

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

        });


    pModel->Register_Event("P1_Landing", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            _vector vOwnerPos = m_pTransformCom->Get_State(STATE::POSITION);
            _vector vDir = vTargetPos - vOwnerPos;
            vDir = XMVector3Normalize(vDir);
            _float fOffset = 40.f;
            _vector vLandPos = vTargetPos + vDir * fOffset;
            m_pCharVirCom->Start_Dive(vTargetPos, 1.f);
            m_pWeapon->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("P1_LandShake", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CClientInstance::GetInstance()->ActiveCamera_Shaking(3.f, 1.f);
            m_pWeapon->Set_OnAttackCollision(false);

        });

#pragma endregion

#pragma region DEVOUR

    pModel->Register_Event("Devour_Attack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("Devour_Attack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Devour_Attack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("Devour_Attack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(false);

        });



    pModel->Register_Event("Devour_Attack3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("Devour_Attack3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Devour_Attack4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("Devour_Attack4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pWeapon->Set_OnAttackCollision(false);

        });


    pModel->Register_Event("Devour_Impurse", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {

            CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 0.5f);

            CTransform* pTargetTransform = m_pTarget->Get_Transform();
            CCreature* pDamagedTarget = static_cast<CCreature*>(m_pTarget);
            CClientInstance::GetInstance()->Set_PlayerInput(true);

            _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);

            _vector vDir = XMVector3Normalize(vTargetPos - vPosition);
            
            pDamagedTarget->KnockBack(vDir, 40.f, 60.f);
            pDamagedTarget->Take_Damage(50.f, HITREACTION::KNOCKBACK_STRONG);
        });







    pModel->Register_Event("P1_SpinStart", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;

            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "P1_SpinStart", true);
        });

    pModel->Register_Event("P1_SpinStart", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
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


    pModel->Register_Event("P1_StingGrab_StepBack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 20.f;

            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "isP1_StingGrab_StepBack",true);


        });


    pModel->Register_Event("P1_StingGrab_StepBack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "isP1_StingGrab_StepBack", false);

        });

    pModel->Register_Event("P1_StingGrab_Look", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            m_isLookAt = true;
            m_fTurnSpeed = 20.f;


        });


    pModel->Register_Event("P1_StingGrab_Rush", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = false;
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "isP1_StingGrab_Rush", true);

        });


    pModel->Register_Event("P1_StingGrab_Rush", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "isP1_StingGrab_Rush", false);

        });






    pModel->Register_Event("StingGrab_Hold", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            m_isLookAt = false;
            Grab_Check_Begin();
        });






#pragma endregion

#pragma region 5HITCombo



    pModel->Register_Event("P1_ComboReady", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 0.5f);

        });


    pModel->Register_Event("P1_ComboReady_Attack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pModel->Register_Event("P1_ComboReady_Attack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);

        });


    pModel->Register_Event("P1_ComboReady_Attack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {

            m_isLookAt = true;
            m_pWeapon->Set_OnAttackCollision(true);
            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
        });

    pModel->Register_Event("P1_ComboReady_Attack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pWeapon->Set_OnAttackCollision(false);
        });



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
            CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
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

    pP2Model->Register_Event("P2_StompStr_Slow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.01f, 0.05f, 1.f);
        });


    pP2Model->Register_Event("HandStompStr_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 바디 오른손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);
            CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);


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

            m_pP2Weapon->Set_OnAttackCollision(true);
            CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);


        });

    pP2Model->Register_Event("DashUpperAttack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pP2Weapon->Set_OnAttackCollision(false);

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
            
            
        });


    pP2Model->Register_Event("FakeRunAttackAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isGhost = false;

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

    pP2Model->Register_Event("FakeAttac_Movement1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 30.f;
            m_isGhost = true;
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName,"isFakeAttac_Movement1",true);

        });

    pP2Model->Register_Event("FakeAttac_Movement1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            //m_isGhost = false;
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "isFakeAttac_Movement1", false);

        });


    pP2Model->Register_Event("FakeAttac_Movement2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = true;
            m_fTurnSpeed = 30.f;
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "FakeAttac_Movement2", true);



        });

    pP2Model->Register_Event("FakeAttac_Movement2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "FakeAttac_Movement2", false);
            m_fTurnSpeed = 8.f;


        });


    pP2Model->Register_Event("FakeAttac_Movement3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "FakeAttac_Movement3", true);


        });

    pP2Model->Register_Event("FakeAttac_Movement3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 무기 공격 콜라이더 OFF
            m_isLookAt = false;
            m_fTurnSpeed = 8.f;

            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "FakeAttac_Movement3", false);

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
            CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);


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

            m_isGhost = true;

            CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);

            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            m_pCharVirCom->Jump(50.f, 7.f);

        });



    pP2Model->Register_Event("P2_JumpAttack_End", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            _vector vOwnerPos = m_pTransformCom->Get_State(STATE::POSITION);
            _vector vDir = vTargetPos - vOwnerPos;
            vDir = XMVector3Normalize(vDir);
            _float fOffset = 40.f;
            _vector vLandPos = vTargetPos + vDir * fOffset;
            m_pCharVirCom->Start_Dive(vTargetPos, 80.f);

        });


    pP2Model->Register_Event("P2_JumpAttack_Attack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {


            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

            // 왼손 공격 콜라이더 ON
            m_pPahse2Body->Set_OnAttackCollision(true);
            CClientInstance::GetInstance()->ActiveCamera_Shaking(3.f, 1.f);


            
        });

    pP2Model->Register_Event("P2_JumpAttack_Attack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {


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

            // 오른손 무기 콜라이더ON
            m_pP2Weapon->Set_OnAttackCollision(true);

            _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        });

    pP2Model->Register_Event("P2_JumpAttack_Attack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {

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


    pP2Model->Register_Event("P2_SideStepCencel_L", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            if (m_pController->Get_BlackBoard()->Get_Value<_bool>(m_strName, "is_Berserker"))
            {
                m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "SkipMontion", true);
            }

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


    pP2Model->Register_Event("P2_SideStepCencel_R", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            if (m_pController->Get_BlackBoard()->Get_Value<_bool>(m_strName, "is_Berserker"))
            {
                m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "SkipMontion", true);
            }

        });


#pragma endregion

#pragma region P2_VIPER_DASH_DRIFT

    pP2Model->Register_Event("DashDrift_Pause", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            m_isGhost = true;
            m_isLookAt = true;
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 1.f, 0.1f, 0.25f);

        });


    pP2Model->Register_Event("DashDrift_Pause", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {

            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName,"isP2_Dash_Abort", true);


        });


    pP2Model->Register_Event("P2_DashDriftAttack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {
            m_pPahse2Body->Set_OnAttackCollision(true);
        });


    pP2Model->Register_Event("P2_DashDriftAttack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {
            m_isLookAt = false;
            m_isGhost = false;
            //m_pPahse2Body->Set_OnAttackCollision(false);

        });



#pragma endregion

#pragma region P2_SWINGCOMBO


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


#pragma region P2_AROUND
    pP2Model->Register_Event("P2_SwingRound_Attack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {

            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("P2_SwingRound_Attack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {

            m_pPahse2Body->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("P2_SwingRound_Attack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {

            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("P2_SwingRound_Attack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {

            m_pPahse2Body->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("P2_SwingRound_Attack3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {

            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("P2_SwingRound_Attack3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {

            m_pPahse2Body->Set_OnAttackCollision(false);

        });

    pP2Model->Register_Event("P2_SwingRound_Attack4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this, pP2Model]()
        {

            m_pPahse2Body->Set_OnAttackCollision(true);

        });

    pP2Model->Register_Event("P2_SwingRound_Attack4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this, pP2Model]()
        {

            m_pPahse2Body->Set_OnAttackCollision(false);
            //  CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);

        });


#pragma endregion



    return S_OK;

}

HRESULT CViper::Ready_AnimEffectEvent()
{
    // ======================================== [ 1 Phase ] ========================================

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
        m_iFireFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), m_pCore->Get_CoreCenter());
        });
    pModel->Register_Event("GSFire0_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { 
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_fire2"), m_iFireFX_Idx, m_pCore->Get_CoreCenter());
        });
    pModel->Register_Event("GSFire1_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { 
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
       
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Point_Particle_Blust"), m_pCore->Get_CoreTip());
        });

    pModel->Register_Event("Sting_BloodLoop_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iBloodFX_Idx[0] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        m_iBodyParticleFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_pTransformCom->Get_State(STATE::POSITION));
        m_bLoopFX_Flag = true;
        });

    pModel->Register_Event("Sting_BloodLoop_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0], m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        //m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("Sting_BloodLoop2_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0], m_pBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        //m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx, m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("Sting_BloodLoop2_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0]);
        //m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx);
        //m_bLoopFX_Flag = false;
        });


    pModel->Register_Event("BloodMouth0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(0.8f, 0.8f);
        
        // HDR 노란빛 조명
        LIGHT_TRANSITION_DESC LightDesc{};
        LightDesc.fDuration = 3.f;
        LightDesc.vFadeTime = _float2(3.f, 0.f);
        LightDesc.vDiffuse = _float4(15.f, 15.f, 10.f, 1.f);
        LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 0.1f);
        LightDesc.vSpecular = _float4(1.5f, 1.5f, 1.f, 1.f);
        LightDesc.isReturnToStart = false;
        m_pGameInstance->Start_LightTransition(TEXT("Viper_CutScene_PointLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
        
        });

    pModel->Register_Event("BloodMouth1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("BloodMouth2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(0.8f, 0.8f);
        });

    pModel->Register_Event("BloodMouth3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("BloodMouth4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(0.8f, 0.8f);
        
        // 어두운 흰 조명
        LIGHT_TRANSITION_DESC LightDesc{};
        LightDesc.fDuration = 1.f;
        LightDesc.vFadeTime = _float2(1.f, 0.f);
        LightDesc.vDiffuse = _float4(1.f, 1.f, 0.8f, 1.f);
        LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.4f, 1.f);
        LightDesc.vSpecular = LightDesc.vDiffuse;
        LightDesc.isReturnToStart = false;
        m_pGameInstance->Start_LightTransition(TEXT("Viper_CutScene_PointLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
        });

    pModel->Register_Event("BloodMouth5_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood_once"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle_Blust"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("HurtRoar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle_Blust"), m_pTransformCom->Get_State(STATE::POSITION));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("mist4"), m_pBody->Get_BoneMatrix("Bip001-L-Hand").r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_CutSceen_Land"), m_pBody->Get_BoneMatrix("Bip001-R-Hand").r[3]);
        });

    //1패 컷씬

    pModel->Register_Event("Cubcscene1_Breath0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Cubcscene1_Breath1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Cubcscene1_Breath2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Cubcscene1_Breath3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Cubcscene1_Breath4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), rot, m_pBody->Get_BoneMatrix("Bone_tongue_04").r[3]);
        });

    pModel->Register_Event("Cubcscene1_Roar_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_iRotFX_Idx = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), rot, XMVectorSet(-31.938f, -29.986f, 201.162f, 1.f));
        });

    pModel->Register_Event("Cubcscene1_Roar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), m_iRotFX_Idx);
        });

    pModel->Register_Event("Viper_StartCutScene_Jump_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("mist2"), XMVectorSet(-31.938f, -29.986f, 198.162f, 1.f));
        });

    pModel->Register_Event("WeaponDown_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Soward_Particle_red"), m_pBody->Get_BoneMatrix("Bip001-R-Hand").r[3]);
        });

    /////=========================================================== [ 2P ] ================================================================================ ////
    // CutScene
    pModel->Register_Event("CameraShaking0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 메인 조명 낮추기
        LIGHT_TRANSITION_DESC LightDesc{};
        LightDesc.fDuration = 2.f;
        LightDesc.vFadeTime = _float2(2.f, 0.f);
        LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 0.7f);
        LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
        LightDesc.vSpecular = LightDesc.vDiffuse;
        LightDesc.isReturnToStart = false;
        m_pGameInstance->Start_LightTransition(TEXT("MainLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 3.f);
        });

    pModel->Register_Event("CameraShaking1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        });

    pModel->Register_Event("CoreLightOn", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 조명 키기, 블링크 조명
        m_pGameInstance->Set_LightEnable(TEXT("Viper_Core"), ENUM_CLASS(LEVEL::VIPER), true);
        
        LIGHT_TRANSITION_DESC LightDesc{};
        LightDesc.fDuration = 30.f;
        LightDesc.vFadeTime = _float2(0.f, 0.f);
        LightDesc.vDiffuse = _float4(1.9f, 1.9f, 1.7f, 1.f);
        LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.5f, 1.f);
        LightDesc.vSpecular = LightDesc.vDiffuse;
        LightDesc.isReturnToStart = true;
        LightDesc.iBlinkCount = 150;
        m_pGameInstance->Start_LightTransition(TEXT("Viper_Core"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
        });

    pModel->Register_Event("CoreLightOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 코어 조명 끄기
        m_pGameInstance->Set_LightEnable(TEXT("Viper_Core"), ENUM_CLASS(LEVEL::VIPER), false);
        // 피 데칼 스폰
        Spawn_BloodDecal();
        // 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        // 위에서 아래로 붉은 빛 흰색 HDR 조명
        LIGHT_TRANSITION_DESC LightDesc{};
        LightDesc.fDuration = 3.f;
        LightDesc.vFadeTime = _float2(3.f, 0.f);
        LightDesc.vDiffuse = _float4(10.f, 7.f, 5.f, 1.f);
        LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
        LightDesc.vSpecular = _float4(1.f, 1.f, 0.5f, 1.f);
        LightDesc.isReturnToStart = false;
        m_pGameInstance->Start_LightTransition(TEXT("Viper_CutScene_PointLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
        });

    pModel->Register_Event("CameraShaking2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        // 림라이트 Intensity 높이기
        m_pBody->Set_EnableRimLight(true);
        });

    pModel->Register_Event("CameraShaking3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 건물 부시기
        Viper_Cinematic_ShaderSettings();
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.5f, 1.f);
        // 블링크 림라이트 시작
        m_pBody->Set_EnableBlinkRimLight(true);
        });

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

    // ======================================== [ 2 Phase ] ========================================
    
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
        // 이미시브 데칼 스폰
        Spawn_EmissiveCrackDecal(m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        });
    
    pModel->Register_Event("Hand_Stomp_Strong_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land_Big"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        // 이미시브 데칼 스폰
        Spawn_EmissiveCrackDecal(m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
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
        // 이미시브 데칼 스폰
        Spawn_EmissiveCrackDecal(m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
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
        // 이미시브 데칼 스폰
        Spawn_EmissiveCrackDecal(m_pTransformCom->Get_State(STATE::POSITION));
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
        // 이미시브 데칼 스폰
        Spawn_EmissiveCrackDecal(m_pTransformCom->Get_State(STATE::POSITION));
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
        // 이미시브 데칼 스폰
        Spawn_EmissiveCrackDecal(m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        });
    
    pModel->Register_Event("JumpAtk_Stamp_RightHand_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pTransformCom->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SphereTrail_V"), rot, m_pTransformCom->Get_State(STATE::POSITION));
        });
    
    pModel->Register_Event("JumpAtk_Stamp_RightHand_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Land"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
        // 이미시브 데칼 스폰
        Spawn_EmissiveCrackDecal(m_pPahse2Body->Get_BoneMatrix("Bip001-L-Finger2").r[3]);
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
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_R1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_R2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_R3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_R4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_R5_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_R6_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_R7_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_R8_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-R-Foot").r[3]);
        });

    pModel->Register_Event("CutSceenFoot_L0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_L1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_L2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_L3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_L4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_L5_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_L6_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Foot").r[3]);
        });
    pModel->Register_Event("CutSceenFoot_L7_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), 발위치);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_Footprint"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Foot").r[3]);
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
        // 트윈 블레이드 범위 줄이기
        LIGHT_DESC LightDesc{};
        LightDesc.eType = LIGHT_DESC::POINT;
        LightDesc.vDiffuse = _float4(2.f, 1.5f, 1.2f, 1.f);
        LightDesc.vAmbient = _float4(0.5f, 0.35f, 0.3f, 1.f);
        LightDesc.vSpecular = LightDesc.vDiffuse;
        LightDesc.fRange = 3.5f;
        m_pGameInstance->Set_LightDesc(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(LEVEL::VIPER), LightDesc);
        // 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.2f, 2.f);
        // 레디얼 블러
        RADIAL_BLUR_DESC RadialDesc{};
        RadialDesc.vCenterUV = _float2(0.5f, 0.5f);
        RadialDesc.fSampleRadius = 0.05f;
        RadialDesc.vMaskRadius = _float2(0.f, 0.3f);
        RadialDesc.fExponent = 1.f;
        RadialDesc.iNumSamples = 16;
        RadialDesc.fAttenuation = 0.1f;
        RadialDesc.fStrength = 1.f;
        RadialDesc.fDuration = 2.f;
        RadialDesc.vFadeTime = _float2(0.5f, 0.5f);
        m_pGameInstance->Start_RadialBlur(RadialDesc);
        });

    pModel->Register_Event("Cutscene_LastRoar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("scream"), m_iRotFX_Idx);
        m_bLoopFX_Flag = false;
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Particle"), m_iBodyParticleFX_Idx);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("mist1"));
        });

    pModel->Register_Event("GrabGround_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("mist4"), m_pPahse2Body->Get_BoneMatrix("Bip001-L-Hand").r[3]);
        });

    pModel->Register_Event("CameraShaking10", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        });

    pModel->Register_Event("CameraShaking11", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        });

    pModel->Register_Event("CameraShaking12", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        });

    pModel->Register_Event("CameraShaking13", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        });

    pModel->Register_Event("CameraShaking14", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        });

    pModel->Register_Event("CameraShaking15", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        });

    pModel->Register_Event("CameraShaking16", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
        });

    pModel->Register_Event("CameraShaking17", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
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

    // 버서커 모드
    // 번!!!!개!!!!!
    pModel->Register_Event("DashDrift_Thunder0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashDrift_Thunder1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashDrift_Thunder2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashDrift_Thunder3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashDrift_Thunder4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashDrift_Thunder5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });

    pModel->Register_Event("DashDriftEnd_Thunder0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashDriftEnd_Thunder1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashDriftEnd_Thunder2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashDriftEnd_Thunder3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashDriftEnd_Thunder4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });

    pModel->Register_Event("DashUpper_Thunder0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashUpper_Thunder1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashUpper_Thunder2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashUpper_Thunder3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashUpper_Thunder4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashUpper_Thunder5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashUpper_Thunder6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashUpper_Thunder7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashUpper_Thunder8", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("DashUpper_Thunder9", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("DashUpper_Thunder10", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });

    pModel->Register_Event("JumpAttack_Thunder0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("JumpAttack_Thunder1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("JumpAttack_Thunder2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("JumpAttack_Thunder3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("JumpAttack_Thunder4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("JumpAttack_Thunder5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("JumpAttack_Thunder6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("JumpAttack_Thunder7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("JumpAttack_Thunder8", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("JumpAttack_Thunder9", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("JumpAttack_Thunder10", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });

    pModel->Register_Event("SwingCombo_Thunder0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("SwingCombo_Thunder1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("SwingCombo_Thunder2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("SwingCombo_Thunder3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("SwingCombo_Thunder4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("SwingCombo_Thunder5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("SwingCombo_Thunder6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });
    pModel->Register_Event("SwingCombo_Thunder7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.3f, 3); });
    pModel->Register_Event("SwingCombo_Thunder8", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { if (m_isBerserker) Start_Thunder(0.2f, 2); });

    // ======================================== [ Cinematic ] ========================================

    pModel = static_cast<CModel*>(m_pCinematicBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

    pModel->Register_Event("StartEnvi_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
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


    pModel->Register_Event("BloodLoop_HandsUp_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iBloodFX_Idx[0] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pCinematicBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        m_iBloodFX_Idx[1] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist001").r[3]);
        m_iBloodFX_Idx[2] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist").r[3]);
        });

    pModel->Register_Event("BloodLoop_HandsUp_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0], m_pCinematicBody->Get_BoneMatrix("Bip001-L-Forearm").r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[1], m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist001").r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[2], m_pCinematicBody->Get_BoneMatrix("Muscle_L_ForeTwist").r[3]);

        });

    pModel->Register_Event("BloodLoop_HandsUp_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[0]);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[1]);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_blood2_loop"), m_iBloodFX_Idx[2]);
        //위의 팔 피 루프 끝내기
        });

    pModel->Register_Event("BloodMouth6_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iBloodFX_Idx[0] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), m_pCinematicBody->Get_BoneMatrix("Bone_tongue_04_end").r[3]);
        });

    pModel->Register_Event("BloodMouth6_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_mouth_particle"), m_iBloodFX_Idx[0], m_pCinematicBody->Get_BoneMatrix("Bone_tongue_04_end").r[3]);
        });

    pModel->Register_Event("CameraShaking4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        m_pCinematicBody->Set_AccRimEmissvie(true);
        });

    pModel->Register_Event("CameraShaking5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });

    pModel->Register_Event("CameraShaking6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });

    pModel->Register_Event("CameraShaking7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });

    pModel->Register_Event("CameraShaking8", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });

    pModel->Register_Event("CameraShaking9", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });

    pModel->Register_Event("Cinematic_LastCameraShaking", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.5f);
        // 시네마틱 컷씬 조명 끄기
        LIGHT_TRANSITION_DESC LightTransDesc{};
        LightTransDesc.fDuration = 1.f;
        LightTransDesc.vFadeTime = _float2(1.f, 0.f);
        LightTransDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
        LightTransDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
        LightTransDesc.vSpecular = LightTransDesc.vDiffuse;
        LightTransDesc.isReturnToStart = false;
        LightTransDesc.Callback = [&]() { m_pGameInstance->Set_LightEnable(TEXT("Viper_CutScene_PointLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false); };
        m_pGameInstance->Start_LightTransition(TEXT("Viper_CutScene_PointLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightTransDesc);
        // 트윈 블레이드 범위 늘리기
        LIGHT_DESC LightDesc{};
        LightDesc.eType = LIGHT_DESC::POINT;
        LightDesc.vDiffuse = _float4(2.f, 1.5f, 1.2f, 1.f);
        LightDesc.vAmbient = _float4(0.5f, 0.35f, 0.3f, 1.f);
        LightDesc.vSpecular = LightDesc.vDiffuse;
        LightDesc.fRange = 15.f;
        m_pGameInstance->Set_LightDesc(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(LEVEL::VIPER), LightDesc);
        });

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
    _vector vLeftEyeRight = m_pPahse2Body->Get_BoneMatrix("Bone_eye_L").r[0];
    vLeftEyePos += vLeftEyeRight * 5.f;
    m_pLineTrail[ENUM_CLASS(EYE::LEFT)]->Add_ControlPoint(vLeftEyePos);

    _vector vRightEyePos = m_pPahse2Body->Get_BoneMatrix("Bone_eye_R").r[3];
    _vector vRightEyeRight = m_pPahse2Body->Get_BoneMatrix("Bone_eye_R").r[0];
    vRightEyePos += vRightEyeRight * 5.f;
    m_pLineTrail[ENUM_CLASS(EYE::RIGHT)]->Add_ControlPoint(vRightEyePos);
}

void CViper::Viper_Cinematic_ShaderSettings()
{
    _float fDuration = 3.f;

    // 메인 조명
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 0.7f);
    LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 0.4f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("MainLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // ON
    // 플레이어 주변광 점조명 주황색
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 0.371f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.0f);
    LightDesc.vSpecular = _float4(0.5f, 0.185f, 0.0f, 1.f);
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Orange"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 플레이어 주변광 점조명 흰색
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_White"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 바이퍼 무기 조명
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(2.f, 1.5f, 1.2f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.35f, 0.3f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    FOG_CONFIG FogConfig = m_pGameInstance->Get_FogConfig();
    FogConfig.isUseSubColor = false;
    FogConfig.isUseHeight = false;
    m_pGameInstance->Set_FogConfig(FogConfig);

    // 포그 세팅 (어두운 보라색)
    FOG_TRANSITION_DESC FogDesc{};
    FogDesc.fDensity = 0.05f;
    FogDesc.fBias = 0.95f;
    FogDesc.vColor = _float4(0.1f, 0.053f, 0.086f, 1.f);
    FogDesc.isUseHeight = false;
    FogDesc.isUseNoise = false;
    m_pGameInstance->Start_FogTransition(fDuration, FogDesc);

    // 스카이 박스 세팅
    SKY_DESC SkyDesc{};
    SkyDesc.vNebulaColorR = _float3(0.147f, 0.076f, 0.125f);
    SkyDesc.vNebulaColorG = _float3(0.147f, 0.076f, 0.125f);
    SkyDesc.vNebulaColorB = _float3(0.f, 0.f, 0.f);
    SkyDesc.fStarStrength = 0.2f;
    SkyDesc.fMoonSize = 0.8f;
    SkyDesc.vMoonDirection = _float3(-0.21f, 0.19f, 1.f);
    SkyDesc.vMoonColor = _float3(1.f, 0.5f, 0.5f);
    SkyDesc.fMoonIntensity = 0.4f;
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, fDuration);

    // 클라우드 세팅
    CLOUD_DESC CloudDesc{};
    CloudDesc.vCloudColor = _float3(1.f, 1.f, 1.f);
    CloudDesc.fCloudSpeed = 0.25f;
    CloudDesc.fCloudScale = 1.f;
    CloudDesc.fCloudDensity = 1.f;
    CloudDesc.fCloudLightIntensity = 0.2f;
    CloudDesc.vLightDir = _float3(0.f, 1.f, 0.f);
    CloudDesc.fDynamic = 1.f;
    static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 1))->Start_LerpCloud(CloudDesc, fDuration);
    
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Gray"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
}

void CViper::Viper_2PhaseBerserker_ShaderSettings()
{
    // 림라이트 끄기
    m_pGameInstance->Set_EnableRimLight(false);

    // 광전사 모드 셰이더 세팅
    _float fDuration = 3.f;

    // 메인 조명 끄기
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("MainLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 점 조명 : 그레이 조명 켜지기, 오렌지, 화이트, 무기 조명은 꺼지기
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.409f, 0.381f, 0.295f, 1.f);
    LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 0.7f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Gray"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_White"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Orange"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 포그 검정
    FOG_TRANSITION_DESC FogDesc{};
    FogDesc.fDensity = 0.05f;
    FogDesc.fBias = 0.95f;
    FogDesc.vColor = _float4(0.f, 0.f, 0.f, 0.f);
    FogDesc.isUseHeight = false;
    FogDesc.isUseNoise = false;
    FogDesc.Callback = [&]() { m_pGameInstance->Set_EnableFog(false); };
    m_pGameInstance->Start_FogTransition(fDuration, FogDesc);

    // 스카이 검정
    SKY_DESC SkyDesc{};
    SkyDesc.vNebulaColorR = _float3(0.f, 0.f, 0.f);
    SkyDesc.vNebulaColorG = _float3(0.f, 0.f, 0.f);
    SkyDesc.vNebulaColorB = _float3(0.f, 0.f, 0.f);
    SkyDesc.fStarStrength = 0.f;
    SkyDesc.fMoonSize = 0.8f;
    SkyDesc.vMoonDirection = _float3(-0.21f, 0.19f, 1.f);
    SkyDesc.vMoonColor = _float3(0.f, 0.f, 0.f);
    SkyDesc.fMoonIntensity = 0.f;
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, fDuration);

    // 클라우드 세팅
    CLOUD_DESC CloudDesc{};
    CloudDesc.vCloudColor = _float3(0.f, 0.f, 0.f);
    CloudDesc.fCloudSpeed = 0.f;
    CloudDesc.fCloudScale = 0.f;
    CloudDesc.fCloudDensity = 0.f;
    CloudDesc.fCloudLightIntensity = 0.f;
    CloudDesc.vLightDir = _float3(0.f, 0.f, 0.f);
    CloudDesc.fDynamic = 0.f;
    static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 1))->Start_LerpCloud(CloudDesc, fDuration);
}

void CViper::Viper_2PhaseBerserkerEnd_ShaderSettings()
{
    // 림라이트 켜기
    m_pGameInstance->Set_EnableRimLight(true);

    _float fDuration = 3.f;

    // 메인 조명
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 0.7f);
    LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 0.4f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("MainLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // ON
    // 플레이어 주변광 점조명 주황색
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 0.371f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.0f);
    LightDesc.vSpecular = _float4(0.5f, 0.185f, 0.0f, 1.f);
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Orange"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 플레이어 주변광 점조명 흰색
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_White"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 바이퍼 무기 조명
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(2.f, 1.5f, 1.2f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.35f, 0.3f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    FOG_CONFIG FogConfig = m_pGameInstance->Get_FogConfig();
    FogConfig.isUseSubColor = false;
    FogConfig.isUseHeight = false;
    m_pGameInstance->Set_FogConfig(FogConfig);

    // 포그 세팅 (어두운 보라색)
    FOG_TRANSITION_DESC FogDesc{};
    FogDesc.fDensity = 0.05f;
    FogDesc.fBias = 0.8f;
    FogDesc.vColor = _float4(0.f, 0.012f, 0.039f, 1.f);
    FogDesc.isUseHeight = false;
    FogDesc.isUseNoise = false;
    m_pGameInstance->Start_FogTransition(fDuration, FogDesc);

    // 스카이 박스 세팅
    SKY_DESC SkyDesc{};
    SkyDesc.vNebulaColorR = _float3(0.f, 0.035f, 0.082f);
    SkyDesc.vNebulaColorG = _float3(0.f, 0.035f, 0.082f);
    SkyDesc.vNebulaColorB = _float3(0.f, 0.f, 0.f);
    SkyDesc.fStarStrength = 2.f;
    SkyDesc.fMoonSize = 0.8f;
    SkyDesc.vMoonDirection = _float3(-0.21f, 0.19f, 1.f);
    SkyDesc.vMoonColor = _float3(0.822f, 0.822f, 0.822f);
    SkyDesc.fMoonIntensity = 0.3f;
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, fDuration);

    // 클라우드 세팅
    CLOUD_DESC CloudDesc{};
    CloudDesc.vCloudColor = _float3(1.f, 1.f, 1.f);
    CloudDesc.fCloudSpeed = 0.25f;
    CloudDesc.fCloudScale = 1.f;
    CloudDesc.fCloudDensity = 2.f;
    CloudDesc.fCloudLightIntensity = 0.2f;
    CloudDesc.vLightDir = _float3(0.f, 1.f, 0.f);
    CloudDesc.fDynamic = 1.f;
    static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 1))->Start_LerpCloud(CloudDesc, fDuration);

    // Test
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Gray"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
}

void CViper::Start_Thunder(_float fDuration, _uint iBlinkCount)
{
    m_pGameInstance->Set_LightEnable(TEXT("Viper_Thunder_Ambient"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), true);
    m_pGameInstance->Set_LightEnable(TEXT("Viper_Thunder"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), true);

    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(9.f, 8.5f, 7.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = true;
    LightDesc.iBlinkCount = iBlinkCount;
    LightDesc.Callback = [&]() 
        { 
            m_pGameInstance->Set_LightEnable(TEXT("Viper_Thunder"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false);
            m_pGameInstance->Set_LightEnable(TEXT("Viper_Thunder_Ambient"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false);
        };

    m_pGameInstance->Start_LightTransition(TEXT("Viper_Thunder"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
}

void CViper::Spawn_BloodDecal()
{
    DECAL_DESC Desc{};
    Desc.fLifeTime = 55.f;
    Desc.vFadeTime = _float2(0.2f, 0.2f);
    Desc.vColor = _float3(0.2245f, 0.08f, 0.08f);
    Desc.eType = DECALTYPE::CIRCLE;
    _vector vDecalPos = m_pTransformCom->Get_State(STATE::POSITION);
    _float fOffset = 2.f;
    _float fPosX = XMVectorGetX(vDecalPos);
    _float fPosZ = XMVectorGetZ(vDecalPos);
    Desc.vScale = _float3(7.f, 0.7f, 7.f);
    Desc.isRandomTexture = false;

    for (_uint i = 0; i < 10; ++i)
    {
        vDecalPos = XMVectorSetX(vDecalPos, m_pGameInstance->Rand(fPosX - fOffset, fPosX + fOffset));
        vDecalPos = XMVectorSetZ(vDecalPos, m_pGameInstance->Rand(fPosZ - fOffset, fPosZ + fOffset));
        XMStoreFloat3(&Desc.vPosition, vDecalPos);

        Desc.iTextureIndex = static_cast<_uint>(m_pGameInstance->Rand(1.f, 4.f));
        m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Decal"), Desc);
    }
}

void CViper::Spawn_EmissiveCrackDecal(_fvector vPosition)
{
    DECAL_DESC Desc{};
    Desc.fLifeTime = 5.f;
    Desc.vFadeTime = _float2(0.2f, 0.2f);
    Desc.eType = DECALTYPE::EMISSIVE;
    XMStoreFloat3(&Desc.vPosition, vPosition);
    Desc.vScale = _float3(7.f, 0.7f, 7.f);
    Desc.EmissiveDesc.vBaseColor = _float3(0.12f, 0.1f, 0.08f);
    Desc.EmissiveDesc.vEmissiveColor = _float3(1.2f, 1.f, 0.8f);
    Desc.EmissiveDesc.vBorderColor = _float3(0.f, 0.f, 0.f);
    Desc.EmissiveDesc.fEmissiveMaskPower = 1.f;
    Desc.EmissiveDesc.fEmissiveIntensity = 3.f;
    Desc.isRandomTexture = false;
    Desc.iTextureIndex = 7;
    m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Decal"), Desc);
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
