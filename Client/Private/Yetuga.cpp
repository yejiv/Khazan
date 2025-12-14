#include "Yetuga.h"
#include "ClientInstance.h"
#include "GameInstance.h"
#include "AI_Controller_Yetuga.h"
#include "BlackBoard.h"
#include "Body_Yetuga.h"
#include "CharacterVirtual.h"
#include "Projectile_Yetuga.h"
#include "Projectile_Rock_Yetuga.h"
#include "Projectile_Breath_Yetuga.h"
#include "BossHp.h"
#include "Head_Yetuga.h"
#include "Viper.h"
#include "FSM_Yetuga.h"
#include "AS_CutScene_Yetuga.h"
#include "Destructible_Stone.h"

CYetuga::CYetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice, pContext }
{
}

CYetuga::CYetuga(const CYetuga& Prototype)
    :CMonster{ Prototype }
{
}

_float4* CYetuga::Get_LockOnPosition()
{
    return m_vLockOnPosition;
}

void CYetuga::Yetuga_Jump(_fvector vGoalPos, _float fHeight, _float fSpeed)
{
    m_pCharVirCom->Jump_Direction(vGoalPos, fHeight, fSpeed);
}

void CYetuga::Yetuga_Land(_fvector vGoalPosition, _float fSpeed)
{
    m_pCharVirCom->Start_Dive(vGoalPosition, fSpeed);
}

CAS_CutScene_Yetuga* CYetuga::Get_Yetuga_CutSceneState()
{
    CFSM_Yetuga* pFSM = static_cast<CFSM_Yetuga*>(m_pController->Get_State_Machine());
    CAS_CutScene_Yetuga* pCutSceneState = pFSM->Get_CutScene_Yetuga();
    return pCutSceneState;
}

void CYetuga::KnockBack(_vector vDir, _float fPower, _float fLoss)
{
    
    m_isKnockBack = true;
    m_fKnockBackDir = vDir;
    m_fKnockBackPower = fPower * 0.5f;
    m_fKnockBackLoss = fLoss;

    if (Get_IsGroggy() || m_isSuperArmmor)
        m_isKnockBack = false;

}


HRESULT CYetuga::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CYetuga::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pCharVirCom->Set_Position(XMVectorSet(537.354f, 18.684f, 221.961f, 1.f));

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;

    m_IceBreathSounds.resize(10);

    m_IceBreathSounds =
    {
        TEXT("Mon_efx_yetuga_icebreath_obj_idle_01 (SFX).wav"),
        TEXT("Mon_efx_yetuga_icebreath_obj_idle_02 (SFX).wav"),
        TEXT("Mon_efx_yetuga_icebreath_obj_idle_03 (SFX).wav"),
        TEXT("Mon_efx_yetuga_icebreath_obj_idle_04 (SFX).wav"),
        TEXT("Mon_efx_yetuga_icebreath_obj_idle_05 (SFX).wav"),
        TEXT("Mon_efx_yetuga_icebreath_obj_idle_06 (SFX).wav"),
    };

    if (FAILED(Ready_SFX()))
        return E_FAIL;


    m_pController = CAI_Controller_Yetuga::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    if(nullptr != m_pController)
        m_pController->Get_BlackBoard()->Set_Value(m_strName, "Target", m_pTarget);

    m_fRecoveryPerSec = 5.f;

    m_vDecalSize[ENUM_CLASS(DECALTYPE::LINEAR)] = { 4.f, 6.f };
    m_vDecalSize[ENUM_CLASS(DECALTYPE::CIRCLE)] = { 5.f, 7.f };
    m_vDecalSize[ENUM_CLASS(DECALTYPE::CURVE)] = { 4.f, 6.f };


    for (size_t i = 0; i < 5; i++)
    {
        CDestructible_Stone::STONE_DESC Desc;
        Desc.iLevelIndex = ENUM_CLASS(LEVEL::HEINMACH);
        Desc.vPos = XMVectorSet(1000.f, 1000.f, 1000.f, 1.f);

        CDestructible_Stone* pStone = dynamic_cast<CDestructible_Stone*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Destructible_Stone"), &Desc));
        m_pYetugaStones.push_back(pStone);
    }    

    return S_OK;
}

void CYetuga::Priority_Update(_float fTimeDelta)
{
    //if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
    //{
    //    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(537.354f, 18.684f, 221.961f, 1.f));
    //    m_pCharVirCom->Teleport(XMVectorSet(537.354f, 18.684f, 221.961f, 1.f), m_pTransformCom->Get_Rotation_Quat(), m_pTransformCom);
    //}

    //if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
    //{
    //    CAS_CutScene_Yetuga* Cut_Yetuga = Get_Yetuga_CutSceneState();
    //    Cut_Yetuga->YetugaScene_Jump(this);        
    //}

    //if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
    //{
    //    CAS_CutScene_Yetuga* Cut_Yetuga = Get_Yetuga_CutSceneState();
    //    Cut_Yetuga->YetugaScene_Land(this);
    //}

    CBlackBoard* pBB = m_pController->Get_BlackBoard();

    if (pBB->Get_Value<_bool>(m_strName, "isDetected"))
    {
        CBossHp::BOSSMON_UPDATE_DESC HPDesc{};
        HPDesc.isOpen = true;
        HPDesc.pHpMaxValue = &m_fMaxHP;
        HPDesc.pHpValue = &m_fCurrentHP;
        HPDesc.pStaminaMaxValue = &m_fMaxStamina;
        HPDesc.pStaminaCulValue = &m_fCurrentStamina;
        HPDesc.wstrName = TEXT("예투가");

        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &HPDesc);
    }
   
    CContainerObject::Priority_Update(fTimeDelta);
}

void CYetuga::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);
    
    if (m_fCurrentHP > 0.f)
    {
        if (this->Get_IsGroggy())
            return;

        if (m_isLookAt)
        {
            CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
            if (nullptr == pModel)
                return;
            _float fRatio = pModel->MakeRatio();
            Look_Target_Lerp(fTimeDelta, fRatio, m_fTurnSpeed);
        }

    }

    if (m_isLanding)
    {
        Update_Landing(fTimeDelta);
    }

  
    __super::Update(fTimeDelta);

    m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Body_ExpGained");

    // Radial Blur Focus Update
    if (m_pController->Get_BlackBoard()->Get_Value<_bool>(m_strName, "isDetected"))
        m_pGameInstance->Set_RadialBlurCenter(m_pTransformCom->Get_State(STATE::POSITION), 5.f);
   
    if (m_pGameInstance->Key_Down(DIK_Z))
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(516.947f, -11.952f, 226.f, 1.f));
        m_pCharVirCom->Set_Position(XMVectorSet(516.947f, -11.952f, 226.f, 1.f));
    }
}

void CYetuga::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    CContainerObject::Late_Update(fTimeDelta);
    

}

HRESULT CYetuga::Render()
{
    return S_OK;
}

