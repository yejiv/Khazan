#include "AS_DashAttack_Imp_Melee.h"
#include "Body_Imp_Melee.h"
#include "Imp_Melee.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"

CAS_DashAttack_Imp_Melee::CAS_DashAttack_Imp_Melee()
{

}

void CAS_DashAttack_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
    
   
    pModel->Set_Animation(9);
    m_fCurrentTime = 0.f;
    m_isEnd = false;
    m_isCrashed = false;
    m_eState = DASHATTACK_STATE::START;

}

void CAS_DashAttack_Imp_Melee::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pOwner->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pBB -> Get_Value<CGameObject*>(pImp->Get_Name(), "Target")->Get_Component(TEXT("Com_Transform")));
    _vector vOwnerLoc = pTransform->Get_State(STATE::POSITION);
    _vector vTargetLoc = pTargetTransform->Get_State(STATE::POSITION);
    _vector vDir = XMVector3Normalize(vTargetLoc - vOwnerLoc);
    _float fDist = XMVectorGetX(XMVector3Length(vTargetLoc - vOwnerLoc));

    m_fCurrentTime += fTimeDelta;

    switch (m_eState)
    {
    case Client::DASHATTACK_STATE::START:

        if (pModel->Play_Animation(fTimeDelta))
        {
            m_fSpeed = 15.f;
            pModel->Set_Animation(8);
            m_eState = DASHATTACK_STATE::RUNNIG;
        }
        break;
    case Client::DASHATTACK_STATE::RUNNIG:
        pImp->Get_Controller()->AI_MoveTo(pImp, pBB->Get_Value<CGameObject*>(pImp->Get_Name(), "Target"),m_fSpeed,
            pBB -> Get_Value<_float>(pImp->Get_Name(), "RunRange"),fTimeDelta);

        if (pBB->Get_Value<_float>(pImp->Get_Name(), "RunRange") - 0.5f >= fDist)
        {
            pModel->Set_Animation(9);
            m_eState = DASHATTACK_STATE::FINISHED;
            m_fCurrentTime = 0.f;
        }
        else if (m_fCurrentTime >= m_fLoseTime)
        {
            pModel->Set_Animation(0);
            m_eState = DASHATTACK_STATE::DODGE;
            m_fCurrentTime = 0.f;
        }
        break;
    case Client::DASHATTACK_STATE::FINISHED:

        if(pModel->Play_Animation(fTimeDelta))
        {
            pBB->Set_Value<_bool>(pImp->Get_Name(), "isDashAttackFinished", true);
        }
        break;
    case Client::DASHATTACK_STATE::DODGE:
        if (pModel->Play_Animation(fTimeDelta))
        {    
            pBB->Set_Value<_bool>(pImp->Get_Name(), "isDashAttackFinished", true);
        }
        break;

    }
}

void CAS_DashAttack_Imp_Melee::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_DashAttack_Imp_Melee::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{

}

CAS_DashAttack_Imp_Melee* CAS_DashAttack_Imp_Melee::Create()
{
    return new CAS_DashAttack_Imp_Melee();
}

void CAS_DashAttack_Imp_Melee::Free()
{
    __super::Free();
}
