#include "AS_Dead_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"
#include "ClientInstance.h"


CAS_Dead_Yetuga::CAS_Dead_Yetuga()
{
}

void CAS_Dead_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    m_pGameInstance->Emit_Event< EVENT_ANNOUNCE_RESULT>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_RESULT), {});
    pModel->Set_Animation(94);

}

void CAS_Dead_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {  
        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        // pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isDeadFinished", true);
        pYetuga->Creature_Release();
    }

}

void CAS_Dead_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Dead_Yetuga* CAS_Dead_Yetuga::Create()
{
    return new CAS_Dead_Yetuga();
}

void CAS_Dead_Yetuga::Free()
{
    __super::Free();
}