void CYetuga::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);

    if (COLLISION_LAYER::PLAYER_ATTACK == eLayer)
    {
        _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
        _vector vHitDir = XMLoadFloat3(&vContactPoint) - vPosition;

        _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
        _float fDot = XMVectorGetX(XMVector3Dot(vLook, vHitDir));
        _vector vUp = XMVector3Cross(vLook, vHitDir);
        _float vUpY = XMVectorGetY(vUp);
        DIRECTION_INFO DirInfo{};

        if (fDot >= 0.f)
        {
            DirInfo.Add_Flag(DirInfo.F); 
        }
        else if(fDot < 0.f)
        {
            DirInfo.Add_Flag(DirInfo.B);
        }

      
        else if (vUpY >= 0.f)
        {
            DirInfo.Add_Flag(DirInfo.R);
        }
        else
        {
            DirInfo.Add_Flag(DirInfo.L);

        }

        //// Decal Spawn
        //_vector vDecalPos = m_pTransformCom->Get_State(STATE::POSITION);
        //_float fOffset = 2.f;
        //_float fPosX = XMVectorGetX(vDecalPos);
        //_float fPosZ = XMVectorGetZ(vDecalPos);
        //vDecalPos = XMVectorSetX(vDecalPos, m_pGameInstance->Rand(fPosX - fOffset, fPosX + fOffset));
        //vDecalPos = XMVectorSetZ(vDecalPos, m_pGameInstance->Rand(fPosZ - fOffset, fPosZ + fOffset));
        //DECAL_DESC Desc{};
        //Desc.fLifeTime = 8.f;
        //Desc.vFadeTime = _float2(0.2f, 0.2f);
        //Desc.eType = static_cast<DECALTYPE>(m_pGameInstance->Rand(0.f, static_cast<_float>(DECALTYPE::EMISSIVE)));
        //XMStoreFloat3(&Desc.vPosition, vDecalPos);
        //Desc.vScale = _float3( m_pGameInstance->Rand(4.f, 8.f), 2.f, m_pGameInstance->Rand(4.f, 8.f));
        //Desc.vColor = _float3(0.2745f, 0.08f, 0.08f);
        //Desc.isRandomTexture = true;

        //m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Decal"), Desc);
        _uint iIndex = m_pController->Get_State_Machine()->Get_CurrentState()->Get_StateIndex();
        if(iIndex == ENUM_CLASS(YETUGA_STATE::GROGGY) || iIndex == ENUM_CLASS(YETUGA_STATE::HIT))
            m_pController->Get_State_Machine()->OnCollision(pDesc, iOtherObjectLayer, this);

    }
}

void CYetuga::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CYetuga::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

void CYetuga::Update_Landing(_float fTimeDelta)
{
    if (!m_isLanding)
        return;

    CTransform* pTransform = m_pTransformCom;


    _vector vPos = pTransform->Get_State(STATE::POSITION);
    _float3 vCurr;
    XMStoreFloat3(&vCurr, vPos);

    // 목표점까지의 방향 (XZ만 이동)
    _float3 vTargetXZ = _float3(m_vLandingTargetPos.x, vCurr.y, m_vLandingTargetPos.z);
    _vector vGoal = XMLoadFloat3(&vTargetXZ);
    _vector vCurrPos = XMLoadFloat3(&vCurr);

    _vector vDirXZ = XMVector3Normalize(vGoal - vCurrPos);


    vCurr.x += XMVectorGetX(vDirXZ) * (m_fLandingHorizontalSpeed * fTimeDelta);
    vCurr.z += XMVectorGetZ(vDirXZ) * (m_fLandingHorizontalSpeed * fTimeDelta);

   
    float distXZ = XMVectorGetX(XMVector3Length(vGoal - XMVectorSet(vCurr.x, vCurr.y, vCurr.z, 1.f)));
    if (distXZ < 4.f)
        m_fLandingHorizontalSpeed = max(1.0f, m_fLandingHorizontalSpeed - 20.f * fTimeDelta);


    m_fLandingVerticalSpeed += m_fGravity * fTimeDelta;
    vCurr.y += m_fLandingVerticalSpeed * fTimeDelta;


    if (vCurr.y <= m_vLandingTargetPos.y)
    {
        vCurr.y = m_vLandingTargetPos.y;
        m_isLanding = false;

        m_fLandingVerticalSpeed = 0.f;
        m_fLandingHorizontalSpeed = 0.f;

    }


    _vector vNewPos = XMVectorSet(vCurr.x, vCurr.y, vCurr.z, 1.f);
    pTransform->Set_State(STATE::POSITION, vNewPos);
    m_pCharVirCom->Set_Position(vNewPos);
}






void CYetuga::Pick_Stone()
{
    _float3 vSpawnPoint = m_pBody->Get_BonePoint("Weapon_L");
    CGameObject* pGameObject = m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Stone"));
    if (nullptr == pGameObject)
        return;

    m_pHoldStone = static_cast<CProjectile_Yetuga*>(pGameObject);
    if (m_pHoldStone == nullptr)
        return;

    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pHoldStone->Set_SpawnDir(vNormalize);
    m_pHoldStone->Set_IsActive(false);   // 던지지 않음
    m_pHoldStone->Set_Visible(true);     // 보이게
    m_pHoldStone->Set_SpanwPoint(vSpawnPoint);
    m_pHoldStone->Reset();

    m_pGameInstance->Push_PoolObject_ToLayer(
        ENUM_CLASS(LEVEL::HEINMACH),
        TEXT("Layer_Yetuga_Stone"),
        m_pHoldStone
    );


    m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_fastball_start_01 (SFX).wav"),1.f);
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_fastball_start_01 (SFX).wav"), 1.f);
}

void CYetuga::Hold_Stone()
{
    if (nullptr == m_pHoldStone)
        return;

    _float3 vSpawnPoint = m_pBody->Get_BonePoint("Weapon_L");
    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pHoldStone->Set_SpawnDir(vNormalize);
    m_pHoldStone->Set_SpanwPoint(vSpawnPoint);
    m_pHoldStone->Reset();

}

void CYetuga::Throw_Stone()
{
    if (m_pHoldStone == nullptr)
        return;

    CTransform* pTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vOffset = XMVectorSet(-0.f, -0.01f, 0.f, 0.f);
    _vector vTargetLoc = pTransform->Get_State(STATE::POSITION) + vOffset;
    _float3 vSpawnPoint = m_pBody->Get_BonePoint("Weapon_L");
    _vector vDir = vTargetLoc - XMLoadFloat3(&vSpawnPoint);
    vDir = XMVector3Normalize(vDir);
    _float3 vSpawnDir{};
    XMStoreFloat3(&vSpawnDir, vDir);
    
    m_pHoldStone->Set_SpanwPoint(vSpawnPoint);
    m_pHoldStone->Set_SpawnDir(vSpawnDir);
    m_pHoldStone->Reset();
    m_pHoldStone->Set_IsActive(true);
    m_pHoldStone->Fire_Projectile();

    CModel* pModel = static_cast<CModel*>(m_pHoldStone->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(1);

    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_fastball_shoot_01 (SFX).wav"), 1.f);
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_fastball_shoot_01 (SFX).wav"), 1.f);

}

void CYetuga::Grab_Check_Begin(const _char* pBoneName)
{
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    if (nullptr == pTargetTransform)
        return;
    _matrix BoneWorld = m_pBody->Get_BoneMatrix(pBoneName);
    
    _vector vGrabPosition = BoneWorld.r[3];
    _vector vOffset = XMVectorSet(0.f, -1.f, 0.f, 0.f);
    pTargetTransform->Set_State(STATE::POSITION, vGrabPosition + vOffset);

}

void CYetuga::Grab_Check_End(const _char* pBoneName)
{
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    if (nullptr == pTargetTransform)
        return;
    _matrix BoneWorld = m_pBody->Get_BoneMatrix(pBoneName);

    _vector vGrabPosition = BoneWorld.r[3];
    pTargetTransform->Set_State(STATE::POSITION, vGrabPosition);
}

void CYetuga::Pick_Rock()
{
     _float3 vLHTemp = m_pBody->Get_BonePoint("Weapon_L");
    _float3 vRHTemp = m_pBody->Get_BonePoint("Weapon_R");

    _vector vLH = XMLoadFloat3(&vLHTemp);
    _vector vRH = XMLoadFloat3(&vRHTemp);
    _vector vCenterOffset = XMVectorSet(0.f, -1.f, 0.f, 0.f);
    _vector vSpawnTemp = (vLH + vRH) * 0.5f + vCenterOffset;
    _float3 vSpawnPoint = {};

    XMStoreFloat3(&vSpawnPoint,vSpawnTemp);
    
    CGameObject* pGameObject = m_pGameInstance->Pop_PoolObject(
        ENUM_CLASS(LEVEL::HEINMACH),
        TEXT("Yetuga_Rock")
    );
    if (nullptr == pGameObject)
        return;

    m_pHoldRock = static_cast<CProjectile_Rock_Yetuga*>(pGameObject);
    if (m_pHoldRock == nullptr)
        return;

    m_isRockPlay = true;

    // 타겟 방향 가져오기
    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);

    // 돌 초기 세팅
    m_pHoldRock->Set_IsActive(false);
    m_pHoldRock->Set_Visible(true);
    m_pHoldRock->Set_SpanwPoint(vSpawnPoint);
    m_pHoldRock->Set_SpawnDir(vNormalize);
    m_pHoldRock->Reset();

    m_pGameInstance->Push_PoolObject_ToLayer(
        ENUM_CLASS(LEVEL::HEINMACH),
        TEXT("Layer_Yetuga_Rock"),
        m_pHoldRock
    );
}

