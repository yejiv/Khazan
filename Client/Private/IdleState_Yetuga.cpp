#include "IdleState_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"

CIdleState_Yetuga::CIdleState_Yetuga()
{
}

void CIdleState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(3, true);

}

void CIdleState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
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
