#include "AS_Dead_Imp_Melee.h"
#include "Imp_Melee.h"
#include "GameInstance.h"
#include "Body_Imp_Melee.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "Interaction_Item.h"

CAS_Dead_Imp_Melee::CAS_Dead_Imp_Melee()
{
}

void CAS_Dead_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(10);

    CInteraction_Item* pItem = dynamic_cast<CInteraction_Item*>(m_pGameInstance->Pop_PoolObject(m_pGameInstance->Get_CurrentLevelID(), TEXT("Item")));
    pItem->RandNormal_Item(pOwner->Get_Transform()->Get_State(STATE::POSITION));
    m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_Item"), pItem);


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
        pImp->HPUI_Dead();
    }

    if (pModel->IsFinished())
    {
        pImp->Dissolve_On();
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
