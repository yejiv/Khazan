#include "IdleState_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Yetuga.h"

CIdleState_Yetuga::CIdleState_Yetuga()
{
}

void CIdleState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(0);
    pModel->Set_AnimationLoop(true);

}

void CIdleState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    if (pModel->Play_Animation(fTimeDelta))
    {
        int a = 10;
    }
}

void CIdleState_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CIdleState_Yetuga* CIdleState_Yetuga::Create()
{
    return new CIdleState_Yetuga();
}

void CIdleState_Yetuga::Free()
{
    __super::Free();
}
