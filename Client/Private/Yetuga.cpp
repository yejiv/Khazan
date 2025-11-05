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
    //m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Body_ExpGained");
    //m_vLockOnPosition = m_pBody->Get_BonePointEX("Holding");

    return m_vLockOnPosition;
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

    //-4 0 27
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(517.f, -12.f, 241.f,1.f));

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;


    m_pController = CAI_Controller_Yetuga::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    CBossHp::BOSSMON_UPDATE_DESC HPDesc{};
    HPDesc.isOpen = true;
    HPDesc.pHpMaxValue = &m_fMaxHP;
    HPDesc.pHpValue = &m_fCurrentHP;
    HPDesc.pStaminaMaxValue = &m_fMaxStamina;
    HPDesc.pStaminaCulValue = &m_fCurrentStamina;

    CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"),&HPDesc);

    return S_OK;
}

void CYetuga::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CYetuga::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);

    m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Body_ExpGained");

    __super::Update(fTimeDelta);
}

void CYetuga::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

    CContainerObject::Late_Update(fTimeDelta);

}

HRESULT CYetuga::Render()
{
    return S_OK;
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
    Safe_AddRef(m_pHoldStone);

    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
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
}

void CYetuga::Hold_Stone()
{
    if (nullptr == m_pHoldStone)
        return;

    _float3 vSpawnPoint = m_pBody->Get_BonePoint("Weapon_L");
    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pHoldStone->Set_SpawnDir(vNormalize);
    m_pHoldStone->Set_SpanwPoint(vSpawnPoint);
    m_pHoldStone->Reset();

}

void CYetuga::Throw_Stone()
{
    CTransform* pTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vOffset = XMVectorSet(0.f,-10.f,0.f,0.f);
    _vector vTargetLoc = pTransform->Get_State(STATE::POSITION) - vOffset;

    if (m_pHoldStone == nullptr)
        return;
    _float3 vSpawnPoint = m_pBody->Get_BonePoint("Weapon_L");
    _float3 vTargetDir = (m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir"));
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize,vTempVec);
    m_pHoldStone->Set_SpanwPoint(vSpawnPoint);
    m_pHoldStone->Set_SpawnDir(vNormalize);
    m_pHoldStone->Reset();
    m_pHoldStone->Set_IsActive(true);
    m_pTransformCom->LookAt(vTargetLoc);
    
    CModel* pModel = static_cast<CModel*>(m_pHoldStone->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(1);
   
    Safe_Release(m_pHoldStone);

}

void CYetuga::Grab_Check_Begin()
{
    // 충돌키고
    // 충돌됬으면
    _float3 vTemp = m_pBody->Get_BonePoint("Holding");
    _float4(vTemp.x,vTemp.y,vTemp.z,1.f);
    // 뼈 포지션을 넣어주고
}

void CYetuga::Grab_Check_End()
{
    // 충돌을 꺼준다.
}

void CYetuga::Pick_Rock()
{
     _float3 vLHTemp = m_pBody->Get_BonePoint("Weapon_L");
    _float3 vRHTemp = m_pBody->Get_BonePoint("Weapon_R");

    _vector vLH = XMLoadFloat3(&vLHTemp);
    _vector vRH = XMLoadFloat3(&vRHTemp);
    _vector vCenterOffset = XMVectorSet(0.f, -0.7f, 0.f, 0.f);
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
    Safe_AddRef(m_pHoldRock);

    m_isRockPlay = true;

    // 타겟 방향 가져오기
    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
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
    /*if (nullptr == m_pHoldRock)
        return;

    // 뼈 위치 가져오기
    _float3 vLHTemp = m_pBody->Get_BonePoint("Weapon_L");
    _float3 vRHTemp = m_pBody->Get_BonePoint("Weapon_L");

    _vector vLH = XMLoadFloat3(&vLHTemp);
    _vector vRH = XMLoadFloat3(&vRHTemp);

    _vector vSpawnTemp = (vLH + vRH) * 0.5f;
    _float3 vSpawnPoint = {};

    XMStoreFloat3(&vSpawnPoint, vSpawnTemp);

    // 오프셋 적용 (월드 기준)
    XMStoreFloat3(&vSpawnPoint, XMLoadFloat3(&vSpawnPoint));

    // 위치와 방향 갱신
    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pHoldRock->Set_SpawnDir(vNormalize);
    m_pHoldRock->Set_SpanwPoint(vSpawnPoint);
    
    m_pHoldRock->Reset();*/


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

    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
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
    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pHoldRock->Set_SpawnDir(vNormalize);
    m_pHoldRock->Set_SpanwPoint(vSpawnPoint);

    m_pHoldRock->Reset();

    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    CBlackBoard* BB = m_pGameInstance->Get_BlackBoard();
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vTargetLoc = pTargetTransform->Get_State(STATE::POSITION);
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


        _vector vNewPos = XMVectorLerp(vPosition, vGoalPos, fAnimRatio - 0.5f);
        m_pTransformCom->Set_State(STATE::POSITION, vNewPos);
    }
    else
    {
        _vector vDir = XMLoadFloat3(&vTempDir);
        _vector vStopPos = vTargetPos - vDir * fLimit;
        m_pTransformCom->Set_State(STATE::POSITION, vStopPos);
    }

    m_pTransformCom->Set_State(STATE::POSITION, vTargetLoc);

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
    Safe_AddRef(m_pBreath);

    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
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

    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pBreath->Set_SpawnDir(vNormalize);
    m_pBreath->Set_SpanwPoint(vSpawnPoint);

    m_pBreath->Reset();

   /* _float3 dbgPos;
    XMStoreFloat3(&dbgPos, vPosition);
    _float3 WorldPos;
    _vector vTemp = m_pTransformCom->Get_State(STATE::POSITION);
    XMStoreFloat3(&WorldPos, vTemp);
    std::cout << "Head Pos : " << dbgPos.x << " , " << dbgPos.y << " , " << dbgPos.z << std::endl;
    std::cout << "WorldPos : " << WorldPos.x << " , " << WorldPos.y << " , " << dbgPos.z << std::endl;
    std::cout << "Spawn Pos : " << vSpawnPoint.x << " , " << vSpawnPoint.y << " , " << vSpawnPoint.z << std::endl;*/

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
    tCharVirDesc.vShapeOffset = _float3(0.f, 2.5f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 2.5f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;


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


    return S_OK;
}

HRESULT CYetuga::Ready_Projectiles()
{
    CProjectile_Yetuga::PROJECTILE_DESC Desc{};
    Desc.fDamage = 10.f;
    Desc.fSpeedPerSec = 150.f;
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
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Rock"), &RockDesc, 2);


       CProjectile_Breath_Yetuga::PROJECTILE_DESC BreathDesc{};
       BreathDesc.fDamage = 5.f;
       BreathDesc.fSpeedPerSec = 5.f;
       BreathDesc.fLifeTime = 10.f;
       RockDesc.fRotationPerSec = 180.f;

       m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Yetuga_Breath"),
           ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Breath"), &BreathDesc ,2);

    return S_OK;
}

HRESULT CYetuga::Ready_AnimEvent()
{
    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

#pragma region ThrowRock

    pModel->Register_Event("ThrowBall", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Pick_Stone(); });
    pModel->Register_Event("ThrowBall", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Throw_Stone(); });
    pModel->Register_Event("ThrowBall", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Hold_Stone(); });

