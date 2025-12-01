#include "AS_P2_DashDrift_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"



CAS_P2_DashDrift_Viper::CAS_P2_DashDrift_Viper()
{

}

void CAS_P2_DashDrift_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(22);

    m_eState = DRIFTSTATE::START;
}

void CAS_P2_DashDrift_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));

   

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

        switch (m_eState)
        {
        case Client::DRIFTSTATE::START:
        {
            m_eState = DRIFTSTATE::LOOP;
            pModel->Set_Animation(21);
        }
        break;
        case Client::DRIFTSTATE::LOOP:
        {
            CTransform* pOwnerTransform = static_cast<CTransform*>(pViper->Get_Component(TEXT("Com_Tranform")));
            //CTransform* pTarget
            //pOwnerTransform->AI_Chase();

            m_eState = DRIFTSTATE::FINISH;
            pModel->Set_Animation(20);
        }
        break;
        case Client::DRIFTSTATE::FINISH:
        {
            pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_DashDriftFinished",true);
        }
        break;
        }
    }

}

void CAS_P2_DashDrift_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_P2_DashDrift_Viper* CAS_P2_DashDrift_Viper::Create()
{
    return new CAS_P2_DashDrift_Viper;
}

void CAS_P2_DashDrift_Viper::Free()
{
    __super::Free();
}
