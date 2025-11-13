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
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(513.f, -11.f, 225.f,1.f));

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;


    m_pController = CAI_Controller_Yetuga::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    if(nullptr != m_pController)
        m_pController->Get_BlackBoard()->Set_Value(m_strName, "Target", m_pTarget);


    m_fRecoveryPerSec = 5.f;

    return S_OK;
}

void CYetuga::Priority_Update(_float fTimeDelta)
{
    CBlackBoard* pBB = m_pController->Get_BlackBoard();
    if (m_fCurrentHP <= 0.f)
    {
        pBB->Set_Value<_bool>(m_strName, "isDead", true);

    }

    if (pBB->Get_Value<_bool>(m_strName, "isDetected"))
    {
        CBossHp::BOSSMON_UPDATE_DESC HPDesc{};
        HPDesc.isOpen = true;
        HPDesc.pHpMaxValue = &m_fMaxHP;
        HPDesc.pHpValue = &m_fCurrentHP;
        HPDesc.pStaminaMaxValue = &m_fMaxStamina;
        HPDesc.pStaminaCulValue = &m_fCurrentStamina;

        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &HPDesc);
    }
   
    CContainerObject::Priority_Update(fTimeDelta);
}

void CYetuga::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);

    if (m_isLookAt)
    {
        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        if (nullptr == pModel)
            return;
        _float fRatio = pModel->MakeRatio();
        Look_Target_Lerp(fTimeDelta,fRatio,m_fTurnSpeed);
    }
        

    __super::Update(fTimeDelta);

    m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Body_ExpGained");

#ifdef _DEBUG
    //m_pGameInstance->Set_DrawFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK));
#endif // _DEBUG

    // ========== Radial Blur ==========
    // 예투가 업데이트
    //  m_pGameInstance->Set_RadialBlurCenter(m_pTransformCom->Get_State(STATE::POSITION));

    // 애니메이션 노티파이에 추가할 레디얼 블러 애니메이션 함수
    //  RADIAL_BLUR_DESC Desc{};
    //  Desc.vCenterUV = _float2(0.5f, 0.5f);
    //  Desc.fSampleRadius = 0.05f;
    //  Desc.vMaskRadius = _float2(0.f, 0.4f);
    //  Desc.fExponent = 1.f;
    //  Desc.iNumSamples = 16;
    //  Desc.fAttenuation = 0.1f;
    //  Desc.fStrength = 0.f;
    //  m_pGameInstance->Start_RadialBlur(2.f, _float2(0.3, 1.f), m_RadialBlurDesc);
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

void CYetuga::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
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



       

        // Decal
        
        //  _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
        //  _vector vTargetPos = XMVectorSet(vPos.m128_f32[0], vPos.m128_f32[1] - 5.f, vPos.m128_f32[2], 1.f);
        //  
        //  _float fFraction;
        //  _float4 vRayHitPos;
        //  _float3 outNormal;
        //  
        //  if (m_pGameInstance->RayCast(
        //      _float3(vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]),
        //      _float3(vTargetPos.m128_f32[0], vTargetPos.m128_f32[1], vTargetPos.m128_f32[2]),
        //      fFraction,
        //      vRayHitPos,
        //      &outNormal
        //  ))
        //  {
        //      DECAL_DESC Desc{};
        //      Desc.fLifeTime = 5.f;
        //      Desc.vFadeTime = _float2(0.5f, 0.5f);
        //      Desc.eType = DECALTYPE::CIRCLE;
        //      Desc.vPosition = _float3(vRayHitPos.x, vRayHitPos.y, vRayHitPos.z);
        //      Desc.vScale = _float3(40.f, 40.f, 40.f);
        //      Desc.vColor = _float3(0.2745f, 0.08f, 0.08f);
        //  
        //      m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Decal"), Desc);
        //  }

        _vector vDecalPos = m_pTransformCom->Get_State(STATE::POSITION);
        _float fOffset = 2.f;
        _float fPosX = XMVectorGetX(vDecalPos);
        _float fPosZ = XMVectorGetZ(vDecalPos);
        vDecalPos = XMVectorSetX(vDecalPos, m_pGameInstance->Rand(fPosX - fOffset, fPosX + fOffset));
        vDecalPos = XMVectorSetZ(vDecalPos, m_pGameInstance->Rand(fPosZ - fOffset, fPosZ + fOffset));
        DECAL_DESC Desc{};
        Desc.fLifeTime = 8.f;
        Desc.vFadeTime = _float2(0.2f, 0.2f);
        Desc.eType = static_cast<DECALTYPE>(m_pGameInstance->Rand(0.f, static_cast<_float>(DECALTYPE::END)));
        XMStoreFloat3(&Desc.vPosition, vDecalPos);
        Desc.vScale = _float3(
            m_pGameInstance->Rand(4.f, 8.f),
            2.f, 
            m_pGameInstance->Rand(4.f, 8.f)
            );
        Desc.vColor = _float3(0.2745f, 0.08f, 0.08f);
        
        m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Decal"), Desc);
    }
}

