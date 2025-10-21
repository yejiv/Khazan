#include "AI_Controller.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "StateMachine.h"
#include "Perception.h"
#include "GameObject.h"
#include "GameInstance.h"

CAI_Controller::CAI_Controller()
    :m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CAI_Controller::AI_MoveTo(CGameObject* pOwner, CGameObject* pTarget, _float fLimit, _float fTimeDelta)
{
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);

    pOwnerTransform->LookAt(vTargetPos);
    pOwnerTransform->AI_Chase(vTargetPos, fTimeDelta, fLimit);
}

HRESULT CAI_Controller::Initialize()
{

    return S_OK;
}

void CAI_Controller::Update(class CGameObject* pOwner, _float fTimeDelta)
{

}


void CAI_Controller::Free()
{
    __super::Free();

    Safe_Release(m_pBB);
    Safe_Release(m_pBT);
    Safe_Release(m_pFSM);
    Safe_Release(m_pPerception);
    Safe_Release(m_pGameInstance);

}