void CYetuga::Hold_Rock()
{

    if (!m_isRockPlay)
        return;

    if (nullptr == m_pHoldRock)
        return;

    _float3 LHTemp = m_pBody->Get_BonePoint("Weapon_L");
    _float3 RHTemp = m_pBody->Get_BonePoint("Weapon_R");

    _vector vLH = XMLoadFloat3(&LHTemp);
    _vector vRH = XMLoadFloat3(&RHTemp);

    _vector vCenterOffset = XMVectorSet(0.f, -2.5f, 0.f, 0.f);
    _vector vCenter = (vLH + vRH) * 0.5f + vCenterOffset;

    _float fAnimRatio = {};
    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    fAnimRatio = pModel->MakeRatio();

    _float fAngle = XM_PI * fAnimRatio * 0.7f;
    _float fY = sin(fAngle) * 7.5f;
    _float fZ = cos(fAngle) * 2.f;

    _vector vOffset = XMVectorSet(0.f, fY, fZ, 0.f);
    _vector vResult = vCenter + vOffset;

    _float3 vSpawnPoint{};
    XMStoreFloat3(&vSpawnPoint, vResult);

    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pHoldRock->Set_SpawnDir(vNormalize);
    m_pHoldRock->Set_SpanwPoint(vSpawnPoint);

    m_pHoldRock->Reset();

}

void CYetuga::Smash()
{

    _float3 vLHTemp = m_pBody->Get_BonePoint("Weapon_L");
    _float3 vRHTemp = m_pBody->Get_BonePoint("Weapon_R");

    _vector vLH = XMLoadFloat3(&vLHTemp);
    _vector vRH = XMLoadFloat3(&vRHTemp);
    _vector vCenterOffset = XMVectorSet(0.f, -0.7f, 0.f, 0.f);
    _vector vSpawnTemp = (vLH + vRH) * 0.5f + vCenterOffset;
    _float3 vSpawnPoint = {};

    XMStoreFloat3(&vSpawnPoint, vSpawnTemp);
    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pHoldRock->Set_SpawnDir(vNormalize);
    m_pHoldRock->Set_SpanwPoint(vSpawnPoint);

    m_pHoldRock->Reset();

    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    CBlackBoard* BB = m_pController->Get_BlackBoard();
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
    _float fAnimRatio = pModel->MakeRatio();
    _float fDistSq = BB->Get_Value<_float>(m_strName, "TargetDist");
    _float fLimit = 6.f;
    _float fLimitSq = fLimit * fLimit;

    _float3 vTempDir = BB->Get_Value<_float3>(m_strName, "TargetDir");
    if (fDistSq > fLimitSq)
    {
        _vector vDir = XMLoadFloat3(&vTempDir);

        _vector vGoalPos = vTargetPos - vDir * fLimit;


        _vector vNewPos = XMVectorLerp(vPosition, vGoalPos, fAnimRatio - 0.25f);
        m_pTransformCom->Set_State(STATE::POSITION, vNewPos);
    }
    else
    {
        _vector vDir = XMLoadFloat3(&vTempDir);
        _vector vStopPos = vTargetPos - vDir * fLimit;
        m_pTransformCom->Set_State(STATE::POSITION, vStopPos);
    }


}

void CYetuga::Breath_Start()
{
    _matrix BoneMatrix = m_pBody->Get_BoneMatrix("Bip001-Head");
    _vector vPosition = BoneMatrix.r[3];
    _vector vLook = XMVector3Normalize(BoneMatrix.r[2]);

    _vector vTempSpawnPoint = vPosition + vLook * 0.5f;
    _float3 vSpawnPoint{};
    XMStoreFloat3(&vSpawnPoint, vTempSpawnPoint);


    CGameObject* pGameObject = m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Breath"));
    if (nullptr == pGameObject)
        return;
    m_pBreath = static_cast<CProjectile_Breath_Yetuga*>(pGameObject);
    if (m_pBreath == nullptr)
        return;

    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pBreath->Set_SpawnDir(vNormalize);
    m_pBreath->Set_IsActive(false);
    m_pBreath->Set_Visible(true);
    m_pBreath->Set_SpanwPoint(vSpawnPoint);
    m_pBreath->Reset();

    m_pGameInstance->Push_PoolObject_ToLayer(
        ENUM_CLASS(LEVEL::HEINMACH),
        TEXT("Layer_Yetuga_Breath"),
        m_pBreath

    );
}

void CYetuga::Breath_Loop()
{
    if (nullptr == m_pBreath)
        return;

    _matrix BoneMatrix = m_pBody->Get_BoneMatrix("Bip001-Head");
    _vector vPosition = BoneMatrix.r[3];
    _vector vLook = XMVector3Normalize(BoneMatrix.r[1]);

    _vector vTempSpawnPoint = vPosition + vLook * 15.f;
    _float3 vSpawnPoint{};
    XMStoreFloat3(&vSpawnPoint, vTempSpawnPoint);

    _float3 vTargetDir = m_pController->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pBreath->Set_SpawnDir(vNormalize);
    m_pBreath->Set_SpanwPoint(vSpawnPoint);

    m_pBreath->Reset();

}

void CYetuga::Abort_Node(_bool isToggle)
{
    _float fDist = m_pController->Get_BlackBoard()->Get_Value<_float>(m_strName, "TargetDist");
    _float fAbortRange = m_pController->Get_BlackBoard()->Get_Value<_float>(m_strName, "JumpAttackRange");

    if (fDist >= fAbortRange)
        m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "isAbort", isToggle);
}

void CYetuga::Start_DefaultRadialBlur()
{
    RADIAL_BLUR_DESC Desc{};
    Desc.vCenterUV = _float2(0.5f, 0.5f);
    Desc.fSampleRadius = 0.05f;
    Desc.vMaskRadius = _float2(0.f, 0.7f);
    Desc.fExponent = 1.f;
    Desc.iNumSamples = 16;
    Desc.fAttenuation = 0.1f;
    Desc.fStrength = 0.5f;       // == Target Strength(0 ~ 1) -> 이 강도를 최대값으로 사용하여 보간 적용됨
    Desc.fDuration = 3.f;
    Desc.vFadeTime = _float2(0.3f, 1.f);
    m_pGameInstance->Start_RadialBlur(Desc);
}

