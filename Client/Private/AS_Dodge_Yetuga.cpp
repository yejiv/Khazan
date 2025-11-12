#include "AS_Dodge_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_Dodge_Yetuga::CAS_Dodge_Yetuga()
{
}

void CAS_Dodge_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    _uint iRand = static_cast<_uint>(m_pGameInstance->Rand(0.f, 3.f));
    _uint iAnimID = 0;
    HITREACTION eHitreaction = 
        //static_cast<HITREACTION>(m_pGameInstance->Get_BlackBoard()->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));
        static_cast<HITREACTION>(pBB->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));

    if (HITREACTION::KNOCKBACK_STRONG == eHitreaction)
        iAnimID = 21;
    else
    {
        switch (iRand)
        {
        case 0:
            iAnimID = 19;
            break;
        case 1:
            iAnimID = 20;
            break;
        case 2:
            iAnimID = 24;
            break;
        }
    }
   

    pModel->Set_Animation(iAnimID);
   
}

void CAS_Dodge_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        //m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isDodgeFinished", true);
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isDodgeFinished", true);
    }


}

void CAS_Dodge_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Dodge_Yetuga* CAS_Dodge_Yetuga::Create()
{
    return new CAS_Dodge_Yetuga();
}

void CAS_Dodge_Yetuga::Free()
{
    __super::Free();
}