void CYetuga::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CYetuga::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

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

    Safe_Release(m_pHoldStone);
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
    Safe_AddRef(m_pBreath);

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
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 4.1f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 2.f;
    tCharVirDesc.fHeight = 4.f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fMass = 10.f;
    tCharVirDesc.fMaxStrength = 0.f;
    tCharVirDesc.fPredictiveContactDistance = 0.3f;
    tCharVirDesc.iMaxConstraintIterations = 20;
    tCharVirDesc.fCollisionTolerance = 0.03f;
    tCharVirDesc.fPenetrationRecoverySpeed = 1.7f;

    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;
   
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
       RockDesc.fRotationPerSec = 180.f;

       m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Yetuga_Rock"),
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
        m_pBody->Set_OnAttackCollision(true);
        m_isLookAt = true;
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
        });

    pModel->Register_Event("Side_L_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(false);
        m_isLookAt = false;
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
        });

    pModel->Register_Event("Side_R_Attack", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_fTurnSpeed = 40.f;
        m_pBody->Set_OnAttackCollision(false);
        m_isLookAt = false;
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

        
        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_First", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

       
        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_First", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });


    pModel->Register_Event("Dampsey_Second", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        
        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_Second", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        

        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_Second", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });



    pModel->Register_Event("Dampsey_Third", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
       

        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_Third", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
       

        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_Third", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });


    pModel->Register_Event("Dampsey_Forth", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_Forth", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_Forth", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });


    pModel->Register_Event("Dampsey_Final", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        m_isLookAt = true;
        m_pBody->Set_OnAttackCollision(true);

        });
    pModel->Register_Event("Dampsey_Final", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        m_isLookAt = false;
        m_pBody->Set_OnAttackCollision(false);

        });

    pModel->Register_Event("Dampsey_Final", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {

        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        _float fAnimRatio = pModel->MakeRatio();
        CheckMinDistanceWithPlayer(5.f, fAnimRatio);

        });



    pModel->Register_Event("Dampsey_After", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        m_isLookAt = true;

        });
    pModel->Register_Event("Dampsey_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isLookAt = false;

        });




#pragma endregion

