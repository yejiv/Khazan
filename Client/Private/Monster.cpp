#include "Monster.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "ContainerObject.h"
#include "BlackBoard.h"
#include "AI_Controller.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCreature{ pDevice, pContext }
{

}

CMonster::CMonster(const CMonster& Prototype)
    : CCreature{ Prototype }
{

}

void CMonster::Take_Damage(_float fDamage, HITREACTION eHitreaction, _float fValidTime,CGameObject* pGameObject)
{
    m_fCurrentHP -= fDamage;
    m_pController->AI_ApplyDamage(pGameObject,fDamage,ENUM_CLASS(eHitreaction),fValidTime);
}

void CMonster::Consume_Stamina(_float fAmout)
{
    if (m_fCurrentStamina > 0.f)
        m_fCurrentStamina -= fAmout;

    if (m_fCurrentStamina <= 0)
        m_fCurrentStamina = 0.1f;
}

void CMonster::Recovery_Stamina(_float fTimeDelta)
{
    if (!m_isRequestRecoveryStamina)
        return;

    if (m_fCurrentStamina < m_fMaxStamina)
    {
        m_fCurrentStamina += m_fRecoveryPerSec * fTimeDelta;
        if (m_fCurrentStamina > m_fMaxStamina)
            m_fCurrentStamina = m_fMaxStamina;
    }

    if (m_fCurrentStamina >= m_fMaxStamina)
        m_isRequestRecoveryStamina = false;

}

void CMonster::Look_Target()
{
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    m_pTransformCom->LookAt(vTargetPos);
}

void CMonster::Look_Target_Lerp(_float fTimeDleta, _float AnimRatio, _float fTrunSpeed)
{
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    
    _float fResultTurnSpeed = AnimRatio * fTrunSpeed;

    m_pTransformCom->LookAt_Lerp(vTargetPos,fTimeDleta, fResultTurnSpeed);

}

void CMonster::Start_Decel(_float fDuration)
{
    m_vVelocutyTarget = _float3(0.f, 0.f, 0.f);
    m_fDecelTime = fDuration;
    m_fDecelElapsed = 0.f;
    m_isDecelerating = true;
}

void CMonster::Update_Velocity(_float fTimeDelta)
{
    if (m_isDecelerating)
    {
        m_fDecelElapsed += fTimeDelta;

        _float fLerpTime = m_fDecelElapsed / m_fDecelTime;
        if (fLerpTime > 1.f)
            fLerpTime = 1.f;
        else
            fLerpTime = fLerpTime;

        XMStoreFloat3(&m_vVelocity, XMVectorLerp(XMLoadFloat3(&m_vVelocity), XMLoadFloat3(&m_vVelocutyTarget), fLerpTime));

        if (fLerpTime >= 1.f)
            m_isDecelerating = false;
    }
    _vector vRseult = XMVectorScale(XMLoadFloat3(&m_vVelocity),fTimeDelta);
    _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);

    m_pTransformCom->Set_State(STATE::POSITION, vPosition + vRseult);
}



HRESULT CMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster::Initialize_Clone(void* pArg)
{
    MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);


    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    // 이름
    m_strName = pDesc->strName;
    // 타겟
    m_pTarget = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::HEINMACH),TEXT("Layer_Creature_Player"),0);
    if (nullptr == m_pTarget)
        return E_FAIL;
    // 블랙보드에 설정
    CBlackBoard* pBlackBoard = m_pGameInstance->Get_BlackBoard();
    pBlackBoard->Set_Value(m_strName, "Target", m_pTarget);

    return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
    
}

void CMonster::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    //m_pRigidBodyCom->Sync_Update(m_pTransformCom);


}

void CMonster::Late_Update(_float fTimeDelta)
{

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

}

HRESULT CMonster::Render()
{
  
    return S_OK;
}

//HRESULT CMonster::Ready_Components()
//{
//    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
//        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
//        return E_FAIL;
//
//    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fiona"),
//        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
//        return E_FAIL;
//
//
//   /* CRigidBody::RIGID_BOXSHAPE_DESC RigidDesc{};
//    RigidDesc.vExtent = { 0.5f, 0.5f, 0.5f };
//    RigidDesc.bIsTrigger = false;
//    RigidDesc.bStartActive = true;
//    RigidDesc.eMotion = EMotionType::Static;
//    RigidDesc.eQuality = EMotionQuality::Discrete;
//    RigidDesc.eShapeType = SHAPE::BOX;
//    RigidDesc.fFriction = 0.8f;
//    RigidDesc.fMass = 1.0f;
//    RigidDesc.fRestitution = 0.0f;
//    RigidDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
//    _float3 vPos{};
//    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
//    _float4 vQuat{};
//    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
//    RigidDesc.vPos = vPos;
//    RigidDesc.vQuat = vQuat;
//
//    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
//        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidDesc)))
//        return E_FAIL;*/
//
//
//    return S_OK;
//}


void CMonster::Free()
{
    __super::Free();

    Safe_Release(m_pController);
    //Safe_Release(m_pRigidBodyCom);
}
