#include "Action_Node.h"

CAction_Node::CAction_Node(ACTION Action, TERMINATE Terminate)
    :m_Action{ Action }
    , m_Terminate{Terminate}
{
}

BTNODESTATE CAction_Node::Tick(CBlackBoard* BB)
{

    if (m_Action)
    {
        return m_Action(BB);
       
    }


    return BTNODESTATE::FAILURE;
  

}

void CAction_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
    if (m_Terminate)
        return m_Terminate(BB, eState);
}


void CAction_Node::Abort()
{
    // 강제 중단
    if (m_Terminate)
        m_Terminate(nullptr, BTNODESTATE::FAILURE);
}

CAction_Node* CAction_Node::Create(ACTION Action, TERMINATE Terminate)
{
    return new CAction_Node(Action,Terminate);
}

void CAction_Node::Free()
{
    __super::Free();
}
