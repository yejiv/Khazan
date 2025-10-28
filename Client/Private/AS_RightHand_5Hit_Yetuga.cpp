#include "AS_RightHand_5Hit_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"

CAS_RightHand_5Hit_Yetuga::CAS_RightHand_5Hit_Yetuga()
{
}

void CAS_RightHand_5Hit_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(1);
}

void CAS_RightHand_5Hit_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>("Yetuga", "isAttackFinished", true);
    }
}

void CAS_RightHand_5Hit_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_RightHand_5Hit_Yetuga* CAS_RightHand_5Hit_Yetuga::Create()
{
    return new CAS_RightHand_5Hit_Yetuga();
}

void CAS_RightHand_5Hit_Yetuga::Free()
{
    __super::Free();
}
