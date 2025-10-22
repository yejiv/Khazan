#include "Condition_Node.h"

CCondition_Node::CCondition_Node(CONDITION Condition)
    :m_Condition{ Condition }
{
}

BTNODESTATE CCondition_Node::Tick(CBlackBoard* BB)
{
    if (m_Condition && m_Condition(BB))
        return BTNODESTATE::SUCCESS;

    return BTNODESTATE::FAILURE;
}

CCondition_Node* CCondition_Node::Create(CONDITION Condition)
{
    return new CCondition_Node(Condition);
}

void CCondition_Node::Free()
{
    __super::Free();
}
