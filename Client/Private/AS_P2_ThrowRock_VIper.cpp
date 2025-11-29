#include "AS_P2_ThrowRock_VIper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_P2_ThrowRock_VIper::CAS_P2_ThrowRock_VIper()
{

}

void CAS_P2_ThrowRock_VIper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pModel->Set_Animation(50);

}

void CAS_P2_ThrowRock_VIper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_ThrowRockFinished", true);
    }
}

void CAS_P2_ThrowRock_VIper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_P2_ThrowRock_VIper* CAS_P2_ThrowRock_VIper::Create()
{
    return new CAS_P2_ThrowRock_VIper();
}

void CAS_P2_ThrowRock_VIper::Free()
{
    __super::Free();
}