HRESULT CYetuga::Ready_Components()
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
    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 0.3f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fMass = 10.f;
    tCharVirDesc.fMaxStrength = 10.f;
    tCharVirDesc.vStickToFloorStepDown = _float3(0.f, 0.f, 0.f);
    tCharVirDesc.vWalkStairsStepUp = _float3(0.f, 0.f, 0.f);
    tCharVirDesc.vWalkStairsStepDownExtra = _float3(0.f, 0.f, 0.f);
    //tCharVirDesc.fPredictiveContactDistance = 0.3f;
    //tCharVirDesc.iMaxConstraintIterations = 20;
    //tCharVirDesc.fCollisionTolerance = 0.03f;
    //tCharVirDesc.fPenetrationRecoverySpeed = 1.7f;
    
    m_tYetugaChaVir.pGameObject = this;
    m_tYetugaChaVir.strName = TEXT("tYetugaChaVir");
    m_tYetugaChaVir.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tYetugaChaVir;
   
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    m_pCharVirCom->Collision_Active(true);

    

    return S_OK;
}

HRESULT CYetuga::Ready_PartObjects()
{
    CBody_Yetuga::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    BodyDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"),ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Yetuga_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Yetuga*>(pBody);
    Safe_AddRef(m_pBody);

    CHead_Yetuga::HEAD_DESC HeadDesc{};
    HeadDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    HeadDesc.pOwnerTransform = m_pTransformCom;
    HeadDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Head"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Yetuga_Head"), &HeadDesc)))
        return E_FAIL;

    CPartObject* pHead = Find_PartObject(TEXT("Part_Head"));
    if (nullptr == pHead)
        return E_FAIL;

    m_pHead = dynamic_cast<CHead_Yetuga*>(pHead);
    Safe_AddRef(m_pHead);

    return S_OK;
}

HRESULT CYetuga::Ready_Projectiles()
{
    CProjectile_Yetuga::PROJECTILE_DESC Desc{};
    Desc.fDamage = 10.f;
    Desc.fSpeedPerSec = 50.f;
    Desc.fLifeTime = 3.f;
    Desc.fRotationPerSec = 180.f;

    m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH),TEXT("Prototype_Projectile_Yetuga_Stone"),
        ENUM_CLASS(LEVEL::HEINMACH),TEXT("Yetuga_Stone"),&Desc,5);

    CProjectile_Rock_Yetuga::PROJECTILE_DESC RockDesc{};
    RockDesc.fDamage = 30.f;
    RockDesc.fSpeedPerSec = 1.f;
    RockDesc.fLifeTime = 10.f;
    RockDesc.fRotationPerSec = 180.f;

       m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Yetuga_Rock"),
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Rock"), &RockDesc, 5);


       CProjectile_Breath_Yetuga::PROJECTILE_DESC BreathDesc{};
       BreathDesc.fDamage = 5.f;
       BreathDesc.fSpeedPerSec = 5.f;
       BreathDesc.fLifeTime = 10.f;
       BreathDesc.fRotationPerSec = 180.f;

       m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Yetuga_Breath"),
           ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Breath"), &BreathDesc ,5);

    return S_OK;
}

HRESULT CYetuga::Ready_AnimEvent()
{
    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

    Ready_AnimEffectEvent(pModel);
        
#pragma region ThrowRock

    pModel->Register_Event("ThrowBall", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        Pick_Stone(); 
        });
    
    pModel->Register_Event("ThrowBall", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        { 
            m_isLookAt = false;
            m_fTurnSpeed = 50.f;
            Throw_Stone(); 
        });

    pModel->Register_Event("ThrowBall", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() 
        {
            m_isLookAt = true;
            m_fTurnSpeed = 50.f;
            Hold_Stone(); 
        });

#pragma endregion

#pragma region 2HIT

    pModel->Register_Event("2Hit_Fake", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        m_fTurnSpeed = 40.f;
        m_isLookAt = true; 
        });
    pModel->Register_Event("2Hit_Fake", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { 
        m_isLookAt = false; });

    pModel->Register_Event("2Hit_Fake", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(3.f,fAnimRatio);

        });

    pModel->Register_Event("2Hit_One", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        m_fTurnSpeed = 40.f;
        _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
        m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
        m_pBody->Set_OnAttackCollision(true);
        m_isLookAt = true;
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 0.5f);
        });
    pModel->Register_Event("2Hit_One", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { 
        m_pBody->Set_OnAttackCollision(false);


        m_isLookAt = false; });

    pModel->Register_Event("2Hit_One", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(3.f, fAnimRatio);

            });


    pModel->Register_Event("2Hit_Two", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(true);
        _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
        m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);
        m_isLookAt = true; });
    pModel->Register_Event("2Hit_Two", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { 
        m_pBody->Set_OnAttackCollision(false);
        m_isLookAt = false; });


    pModel->Register_Event("2Hit_Two", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(3.f, fAnimRatio);

            });

#pragma endregion

#pragma region NormalSmash

    // Smash
    pModel->Register_Event("Smash_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = true;
        });

    pModel->Register_Event("Smash_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = false;
      
        });

    pModel->Register_Event("Smash_After", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = false;
        });

    // After_Smash
    pModel->Register_Event("Smash_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(true);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.2f, 0.4f, 1.5f);
        m_isLookAt = true;
        });
     
    // After_Smash
    pModel->Register_Event("Smash_After", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = true;
        });

    pModel->Register_Event("Smash_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = false;
        });


#pragma endregion

#pragma region TurnAttack

    pModel->Register_Event("TurnAttack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    m_fTurnSpeed = 40.f;
    m_pBody->Set_OnAttackCollision(true);
    m_isLookAt = true;
        });

    pModel->Register_Event("TurnAttack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(false);
        m_isLookAt = false;
        });


    pModel->Register_Event("TurnAttack_After", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 20.f;
        m_isLookAt = true;
        });

    pModel->Register_Event("TurnAttack_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 20.f;
        m_isLookAt = false;
        });

#pragma endregion

#pragma region Move_L_Attack

    pModel->Register_Event("Side_L_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = true;
        });

    pModel->Register_Event("Side_L_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = false;
        });

    pModel->Register_Event("Side_L_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(true);
        m_isLookAt = true;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });

    pModel->Register_Event("Side_L_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(false);
        m_isLookAt = false;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });


    pModel->Register_Event("Side_L_After", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = true;
        });

    pModel->Register_Event("Side_L_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = false;
        });


#pragma endregion

#pragma region Move_R_Attack

    pModel->Register_Event("Side_R_Look", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = true;
        });

    pModel->Register_Event("Side_R_Look", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = false;
        });

    pModel->Register_Event("Side_R_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(true);
        m_isLookAt = true;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    pModel->Register_Event("Side_R_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(false);
        m_isLookAt = false;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });


    pModel->Register_Event("Side_R_After", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = true;
        });

    pModel->Register_Event("Side_R_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_isLookAt = false;
        });

#pragma endregion

#pragma region DempeyRoll

    pModel->Register_Event("Dampsey_First", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        //Abort_Node(true);

        _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
        m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_First", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        Abort_Node(true);
        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_First", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });


    pModel->Register_Event("Dampsey_Second", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        Abort_Node(false);

        _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
        m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);


        });
    pModel->Register_Event("Dampsey_Second", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        
        Abort_Node(true);

        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_Second", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });



    pModel->Register_Event("Dampsey_Third", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
       
        Abort_Node(false);

        _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
        m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_Third", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
       
        Abort_Node(true);

        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_Third", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });


    pModel->Register_Event("Dampsey_Forth", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        Abort_Node(false);

        _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
        m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_Forth", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        Abort_Node(true);

        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_Forth", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });


    pModel->Register_Event("Dampsey_Final", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        Abort_Node(false);

        _uint iAttackCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName, "AttackCount");
        m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName, "AttackCount", iAttackCnt + 1);

        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_Final", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        Abort_Node(true);

        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_Final", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });



    pModel->Register_Event("Dampsey_After", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        Abort_Node(false);
        m_isLookAt = true;

        });
    pModel->Register_Event("Dampsey_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isLookAt = false;

        });

    //SlowStart


