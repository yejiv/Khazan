#include "Viper.h"
#include "ClientInstance.h"
#include "GameInstance.h"
#include "AI_Controller_Viper.h"
#include "BlackBoard.h"
#include "CharacterVirtual.h"
#include "BossHp.h"
#include "Body_Viper.h"


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
    //CBlackBoard* pBB = m_pController->Get_BlackBoard();

  /*  if (pBB->Get_Value<_bool>(m_strName, "isDetected"))
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

    if (m_isLookAt)
    {
        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        if (nullptr == pModel)
            return;
        _float fRatio = pModel->MakeRatio();
        Look_Target_Lerp(fTimeDelta, fRatio, m_fTurnSpeed);
    }

    __super::Update(fTimeDelta);

    m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Lacirma_ExpGained");

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

void CViper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
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

void CViper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CViper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
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

    return S_OK;
}

HRESULT CViper::Ready_Projectiles()
{
   
    return S_OK;
}

HRESULT CViper::Ready_AnimEvent()
{
    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;
    pModel->Register_Event("WalkStepEvent", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iStepCnt = m_pController->Get_BlackBoard()->Get_Value<_uint>(m_strName,"WalkStepCount");
            m_pController->Get_BlackBoard()->Set_Value<_uint>(m_strName,"WalkStepCount", iStepCnt + 1);
            cout << "Step1111111111111111111111111111111111111111" << endl;
        });

  
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
    __super::Free();
}
