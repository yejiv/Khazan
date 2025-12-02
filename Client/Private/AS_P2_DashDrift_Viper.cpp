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

    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    //_bool isBerserker = pBB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");  
    //if (!isBerserker)
        m_fMoveSpeed = 15.f;

    m_fAttackRange = pBB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
   

    m_eState = DRIFTSTATE::START;
}

void CAS_P2_DashDrift_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();


    if (m_eState == DRIFTSTATE::LOOP)
    {
        CTransform* pOwnerTransform = static_cast<CTransform*>(pViper->Get_Component(TEXT("Com_Transform")));
        CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
        CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
        _vector vOwnerPos = pOwnerTransform->Get_State(STATE::POSITION);
        _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
        pOwnerTransform->AI_Chase(vTargetPos, fTimeDelta, m_fMoveSpeed, m_fAttackRange);

        _float fDist = pBB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");

        if (fDist <= m_fAttackRange + 5.f || pBB->Get_Value<_bool>(pViper->Get_Name(),"isP2_Dash_Abort"))
        {
            m_eState = DRIFTSTATE::FINISH;
            pModel->Set_Animation(20);
        }
    }

    if (pModel->Play_Animation(fTimeDelta))
    {

        switch (m_eState)
        {
        case Client::DRIFTSTATE::START:
        {
            m_eState = DRIFTSTATE::LOOP;
            pModel->Set_Animation(21);
        }
        break;
      
        case Client::DRIFTSTATE::FINISH:
        {
            pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_DashDriftFinished",true);
            pBB->Set_Value<_bool>(pViper->Get_Name(), "isP2_Dash_Abort", false);
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
