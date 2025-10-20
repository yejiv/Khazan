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
