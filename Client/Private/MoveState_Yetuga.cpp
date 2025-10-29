#include "MoveState_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Yetuga.h"

CMoveState_Yetuga::CMoveState_Yetuga()
{
}

void CMoveState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);

    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(1);
    pModel->Set_AnimationLoop(false);
}

void CMoveState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);

    CBlackBoard* pBB = m_pGameInstance->Get_BlackBoard();
    pYetuga->Get_Controller()->
        AI_MoveTo(pOwner, pBB->Get_Value<CGameObject*>("Yetuga", "Target"),pBB->Get_Value<_float>("Yetuga","AttackRange"), fTimeDelta);

    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    if (pModel->Play_Animation(fTimeDelta))
    {
        int a = 10;
    }

}

void CMoveState_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CMoveState_Yetuga* CMoveState_Yetuga::Create()
{
    return new CMoveState_Yetuga();
}

void CMoveState_Yetuga::Free()
{
    __super::Free();
}
