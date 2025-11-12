#include "AS_ThrowBall_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_ThrowBall_Yetuga::CAS_ThrowBall_Yetuga()
{
}

void CAS_ThrowBall_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_AnimationSet("ThrowBall");

}

void CAS_ThrowBall_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    
    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>("Yetuga", "isThrowBallFinished", true);
    }
}

void CAS_ThrowBall_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_ThrowBall_Yetuga* CAS_ThrowBall_Yetuga::Create()
{
    return new CAS_ThrowBall_Yetuga();
}

void CAS_ThrowBall_Yetuga::Free()
{
    __super::Free();
}
