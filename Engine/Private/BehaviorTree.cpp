#include "BehaviorTree.h"
#include "BTNode.h"
#include "BlackBoard.h"
#include "BTNode_Defines.h"

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
    if (BTNODESTATE::RUNNING == m_ePrevState && BTNODESTATE::RUNNING != eState)
    {
        m_pRoot->Terminate(eState,m_pBlackBoard);
    }

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

BTEVENT CBehaviorTree::Notify_Event(BTEVENT Event)
{
    Terminate_AllNode(m_pRoot);

    switch (Event)
    {
    case BTEVENT::DEAD:
        m_pBlackBoard->Set_Value<_bool>(m_strName, "IsDeadEvent", true);
        break;
    case BTEVENT::HIT:
        m_pBlackBoard->Set_Value<_bool>(m_strName, "IsHitEvent", true);
        break;
    }

   /* if (m_pRoot)
        m_pRoot->Tick(m_pBlackBoard);*/

    return Event;
}


void CBehaviorTree::Terminate_AllNode(CBTNode* pNode)
{
    if (!pNode)
        return;

    if (pNode->Get_NodeType() == NODETYPE::LEAF)
    {
        pNode->Terminate(BTNODESTATE::FAILURE, m_pBlackBoard);
        return;
    }
    else if (pNode->Get_NodeType() == NODETYPE::COMPOSITE)
    {
        CComposite_Node* pComposite = static_cast<CComposite_Node*>(pNode);
        for (auto& pChild : pComposite->Get_Children())
            Terminate_AllNode(pChild);
    }
    else if (pNode->Get_NodeType() == NODETYPE::DECORATOR)
    {
        CDecorator_Node* pDecorator = static_cast<CDecorator_Node*>(pNode);
        CBTNode* pChild = pDecorator->Get_Child();
        if (pChild)
            Terminate_AllNode(pChild);
    }

    // 마지막으로 자기 자신 Terminate
    pNode->Terminate(BTNODESTATE::FAILURE, m_pBlackBoard);
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
