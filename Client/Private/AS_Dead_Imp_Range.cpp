#include "AS_Dead_Imp_Range.h"
#include "Imp_Range.h"
#include "GameInstance.h"
#include "Body_Imp_Range.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "Interaction_Item.h"


CAS_Dead_Imp_Range::CAS_Dead_Imp_Range()
{
}

void CAS_Dead_Imp_Range::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));


    pImp->Cast_Failed();
    pModel->Set_Animation(28);

}

void CAS_Dead_Imp_Range::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pImp->Get_Name(), "isDeadFinished", true);
        pImp->Get_Controller()->Set_ControllerActivate(false);        
        pImp->HPUI_Dead();
    }

    if (pModel->IsFinished())
    {
        pImp->Dissolve_On();
    }


}

void CAS_Dead_Imp_Range::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Dead_Imp_Range* CAS_Dead_Imp_Range::Create()
{
    return new CAS_Dead_Imp_Range();
}

void CAS_Dead_Imp_Range::Free()
{
    __super::Free();
}
