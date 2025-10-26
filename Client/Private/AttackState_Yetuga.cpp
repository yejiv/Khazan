#include "AttackState_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"

CAttackState_Yetuga::CAttackState_Yetuga()
{
}

void CAttackState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(0);
    //pModel->Set_AnimationLoop(false);
}

void CAttackState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
    if (pModel->Play_Animation(fTimeDelta))
    {
        //cout << "AttackFinished" << endl;
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>("Yetuga", "isAttackFinished", true);
    }

}

void CAttackState_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAttackState_Yetuga* CAttackState_Yetuga::Create()
{
    return new CAttackState_Yetuga();
}

void CAttackState_Yetuga::Free()
{
    __super::Free();
}