#pragma endregion

#pragma region BACKSMASH

    pModel->Register_Event("BackSmashOne", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        { 
            m_pBody->Set_AttackCollision_Back(true);
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-L-Foot")));
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-R-Foot")));
        });
    pModel->Register_Event("BackSmashOne", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        { 
            m_pBody->Set_AttackCollision_Back(false);
        });

    pModel->Register_Event("BackSmashTwo", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pBody->Set_OnAttackCollision(true);
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("BackSmashTwo", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        { 
            m_pBody->Set_OnAttackCollision(false);
        });
   
#pragma endregion

#pragma region JumpAttack
    pModel->Register_Event("JumpAttack_Jump", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        { 
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", true);
        });
    pModel->Register_Event("JumpAttack_Jump", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", false);

        });


    pModel->Register_Event("JumpAttack_Attack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isLookAt = true;
            m_pBody->Set_OnAttackCollision(true);
        });
    pModel->Register_Event("JumpAttack_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_isLookAt = false;
            m_pBody->Set_OnAttackCollision(false);
        });

    pModel->Register_Event("JumpAttack_Attack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pBody->Set_OnAttackCollision(true);
        });
    pModel->Register_Event("JumpAttack_Attack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pBody->Set_OnAttackCollision(false);
        });
   
#pragma endregion

#pragma region JumpGrab

    pModel->Register_Event("Grab_Collider", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pBody->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("Grab_Collider", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Jump_Grab_Jump", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", true);
            //m_pBody->Set_OnAttackCollision(true);

            // Radial Blur
            RADIAL_BLUR_DESC Desc{};
            Desc.vCenterUV = _float2(0.5f, 0.5f);
            Desc.fSampleRadius = 0.05f;
            Desc.vMaskRadius = _float2(0.f, 0.7f);
            Desc.fExponent = 1.f;
            Desc.iNumSamples = 16;
            Desc.fAttenuation = 0.1f;
            Desc.fStrength = 0.7f;       // == Target Strength(0 ~ 1) -> 이 강도를 최대값으로 사용하여 보간 적용됨
            Desc.fDuration = 1.5f;
            Desc.vFadeTime = _float2(0.3f, 1.f);
            m_pGameInstance->Start_RadialBlur(Desc);

            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), m_pTransformCom->Get_State(STATE::POSITION));

        });

    pModel->Register_Event("Jump_Grab_Jump", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", false);
        });

    pModel->Register_Event("Grab_Hand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isGhost = true;
            
            CCreature* pTarget = static_cast<CCreature*>(m_pTarget);
            pTarget->Take_Damage(0.f, HITREACTION::GRAB);
        });

    pModel->Register_Event("Grab_Hand", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
           // 손뼈행렬 전달
            Grab_Check_Begin("Weapon_L");
        });

    pModel->Register_Event("Grab_Hold", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
        });

    pModel->Register_Event("Grab_Hold", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            Grab_Check_End("Holding");
        });

    pModel->Register_Event("Grab_Hold", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // Distortion
            //  DISTORTION_DESC Desc{};
            //  _vector vCenterPos = m_pTransformCom->Get_State(STATE::POSITION);
            //  //  _float fPosY = XMVectorGetY(vCenterPos);
            //  //  _float fOffset = 2.f;
            //  //  vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
            //  XMStoreFloat3(&Desc.vCenter, vCenterPos);
            //  Desc.fRange = 1.f;
            //  Desc.fPower = 0.03f;
            //  Desc.fDuration = 0.5f;
            //  Desc.vFadeTime = _float2(0.1f, 0.2f);
            //  Desc.fSpeed = 2.f;
            //  Desc.iNoiseIndex = 14;
            //  m_pGameInstance->Start_Distortion(Desc);

            CCreature* pTarget = static_cast<CCreature*>(m_pTarget);
            pTarget->Take_Damage(300.f, HITREACTION::GRAB_FINISHED);


        });

    //Grab_After
    pModel->Register_Event("Grab_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 타겟 풀어주기
            m_isGhost = false;
            //CCreature* pTarget = static_cast<CCreature*>(m_pTarget);
            //pTarget->Take_Damage(5.f,HITREACTION::GRAB_FINISHED,nullptr);
            
        });


#pragma endregion

#pragma region RUSH

    pModel->Register_Event("RushCheck", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pHead->Set_OnAttackCollision(true);
            m_pBody->Set_AttackCollision_Back(true);
            m_isGhost = true;
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_charge_tackle_run_01 (SFX).wav"), 1.f);


        });

    pModel->Register_Event("RushCheck", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pHead->Set_OnAttackCollision(false);
            m_pBody->Set_AttackCollision_Back(false);
            m_isGhost = false;

        });

    //charge attack - 달리기 전에 왼발 구르기
    pModel->Register_Event("ChargeTackle _StampFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
   
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_charge_tackle_start_01 (SFX).wav"), 1.f);
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });



#pragma endregion

#pragma region Amageddon

    // 돌을 풀에 서 꺼낸다.
    pModel->Register_Event("AMG_RockEvent", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Pick_Rock(); });
    // 대미지를 주고 충돌끄고 풀로 보낸다.
    pModel->Register_Event("AMG_RockEvent", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { m_isRockPlay = false; });
    // Hold한다.
    pModel->Register_Event("AMG_RockEvent", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Hold_Rock(); });

    // 점프 시작
    pModel->Register_Event("AMG_JumpEvent", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {  m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), m_pTransformCom->Get_State(STATE::POSITION)); });
    // 최고 높이까지도달
    pModel->Register_Event("AMG_JumpEvent", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { });
    // 속도, 조절
    pModel->Register_Event("AMG_JumpEvent", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { });

    // 플레이어 위치로 방향 조절
    pModel->Register_Event("AMG_AimEvent", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        {
            Look_Target();
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.3f, 0.5f, 2.f);

        });

    pModel->Register_Event("AMG_AimEvent", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()
        {
            CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
            _float fAnimRatio = pModel->MakeRatio();
            CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetLoc = pTargetTransform->Get_State(STATE::POSITION);
            _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

            _vector vDir = vTargetLoc - vPos;
            vDir = XMVector3Normalize(vDir);

            _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
            vLook = XMVector3Normalize(vLook);

            _vector vLerp = XMVectorLerp(vLook, vDir, fAnimRatio * 0.8f);
            vLerp = XMVector3Normalize(vLerp);

            m_pTransformCom->Set_State(STATE::LOOK, vLerp);
        });


    pModel->Register_Event("AMG_SmashEvent", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { 
        Smash(); 
        CClientInstance::GetInstance()->ActiveCamera_Shaking(3.5f, 1.5f);
        });

    pModel->Register_Event("AMG_SmashEvent", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {       
        CDestructible_Stone* pStone = m_pYetugaStones.back();
        m_pYetugaStones.pop_back();
        pStone->Set_Pos(m_pHoldRock->Get_Transform()->Get_State(STATE::POSITION));
        m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Stone"), pStone, TIME_CHANNEL::ENEMY);
        });



#pragma endregion

