#include "AS_FrontAttack_Gomdol.h"
#include "AI_Controller.h"
#include "Gomdol.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Gomdol.h"
#include "FSM_Gomdol.h"

CAS_FrontAttack_Gomdol::CAS_FrontAttack_Gomdol()
{
}

void CAS_FrontAttack_Gomdol::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CGomdol* pYetuga = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    pModel->Set_Animation(15);
}

void CAS_FrontAttack_Gomdol::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CGomdol* pYetuga = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isFrontAttackFinished", true);
        pFSM->Change_State(ENUM_CLASS(GOMDOL_STATE::IDLE), pOwner);

    }
}

void CAS_FrontAttack_Gomdol::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_FrontAttack_Gomdol* CAS_FrontAttack_Gomdol::Create()
{
    return new CAS_FrontAttack_Gomdol();
}

void CAS_FrontAttack_Gomdol::Free()
{
    __super::Free();
}
