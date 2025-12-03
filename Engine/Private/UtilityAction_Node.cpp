#include "UtilityAction_Node.h"
#include "BlackBoard.h"

USING(Engine)

CUtilityAction_Node::CUtilityAction_Node(SCORE Score, ACTION Action, TERMINATE Terminate)
    : m_Score(Score), m_Action(Action), m_Terminate(Terminate)
{
    m_eNodeType = NODETYPE::LEAF;
}

BTNODESTATE CUtilityAction_Node::Tick(CBlackBoard* BB)
{
    if (nullptr == m_Action)
        return BTNODESTATE::FAILURE;

    // ���� ���� �� ���� ���� ON
    m_isExecuting = true;

    // ACTION ��� ��ȯ (FSM�� ����/����/���� ����)
    BTNODESTATE eState = m_Action(BB);

    // ����� ���� OFF
    if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
        m_isExecuting = false;

    return eState;
}

void CUtilityAction_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
    if (m_Terminate)
        m_Terminate(BB, eState);

    m_isExecuting = false;
}

void CUtilityAction_Node::Abort(CBlackBoard* BB)
{
    // 강제 중단
    if (m_Terminate)
        m_Terminate(BB, BTNODESTATE::FAILURE);

    m_isExecuting = false;
}

CUtilityAction_Node* CUtilityAction_Node::Create(SCORE Score, ACTION Action, TERMINATE Terminate)
{
    return new CUtilityAction_Node(Score, Action, Terminate);
}

void CUtilityAction_Node::Free()
{
    __super::Free();
}