#pragma region IceBreath

    pModel->Register_Event("IceBreath", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        Breath_Start(); m_iBreathCount = 0; m_iBreathRotation = -80.f;
        });
    pModel->Register_Event("IceBreath", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Breath_Loop(); 

    Set_SuperArmor(true);

    ++ m_iBreathCount ;
    m_iBreathRotation += 0.8f;

    if (m_iBreathCount > 15)
        m_iBreathCount = 0;
    else
        return;

    _matrix W = m_pBody->Get_BoneMatrix("Bip001-Head");
    _vector S, Q, T;

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
            XMVectorSet(0.f, 0.f, 0.f, 1.f)
        );

        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Breath"), Q, W.r[3]);

    W = m_pTransformCom->Get_WorldMatrix();
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
            XMVectorSet(0.f, 0.f, 0.f, 1.f)
        );
    
        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }
    XMVECTOR Q_Yaw = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(m_iBreathRotation), 0.f);
    Q = XMQuaternionMultiply(Q, Q_Yaw);

    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Ice"), Q, m_pTransformCom->Get_State(STATE::POSITION)); 
    m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "isIceCreate", true);

    });

    //pModel->Register_Event("IceBreath", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Breath_Start(); }); 
   


    pModel->Register_Event("IceBreath_Melee", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        { 
            Set_SuperArmor(false);
            CBlackBoard* pBB = m_pController->Get_BlackBoard();

            DIRECTION_INFO Info = {};
            Info.iDirFlag = pBB->Get_Value<_uint>(m_strName, "TargetDirection");
            if (Info.Check_Flag(DIRECTION_INFO::F) || Info.Check_Flag(DIRECTION_INFO::L) || Info.Check_Flag(DIRECTION_INFO::R))
            {
                if (Info.Check_Flag(DIRECTION_INFO::B))
                    return;

                _float fDist = pBB->Get_Value<_float>(m_strName, "TargetDist");
                if (fDist <= 410.f)
                {
                    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
                    _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
                    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
                    _float fDist = pBB->Get_Value<_float>(m_strName, "TargetDist");

                    _float fMinSpeed = 40.f;
                    _float fMaxSpeed = 20.f;
                    _float fMinDist = 50.f;
                    _float fMaxDist = 410.f;

                    _float t = (fDist - fMinDist) / (fMaxDist - fMinDist);
                    t = clamp(t, 0.f, 1.f);
                    _float fSpeed = fMinSpeed + (fMaxSpeed - fMinSpeed) * t;
                    CClientInstance::GetInstance()->ActiveCamera_Shaking(3.f, 0.5f);
                    CCreature* pDamagedTarget = static_cast<CCreature*>(m_pTarget);
                    _vector vDir = XMVector3Normalize(vTargetPos - vPosition);
                    pDamagedTarget->KnockBack(vDir, fSpeed, 60.f);
                    pDamagedTarget->Take_Damage(150.f, HITREACTION::KNOCKBACK_STRONG);
                }

            }
        });
    //pModel->Register_Event("IceBreath_Melee", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Breath_Loop(); });
    //pModel->Register_Event("IceBreath_Melee", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Breath_Start(); });
#pragma endregion

#pragma region Groggy
    pModel->Register_Event("GroggyStart", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        {

        });
    pModel->Register_Event("GroggyEnd", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        {

        });

#pragma endregion

#pragma region Dead

    pModel->Register_Event("Dead", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            CBossHp::BOSSMON_UPDATE_DESC Desc;
            Desc.isOpen = false;
            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &Desc);
        });

#pragma endregion


#pragma region GrapCamera

    pModel->Register_Event("GrapCamera", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        CClientInstance::GetInstance()->Yetuga_Holding_Start();
        });
    pModel->Register_Event("GrapCamera", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        CClientInstance::GetInstance()->Yetuga_Holding_End();
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.8f, 0.8f);
        });
#pragma endregion


#pragma region CUTSCENE
    
    pModel->Register_Event("FallDown_Start", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(537.354f, 18.684f, 221.961f, 1.f));
        m_pCharVirCom->Set_Position(XMVectorSet(516.947f, 18.684f, 226.435f, 1.f));

        });


    pModel->Register_Event("FallDown_Start", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        m_isLanding = true;
        XMStoreFloat3(&m_vLandingStartPos, m_pTransformCom->Get_State(STATE::POSITION));

 
        m_vLandingTargetPos = _float3(516.947f, -11.952f, 226.435f);

        m_fLandingHorizontalSpeed = 7.0f;
        m_fLandingVerticalSpeed = 0.f;

        m_fGravity = -700.f; 

        });

    pModel->Register_Event("FallDown_Start", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        m_isLookAt = false;

        });


    // 이펙트 넣어 주세요
    pModel->Register_Event("FallDown_End", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        m_isLookAt = false;

        });


    pModel->Register_Event("FallDown_End", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        m_isLookAt = false;

        });
     
     
    pModel->Register_Event("GameStart", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        {

        m_isLookAt = true;

        });


    pModel->Register_Event("GameStart", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        {

        m_isLookAt = false;
        m_pController->Set_ControllerActivate(true);

        });

    

    /*pModel->Register_Event("GoStraight", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRation = pModel->MakeRatio();
        m_pTransformCom->Go_Straight(fAnimRation);

        });*/



#pragma endregion

    return S_OK;
    
}

HRESULT CYetuga::Ready_AnimEffectEvent(CModel* pModel)
{

    CModel* pAnimModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

    //5연타 

    pModel->Register_Event("Dampsey_First_Snow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Dampsey_Second_Snow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Dampsey_Thrid_Snow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Dampsey_Fourth_Snow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Dampsey_Final_Snow ", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });

    //2HIT
    pModel->Register_Event("2Hit_One_FlowSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });

    pModel->Register_Event("2Hit_Two_FlowSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    //counter attack - 내려찍기
    pModel->Register_Event("CounterAttack_LeftHandSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 0.6f);
        });

    pModel->Register_Event("CounterAttack_RightHandSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 0.6f);
        });

    pModel->Register_Event("CounterAttack_FinalAtackSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        Start_DefaultRadialBlur();
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.5f, 0.8f);
        });


    //Jump Attack ->점프찍기
    pModel->Register_Event("JumpAttack_Land", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        cout << "JumpAttack_Land :: Enter" << endl;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        Start_DefaultRadialBlur();
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 0.6f);
        });

    pModel->Register_Event("JumpAttack_Land", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        cout << "JumpAttack_Land :: Exit" << endl;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    pModel->Register_Event("JumpAttack_RightHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 0.8f);
        });

    //Turn_Attack -> 한번 긁는 거 
    pModel->Register_Event("Turn_Atk_FlowSnow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

#pragma region FOOT
    pModel->Register_Event("ChargeTackle_StampFoot_Run0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        Start_DefaultRadialBlur();
        SFX_Move(1);
        
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        SFX_Move(1);
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        SFX_Move(1);

        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        SFX_Move(1);
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        SFX_Move(1);
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        SFX_Move(1);
        });
 /*   pModel->Register_Event("ChargeTackle_StampFoot_Run6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        SFX_Move(1);
        });*/
    pModel->Register_Event("ChargeTackle_StampFoot_Run7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        SFX_Move(1);
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run8", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        SFX_Move(1);

        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run9", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        SFX_Move(1);
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run10", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        SFX_Move(1);

        });
