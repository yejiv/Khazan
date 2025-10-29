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

void CMonster::Take_Damage(CCreature* pGameObject, _float fDamage)
{
    m_fCurrentHP -= fDamage;
    m_pController->AI_ApplyDamage(pGameObject,fDamage);
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
    m_pTarget = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::STAGE1), TEXT("Layer_Creature_Test"), 0);
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
    //m_pRigidBodyCom->Sync_Update(m_pTransformCom);
}

void CMonster::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
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
//    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Model_Fiona"),
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
