#include "AS_Dead_Imp_Melee.h"
#include "Imp_Melee.h"
#include "GameInstance.h"
#include "Body_Imp_Melee.h"
#include "BlackBoard.h"
#include "AI_Controller.h"

CAS_Dead_Imp_Melee::CAS_Dead_Imp_Melee()
{
}

void CAS_Dead_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(10);
}

void CAS_Dead_Imp_Melee::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
        //m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pImp->Get_Name(), "isDeadFinished", true);
        pBB->Set_Value<_bool>(pImp->Get_Name(), "isDeadFinished", true);
        pImp->Creature_Release();
        pImp->HPUI_Dead();
    }
}

void CAS_Dead_Imp_Melee::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dead_Imp_Melee* CAS_Dead_Imp_Melee::Create()
{
    return new CAS_Dead_Imp_Melee();
}

void CAS_Dead_Imp_Melee::Free()
{
    __super::Free();
}