#pragma endregion


    //pModel->Register_Event("BreathMashTwo ", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), m_pTransformCom->Get_State(STATE::POSITION)));
    //    });
    //
    //pModel->Register_Event("BreathMashTwo ", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
    //    });

    //Armageddon 바위던지기 
    //        
    //        pModel->Register_Event("Armageddon_LeftHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
    //            }); 
    //  

    pModel->Register_Event("Focus2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Focus"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });

    pModel->Register_Event("IceBreath_Ground", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Ice"));
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_icebreath_obj_exp_04 (SFX).wav"), 1.f);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_icebreath_obj_exp_01 (SFX).wav"), 1.f);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_icebreath_obj_exp_05 (SFX).wav"), 1.f);

        _matrix W = m_pTransformCom->Get_WorldMatrix();
        _vector S, Q, T;

        if (!XMMatrixDecompose(&S, &Q, &T, W))
        {
            XMFLOAT4X4 m;
            XMStoreFloat4x4(&m, W);
            _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
            _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
            _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));
            _matrix RotationMatrix(
                r0,
                r1,
                r2,
                XMVectorSet(0.f, 0.f, 0.f, 1.f)
            );

            Q = XMQuaternionRotationMatrix(RotationMatrix);
        }
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Ice_Disappear"), Q, W.r[3]);
        });

    pModel->Register_Event("RushGrab_LandSnow ", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("RushGrab_JumpUp", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("RushGrab_Land ", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), m_pTransformCom->Get_State(STATE::POSITION));
        });

    pModel->Register_Event("JumpBackPress_BackSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("JumpBackPress_StandFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("JumpBackPress_HandSmash", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    //Dodge

    pModel->Register_Event("Dodge_Up", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Dodge_Down", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    pModel->Register_Event("Back_Jump_Up", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("Back_Jump_Down", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), m_pTransformCom->Get_State(STATE::POSITION));
        });
    pModel->Register_Event("Back_Jump_LeftHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Back_Jump_RightHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    pModel->Register_Event("Back_Move_RightHand1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Back_Move_RightHand2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Back_Move_LeftHand1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Back_Move_LeftHand2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Back_Move_LeftFoot1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Back_Move_LeftFoot2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Back_Move_LeftFoot3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Back_Move_RightFoot1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Back_Move_RightFoot2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Back_Move_RightFoot3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Back_Move2_RightFoot1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Back_Move2_RightFoot2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Back_Move2_LeftFoot1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Back_Move2_RightFoot1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Back_Move2_RightFoot2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Back_Move2_LeftHand1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("FastBall_HandSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });

    pModel->Register_Event("Run_LeftFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Run_RightFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Sprint_RightHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Sprint_LeftHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Sprint_LeftFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Sprint_RightFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Roar_R", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Roar_L", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });

    //Armageddon 바위던지기
    pModel->Register_Event("Armageddon_RightHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Armageddon_LeftHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Armageddon_Jump", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //손 중간이어야됨
        //m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), m_pTransformCom->Get_State(STATE::POSITION)); 
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), m_pHoldRock->Get_Transform()->Get_WorldMatrix().r[3]);
        });
    pModel->Register_Event("Armageddon_Jump", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });

    pModel->Register_Event("Walk_FootL0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Walk_FootR0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("Run_FootL0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Run_FootR0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });
    pModel->Register_Event("TurnL_FootL0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("TurnL_FootR0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });

    //pModel->Register_Event("Roar0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    });
    //pModel->Register_Event("Roar0_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
    //    });
    //pModel->Register_Event("Roar0_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //    });

    pModel->Register_Event("A_LHand0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("A_LHand1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("A_LHand2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("A_LHand3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("A_RHand0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("A_RHand1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("A_RHand2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("A_RHand3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    pModel->Register_Event("Roar2_LHand0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Roar2_LHand1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Roar2_LHand2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Roar2_LHand3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Roar2_LHand4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_L")));
        });
    pModel->Register_Event("Roar2_RHand0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Roar2_RHand1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Roar2_RHand2_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Roar2_RHand3_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("Roar2_RHand4_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_DropSnow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    pModel->Register_Event("RushGrab_Foot_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
     
    pModel->Register_Event("Roar2_Ground_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });

    //        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Focus"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));

    
    pModel->Register_Event("Roar0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iFX_ID_Roar = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_pBody->Get_BoneMatrix("Bip001-Head"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });
    pModel->Register_Event("Roar0_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        //m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_iFX_ID_Roar, m_pBody->Get_BoneMatrix("Bip001-Head"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });
    pModel->Register_Event("Roar0_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        //m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_iFX_ID_Roar);
        });
    pModel->Register_Event("Roar1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iFX_ID_Roar = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_pBody->Get_BoneMatrix("Bip001-Head"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });
    pModel->Register_Event("Roar1_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        //m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_iFX_ID_Roar, m_pBody->Get_BoneMatrix("Bip001-Head"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });
    pModel->Register_Event("Roar1_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        //m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_iFX_ID_Roar);
        });
    pModel->Register_Event("A_Roar_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iFX_ID_Roar = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_pBody->Get_BoneMatrix("Bip001-Head"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });
    pModel->Register_Event("A_Roar_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        //m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_iFX_ID_Roar, m_pBody->Get_BoneMatrix("Bip001-Head"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });
    pModel->Register_Event("A_Roar_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        //m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_iFX_ID_Roar);
        });
    pModel->Register_Event("StaminaRecover_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iFX_ID_Roar = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_pBody->Get_BoneMatrix("Bip001-Head"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("StaminaRecover_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_iFX_ID_Roar, m_pBody->Get_BoneMatrix("Bip001-Head"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });
    pModel->Register_Event("StaminaRecover_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        //m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Roar"), m_iFX_ID_Roar);
        });


    pModel->Register_Event("ChargeTackle_StunSmoke", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-Head")));
        });
    pModel->Register_Event("ChargeTackle_StunStamp", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("Run_StunDown_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Weapon_R")));
        });
    pModel->Register_Event("ChargeTackle_FootL0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("ChargeTackle_FootL1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-L-Foot")));
        });
    pModel->Register_Event("ChargeTackle_FootR0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointVFX("Bip001-R-Foot")));
        });

    return S_OK;

}



HRESULT CYetuga::Ready_SFX()
{
    //Mon_vo_yetuga_dmg_weak_01 (SFX).wav
    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;


#pragma region RUN

    pModel->Register_Event("SFX_Run_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_Move(1); });
    pModel->Register_Event("SFX_Run_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { SFX_Move(1); });

#pragma endregion

#pragma region SPRINT

    pModel->Register_Event("SFX_Sprint_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_Move(2); });
    pModel->Register_Event("SFX_Sprint_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { SFX_Move(2); });

#pragma endregion

#pragma region LOCKON

    pModel->Register_Event("SFX_LockOn_R", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_Move(3); });
    pModel->Register_Event("SFX_LockOn_R", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { SFX_Move(3); });


    pModel->Register_Event("SFX_LockOn_L", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SFX_Move(4); });
    pModel->Register_Event("SFX_LockOn_L", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { SFX_Move(4); });

#pragma endregion

#pragma region 2Hit
  
    pModel->Register_Event("SFX_NormalAttack_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_normal_atk1_01 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_NormalAttack_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        { 
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_normal_atk2_02 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_NormalAttackVO", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_normal_atk2_01 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_NormalAttackVO", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_normal_atk2_02 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region BACKMOVE

    pModel->Register_Event("SFX_BackMove1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_back_move1_foley_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_back_move1_01 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_BackMove2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_back_move1_foley_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_back_move1_02 (SFX).wavs"), 1.f);
        });

    pModel->Register_Event("SFX_Dodge", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {

            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_back_move1_foley_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_back_move1_03 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_BackMove2_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_back_move2_foley_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_icebreath_back_jump_01 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region BACKJUMP
    pModel->Register_Event("SFX_BackJump1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_back_jump_foley_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_back_jump_01 (SFX).wav"), 1.f);
        });
#pragma endregion

#pragma region COUNTERATTACK
    pModel->Register_Event("SFX_CounterAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_counter_atk_impact2_01 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_CounterAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_counter_atk_impact1_01 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_CounterAttack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_counter_atk_foley_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_CounterAttackVO", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_counter_atk_01 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region TURNATTACK

    pModel->Register_Event("SFX_TrunAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_turn_atk_01 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_TrunAttack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_turn_atk_whoosh_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_TurnAttackVO", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_turn_atk_01 (SFX).wav"), 1.f);
        });
    
