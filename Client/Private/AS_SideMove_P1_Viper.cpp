#include "AS_SideMove_P1_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"


CAS_SideMove_P1_Viper::CAS_SideMove_P1_Viper()
{

}

void CAS_SideMove_P1_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    // 플레이어가 오른쪽에 있으면 왼쪽으로 무빙
    // 왼쪽에 있으면 오른쪽으로 무빙

    m_eState = P1SIDEMOVESTATE::MOVE;

    CGameObject* pTarget = static_cast<CGameObject*>(pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target"));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));

    _vector vOwnerRight = pOwnerTransform->Get_State(STATE::RIGHT);
    _vector vTargetLook = pTargetTransform->Get_State(STATE::LOOK);

    _float fDot = XMVectorGetX(XMVector3Dot(vOwnerRight,vTargetLook));
    if (fDot < -0.1f)
        pModel->Set_Animation(106);
    
    else
        pModel->Set_Animation(107);
       
}

void CAS_SideMove_P1_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();


    if (pModel->Play_Animation(fTimeDelta))
    {
        if (P1SIDEMOVESTATE::MOVE == m_eState)
        {
            CGameObject* pTarget = static_cast<CGameObject*>(pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target"));
            CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
            CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            pOwnerTransform->LookAt(vTargetPos);
            
            pModel->Set_Animation(18); 
            pModel->Set_BlendTime(0.08f);
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 1.f, 0.1f, 0.25f);
            m_eState = P1SIDEMOVESTATE::ATTACK;
        }

        else if (P1SIDEMOVESTATE::ATTACK == m_eState)
        {
            m_eState = P1SIDEMOVESTATE::END;
            pModel->Set_BlendTime(0.25f);
            pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_SideMoveFinished", true);
        }
    }

}

void CAS_SideMove_P1_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}



CAS_SideMove_P1_Viper* CAS_SideMove_P1_Viper::Create()
{
    return new CAS_SideMove_P1_Viper();
}

void CAS_SideMove_P1_Viper::Free()
{
    __super::Free();
}
