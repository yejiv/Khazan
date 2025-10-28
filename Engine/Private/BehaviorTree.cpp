#include "BehaviorTree.h"
#include "BTNode.h"
#include "BlackBoard.h"

CBehaviorTree::CBehaviorTree()
{
}


HRESULT CBehaviorTree::Initialize(void* pArg)
{
    return S_OK;
}

BTNODESTATE CBehaviorTree::Update()
{
    if (nullptr == m_pRoot || nullptr == m_pBlackBoard)
        return BTNODESTATE::FAILURE;

    BTNODESTATE eState = m_pRoot->Tick(m_pBlackBoard);

    // Running 에서 Failure/Success 전환시에 한번만 적용시키도록해야한다.
    /*if (BTNODESTATE::RUNNING == m_ePrevState && BTNODESTATE::RUNNING != eState)
    {
        m_pRoot->Terminate(eState,m_pBlackBoard);
    }*/

    m_ePrevState = eState;

    return eState;
}

void CBehaviorTree::Set_Root(CBTNode* pRoot)
{
    if (nullptr != pRoot)
        m_pRoot = pRoot;
}

void CBehaviorTree::Set_BlackBoard(CBlackBoard* BB)
{
    if (nullptr != BB)
    {
        m_pBlackBoard = BB;
        Safe_AddRef(m_pBlackBoard);
    }
}

CBehaviorTree* CBehaviorTree::Create()
{
    return new CBehaviorTree();
}

void CBehaviorTree::Free()
{
    __super::Free();

    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pRoot);
}
