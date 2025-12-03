#include "AS_P2_SideMove_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"


CAS_P2_SideMove_Viper::CAS_P2_SideMove_Viper()
{

}

void CAS_P2_SideMove_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    CGameObject* pTarget = static_cast<CGameObject*>(pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target"));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));

    _vector vOwnerRight = pOwnerTransform->Get_State(STATE::RIGHT);
    _vector vTargetLook = pTargetTransform->Get_State(STATE::LOOK);

    _float fDot = XMVectorGetX(XMVector3Dot(vOwnerRight, vTargetLook));
    if (fDot < -0.1f)
        pModel->Set_Animation(45);

    else
        pModel->Set_Animation(46);

}

void CAS_P2_SideMove_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SideMoveFinished", true);
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pOwner);
    }

}

void CAS_P2_SideMove_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}



CAS_P2_SideMove_Viper* CAS_P2_SideMove_Viper::Create()
{
    return new CAS_P2_SideMove_Viper();
}

void CAS_P2_SideMove_Viper::Free()
{
    __super::Free();
}
