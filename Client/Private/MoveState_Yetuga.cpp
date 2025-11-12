#include "MoveState_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Yetuga.h"

CMoveState_Yetuga::CMoveState_Yetuga()
{
}

void CMoveState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    
    CYetuga::MONSTER_INFO Info{};
    Info.iStateFlag = pBB->Get_Value<_uint>(pYetuga->Get_Name(), "iMovementFlag");
    m_iPrevMovementFlag = Info.iStateFlag;
    if (Info.iStateFlag == Info.WALK)
    {
        m_fSpeedPerSec = pTransform->Get_SpeedPerSec();
        pModel->Set_Animation(1);

    }
    else if (Info.iStateFlag == Info.RUN)
    {
        m_fSpeedPerSec = pBB->Get_Value<_float>(pYetuga->Get_Name(), "RunSpeed");
        pModel->Set_Animation(6);

    }
    else if (Info.iStateFlag == Info.SPRINT)
    {
        m_fSpeedPerSec = pBB->Get_Value<_float>(pYetuga->Get_Name(), "SprintSpeed");
        pModel->Set_Animation(7);

    }  
}

void CMoveState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
 
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));


    _float fRunRange = pBB->Get_Value<_float>("Yetuga", "RunRange");

    pYetuga->Get_Controller()->
        AI_MoveTo(pOwner, 
            pBB->Get_Value<CGameObject*>("Yetuga", "Target"),
            fRunRange - 0.5f, 
            m_fSpeedPerSec,
            fTimeDelta);

    if (pModel->Play_Animation(fTimeDelta)) {}

}

void CMoveState_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
   
}

CMoveState_Yetuga* CMoveState_Yetuga::Create()
{
    return new CMoveState_Yetuga();
}

void CMoveState_Yetuga::Free()
{
    __super::Free();
}