#pragma endregion

#pragma region SIDEMOVELONG

    pModel->Register_Event("SFX_SideMoveLong_L", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_side_move_long_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_side_move_long_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_SideMoveLong_R", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_side_move_long_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_side_move_long_01 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region BACKPRESS

    pModel->Register_Event("SFX_BackPress", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_jump_back_press_01 (SFX).wav"), 1.f);
        });


    pModel->Register_Event("SFX_BackPressVO", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_jump_back_press_1_01 (SFX).wav"), 1.f);

        });

    pModel->Register_Event("SFX_BackPressVO", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_jump_back_press_2_01 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region THROWBALL

    pModel->Register_Event("SFX_ThrowBallEnd", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_fastball_end1_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_icebreath2_01 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region DASHATTACK

    pModel->Register_Event("SFX_DashAttack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_dash_atk_foley_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_DashAttack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_dash_atk_impact_01(SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_DashAttack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dash_atk_01 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region ARMAGEDDON

    pModel->Register_Event("SFX_Armageddon_Start_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_armageddon_start_foley_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_armageddon_start_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_Armageddon_Start_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_armageddon_start_impact1_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_Armageddon_Start_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_armageddon_start_impact2_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_Armageddon_Start_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_armageddon_start_up_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_Armageddon_JumpStart1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_armageddon_jump_start_foley_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_armageddon_jump_start_01 (SFX).wav"), 1.f);
         
        });

    pModel->Register_Event("SFX_Armageddon_JumpStart2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_armageddon_wing_whoosh_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_Armageddon_JumpEnd", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_armageddon_jump_end_foley_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_armageddon_jump_end_01 (SFX).wav"), 1.f);
        });

    pModel->Register_Event("SFX_ArmageddonEndVO", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_armageddon_end_01 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region ICEBREATH

    pModel->Register_Event("SFX_IceBreathEnd", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_icebreath2_01 (SFX).wav"), 1.f);
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "isIceCreate", false);

           
        });
    pModel->Register_Event("SFX_IceBreathVO_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_icebreath1_01 (SFX).wav"), 1.f);

        });

    pModel->Register_Event("SFX_IceBreathVO_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_icebreath1_roar_01 (SFX).wav"), 1.f);

        });

    pModel->Register_Event("SFX_IceBreathVO_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_icebreath_end_01 (SFX).wav"), 1.f);

        });

#pragma endregion

#pragma region ROAR

    pModel->Register_Event("SFX_Roar", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            //m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_roar2_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 8.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_roar2_02 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_Roar", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_roar2_01 (SFX).wav"), 1.f);

        });

#pragma endregion

#pragma region JUMPGRAB
 
    pModel->Register_Event("SFX_GrabSuccess_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_rush_grab_success_up_01 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_GrabSuccess_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_rush_grab_success_jump_01 (SFX).wav"), 1.f);
        });


    pModel->Register_Event("SFX_GrabSuccess_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_rush_grab_success_down_01 (SFX).wav"), 1.f);
        });
    pModel->Register_Event("SFX_GrabSuccess_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_rush_grab_success_end_01 (SFX).wav"), 1.f);
        });

#pragma endregion

#pragma region STAMINARECOVER

    pModel->Register_Event("SFX_StaminaRecover", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_stamina_recover_roar_01 (SFX).wav"), 1.f);
        });

#pragma endregion

    return S_OK;
}


void CYetuga::SFX_Move(_uint iIndex)
{

    _uint iSoundIndex = m_pGameInstance->Rand(0, 6);


    switch (iIndex)
    {
    case 1 :
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_run_foot_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_run_foot_02 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_run_foot_03 (SFX).wav"), 1.f);
        else if (iSoundIndex == 3)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_run_foot_04 (SFX).wav"), 1.f);
        else if (iSoundIndex == 4)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_run_foot_05 (SFX).wav"), 1.f);
        else if (iSoundIndex == 5)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_run_foot_06 (SFX).wav"), 1.f);
        else if (iSoundIndex == 6)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_run_foot_07 (SFX).wav"), 1.f);

        break;

    case 2:
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_sprint_foot_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_sprint_foot_02 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_sprint_foot_03 (SFX).wav"), 1.f);
        else if (iSoundIndex == 3)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_sprint_foot_04 (SFX).wav"), 1.f);
        else if (iSoundIndex == 4)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_sprint_foot_05 (SFX).wav"), 1.f);
        else if (iSoundIndex == 5)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_sprint_foot_06 (SFX).wav"), 1.f);
        else if (iSoundIndex == 6)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_sprint_foot_07 (SFX).wav"), 1.f);

        break;

    case 3:
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_down_01(SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_down_02(SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_down_03(SFX).wav"), 1.f);
        else if (iSoundIndex == 3)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_down_04(SFX).wav"), 1.f);
        else if (iSoundIndex == 4)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_down_05(SFX).wav"), 1.f);
        else if (iSoundIndex == 5)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_down_06(SFX).wav"), 1.f);
        else if (iSoundIndex == 6)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_down_01(SFX).wav"), 1.f);
        break;


    case 4:
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_up_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_up_02 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_up_03 (SFX).wav"), 1.f);
        else if (iSoundIndex == 3)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_up_04 (SFX).wav"), 1.f);
        else if (iSoundIndex == 4)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_up_05 (SFX).wav"), 1.f);
        else if (iSoundIndex == 5)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_up_06 (SFX).wav"), 1.f);
        else if (iSoundIndex == 6)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_lookonmove_foot_l_r_up_02 (SFX).wav"), 1.f);
        break;



    }
}

void CYetuga::SFX_HIT(_uint iHitIndex)
{

    _uint iSoundIndex = m_pGameInstance->Rand(0, 5);

    if (iHitIndex == 0)
    {
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_weak_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_weak_02 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_weak_03 (SFX).wav"), 1.f);

        else if (iSoundIndex == 3)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_weak_04 (SFX).wav"), 1.f);
        else if (iSoundIndex == 4)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_weak_05 (SFX).wav"), 1.f);
        else if (iSoundIndex == 5)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_weak_06 (SFX).wav"), 1.f);

    }
    else if (iHitIndex == 1)
    {
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_nomal_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_nomal_02 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_nomal_03 (SFX).wav"), 1.f);

        else if (iSoundIndex == 3)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_nomal_04 (SFX).wav"), 1.f);
        else if (iSoundIndex == 4)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_nomal_05 (SFX).wav"), 1.f);
        else if (iSoundIndex == 5)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_nomal_06 (SFX).wav"), 1.f);
    }


    else if (iHitIndex == 2)
    {
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_strong_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_strong_02 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_strong_03 (SFX).wav"), 1.f);
        else if (iSoundIndex == 3)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_strong_04 (SFX).wav"), 1.f);
        else if (iSoundIndex == 4)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_strong_05 (SFX).wav"), 1.f);
        else if (iSoundIndex == 5)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dmg_strong_06 (SFX).wav"), 1.f);
    }

}


CYetuga* CYetuga::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CYetuga* pInstance = new CYetuga(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CYetuga"));
    }
    return pInstance;
}

CGameObject* CYetuga::Clone(void* pArg)
{
    CYetuga* pInstance = new CYetuga(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CYetuga"));
    }
    return pInstance;
}

void CYetuga::Free()
{
    Safe_Release(m_pBody);
    Safe_Release(m_pHead);

    if (m_pHoldStone)
        m_pHoldStone->Set_IsDead(true);

    if (m_pHoldRock)
        m_pHoldRock->Set_IsDead(true);

    if (m_pBreath)
        m_pBreath->Set_IsDead(true);

    __super::Free();
}