#pragma region BACKSMASH

    pModel->Register_Event("BackSmashOne", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() 
        { 
            m_pBody->Set_OnAttackCollision(true);
        });
    pModel->Register_Event("BackSmashOne", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() 
        { 
            m_pBody->Set_OnAttackCollision(false);
        });

    pModel->Register_Event("BackSmashTwo", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pBody->Set_OnAttackCollision(true);
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

   /* "Grab_Hand",
        "Grab_Hold"*/


    pModel->Register_Event("Jump_Grab_Jump", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", true);
            m_pBody->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("Jump_Grab_Jump", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pController->Get_BlackBoard()->Set_Value<_bool>(m_strName, "JumpNotify", false);
         

        });

    pModel->Register_Event("Grab_Hand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_isGhost = true;
            m_pBody->Set_OnAttackCollision(false);

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

    //Grab_After
    pModel->Register_Event("Grab_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            // 타겟 풀어주기
            m_isGhost = false;
            CCreature* pTarget = static_cast<CCreature*>(m_pTarget);
            pTarget->Take_Damage(5.f,HITREACTION::GRAB_FINISHED,nullptr);
            
        });


#pragma endregion

#pragma region RUSH

    pModel->Register_Event("RushCheck", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pHead->Set_OnAttackCollision(true);
            m_pBody->Set_AttackCollision_Back(true);
            m_isGhost = true;
        });

    pModel->Register_Event("RushCheck", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pHead->Set_OnAttackCollision(false);
            m_pBody->Set_AttackCollision_Back(false);
            m_isGhost = false;
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

HRESULT CYetuga::Ready_AnimEffectEvent(CModel* pModel)
{
    //5연타 

    pModel->Register_Event("Dampsey_First_Snow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
        });
    pModel->Register_Event("Dampsey_Second_Snow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_R")));
        });
    pModel->Register_Event("Dampsey_Thrid_Snow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
        });
    pModel->Register_Event("Dampsey_Fourth_Snow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_R")));
        });
    pModel->Register_Event("Dampsey_Final_Snow ", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
        });

    //2HIT
    pModel->Register_Event("2Hit_One_FlowSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
        });

    pModel->Register_Event("2Hit_Two_FlowSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_R")));
        });

    //counter attack - 내려찍기
    pModel->Register_Event("CounterAttack_LeftHandSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
        });

    pModel->Register_Event("CounterAttack_RightHandSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
        });

    pModel->Register_Event("CounterAttack_FinalAtackSnow", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
        });


    //Jump Attack ->점프찍기
    pModel->Register_Event("JumpAttack_Land", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        cout << "JumpAttack_Land :: Enter" << endl;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_R")));
        });

    pModel->Register_Event("JumpAttack_Land", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        cout << "JumpAttack_Land :: Exit" << endl;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_R")));
        });

    pModel->Register_Event("JumpAttack_RightHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_R")));
        });

    //Turn_Attack -> 한번 긁는 거 
    pModel->Register_Event("Turn_Atk_FlowSnow", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_R")));
        });

    //charge attack - 달리기 전에 왼발 구르기
    pModel->Register_Event("ChargeTackle _StampFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-L-Foot")));
        }); 

#pragma region FOOT
    pModel->Register_Event("ChargeTackle_StampFoot_Run0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-R-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-L-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-R-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-L-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-R-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-L-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-R-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-L-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run8", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-R-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run9", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-L-Foot")));
        });
    pModel->Register_Event("ChargeTackle_StampFoot_Run10", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-R-Foot")));
        });
#pragma endregion

    //charge attack - 달리기 전에 왼발 구르기
    pModel->Register_Event("ChargeTackle _StampFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Roar_L"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-L-Foot")));
        });

    //charge attack - 달리기 전에 왼발 구르기
    pModel->Register_Event("ChargeTackle _StampFoot", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Roar_R"), XMLoadFloat4(m_pBody->Get_BonePointEX("Bip001-R-Foot")));
        });

    //pModel->Register_Event("BreathMashTwo ", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), m_pTransformCom->Get_State(STATE::POSITION)));
    //    });
    //
    //pModel->Register_Event("BreathMashTwo ", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
    //    });

    //Armageddon 바위던지기
    //        pModel->Register_Event("Armageddon_RightHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
    //            });
    //        
    //        pModel->Register_Event("Armageddon_LeftHand", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), XMLoadFloat4(m_pBody->Get_BonePointEX("Weapon_L")));
    //            }); 
    //        


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
    Safe_Release(m_pHead);
    __super::Free();
}
