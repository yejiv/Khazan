#include "AS_P2_Roar_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_P2_Roar_Viper::CAS_P2_Roar_Viper()
{

}

void CAS_P2_Roar_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(43);

    // 로어 애니메이션 시작

}

void CAS_P2_Roar_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_RoarFinished", true);

        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2Loar", false);

        // 애니메이션 종료

    }
}

void CAS_P2_Roar_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    // 스테이트 바뀔떄 
}

CAS_P2_Roar_Viper* CAS_P2_Roar_Viper::Create()
{
    return new CAS_P2_Roar_Viper();
}

void CAS_P2_Roar_Viper::Free()
{
    __super::Free();
}
