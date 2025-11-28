#include "AS_P2_HandSwing3Hit.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_P2_HandSwing3Hit::CAS_P2_HandSwing3Hit()
{

}

void CAS_P2_HandSwing3Hit::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pModel->Set_AnimationSet("HandSwing3Hit");

}

void CAS_P2_HandSwing3Hit::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_HandSwing3HitFinished", true);
    }
}

void CAS_P2_HandSwing3Hit::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_P2_HandSwing3Hit* CAS_P2_HandSwing3Hit::Create()
{
    return new CAS_P2_HandSwing3Hit();
}

void CAS_P2_HandSwing3Hit::Free()
{
    __super::Free();
}
