#include "UtilityAction_Node.h"

CUtilityAction_Node::CUtilityAction_Node(SCORE Score, ACTION Action, TERMINATE Terminate)
    :m_Score{ Score }
    , m_Action{Action}
    , m_Terminate{Terminate}
{
    m_eNodeType = NODETYPE::LEAF;
}

BTNODESTATE CUtilityAction_Node::Tick(CBlackBoard* BB)
{
    if (m_Action)
        return m_Action(BB);

    return BTNODESTATE::FAILURE;
}

void CUtilityAction_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
    if (m_Terminate)
        return m_Terminate(BB, eState);
}

void CUtilityAction_Node::Abort(CBlackBoard* BB)
{
    // 강제 중단
    if (m_Terminate)
        m_Terminate(BB, BTNODESTATE::FAILURE);
}

CUtilityAction_Node* CUtilityAction_Node::Create(SCORE Score, ACTION Action, TERMINATE Terminate)
{
    return new CUtilityAction_Node(Score,Action,Terminate);
}

void CUtilityAction_Node::Free()
{
    __super::Free();
}