#pragma endregion


#pragma region 2HIT

    pModel->Register_Event("2HitOne", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Look_Target(); });
    //pModel->Register_Event("2HitOne", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { /*충돌끄고*/});
    //pModel->Register_Event("2HitOne", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { });

    //pModel->Register_Event("2HitTwo", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {  });
    //pModel->Register_Event("2HitTwo", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { /*타겟 풀어주기*/});
    //pModel->Register_Event("2HitTwo", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { /*타겟 풀어주기*/});
#pragma endregion

#pragma region BACKSMASH
    //pModel->Register_Event("BackSmashOne", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { /*충돌 키고*/ });
    //pModel->Register_Event("BackSmashOne", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { /*충돌끄고*/});
    //pModel->Register_Event("BackSmashOne", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { });

    //pModel->Register_Event("BackSmashTwo", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 충돌 키고 });
    //pModel->Register_Event("BackSmashTwo", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { /*충돌 끄고*/});
    //pModel->Register_Event("BackSmashTwo", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { /*타겟 풀어주기*/});
#pragma endregion


#pragma region JumpAttack
    pModel->Register_Event("JumpAttack", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        { 
            m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", true);
        });
    pModel->Register_Event("JumpAttack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        {
            m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", false);

        });
    //pModel->Register_Event("JumpAttack", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Look_Target(); });
#pragma endregion

#pragma region JumpGrab

    pModel->Register_Event("GrabCheck", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        {
            m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", true);

        });
    pModel->Register_Event("GrabCheck", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        { 
            m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", false);

        });
    //pModel->Register_Event("GrabCheck", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Hold_Rock(); });

    pModel->Register_Event("TargetFree", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        {
            // 타겟 풀어주기
        });

#pragma endregion


#pragma region DempeyRoll

    pModel->Register_Event("SetOne", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Look_Target(); });
    pModel->Register_Event("SetOne", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Look_Target(); });
    //pModel->Register_Event("SetOne", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { /*타겟 풀어주기*/});

    pModel->Register_Event("SetTwo", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Look_Target(); });
    pModel->Register_Event("SetTwo", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Look_Target(); });
    //pModel->Register_Event("SetTwo", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { /*타겟 풀어주기*/});

    // 패리 시 StrongAnimation 으로 
    pModel->Register_Event("SetThree", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Look_Target(); });
    pModel->Register_Event("SetThree", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Look_Target(); });
    //pModel->Register_Event("SetThree", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { /*타겟 풀어주기*/});

#pragma endregion


#pragma region Amageddon
    // 돌을 풀에 서 꺼낸다.
    pModel->Register_Event("AMG_RockEvent", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Pick_Rock(); });
    // 대미지를 주고 충돌끄고 풀로 보낸다.
    pModel->Register_Event("AMG_RockEvent", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { m_isRockPlay = false; });
    // Hold한다.
    pModel->Register_Event("AMG_RockEvent", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Hold_Rock(); });

    // 점프 시작
    pModel->Register_Event("AMG_JumpEvent", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { });
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


    pModel->Register_Event("AMG_SmashEvent", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Smash(); });

#pragma endregion

#pragma region IceBreath
    pModel->Register_Event("IceBreath", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Breath_Start(); });
    pModel->Register_Event("IceBreath", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Breath_Loop(); });
    //pModel->Register_Event("IceBreath", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Breath_Start(); });


    /*pModel->Register_Event("IceBreath_Melee", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Breath_Start(); });
    pModel->Register_Event("IceBreath_Melee", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { Breath_Loop(); });*/
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


    return S_OK;
    
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

    __super::Free();
}
