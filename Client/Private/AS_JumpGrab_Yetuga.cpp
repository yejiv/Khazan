#include "AS_JumpGrab_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_JumpGrab_Yetuga::CAS_JumpGrab_Yetuga()
{
}

void CAS_JumpGrab_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_BlackBoard()->
        Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target")->Get_Component(TEXT("Com_Transform")));
    _vector vTargetLoc = pTargetTransform->Get_State(STATE::POSITION);

    pTransform->LookAt(vTargetLoc);


    pModel->Set_Animation(57);

}

void CAS_JumpGrab_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    _bool isGrabbed = m_pGameInstance->Get_BlackBoard()->Get_Value<_bool>(pYetuga->Get_Name(), "isGrabbed");

    CBlackBoard* pBB = m_pGameInstance->Get_BlackBoard();

    if (pBB->Get_Value<_bool>(pYetuga->Get_Name(), "JumpNotify"))
    {
        pYetuga->Get_Controller()->
            AI_MoveTo(pOwner,
                pBB->Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target"),
                pBB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange"),
                10,
                fTimeDelta);
    }

    if (isGrabbed && !m_isGrabbed)
    {
        pModel->Set_Animation(58);
        m_isGrabbed = true;
    }

    if (m_isGrabbed)
    {
       /* m_vGrabPoint = pYetuga->Get_Body()->Get_BonePoint("Holding");  
        CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target");
        CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
        pTargetTransform->Set_State(STATE::POSITION, XMLoadFloat3(&m_vGrabPoint));*/
    }

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isJumpGrabFinished", true);
    }

}

void CAS_JumpGrab_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isGrabbed", false);
    m_isGrabbed = false;
}

void CAS_JumpGrab_Yetuga::OnCollision(COLLISION_DESC* pDesc)
{
   CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
   m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>("Yetuga", "isGrabbed", true);
 
}

CAS_JumpGrab_Yetuga* CAS_JumpGrab_Yetuga::Create()
{
    return new CAS_JumpGrab_Yetuga();
}

void CAS_JumpGrab_Yetuga::Free()
{
    __super::Free();
}
