#include "AS_Idle_Imp_Range.h"
#include "Imp_Range.h"
#include "GameInstance.h"
#include "BlackBoard.h"

CAS_Idle_Imp_Range::CAS_Idle_Imp_Range()
{
  
}

void CAS_Idle_Imp_Range::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(0);

}

void CAS_Idle_Imp_Range::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
    
    if (pModel->Play_Animation(fTimeDelta))
    {
        int a = 10;
    }

}

void CAS_Idle_Imp_Range::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Idle_Imp_Range* CAS_Idle_Imp_Range::Create()
{
    return new CAS_Idle_Imp_Range();
}

void CAS_Idle_Imp_Range::Free()
{
    __super::Free();
}
