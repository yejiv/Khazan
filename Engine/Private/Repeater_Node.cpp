#include "Repeater_Node.h"

CRepeater_Node::CRepeater_Node(_int iRepeatCount)
	:m_iRepeatCount{iRepeatCount}
{
    m_eNodeType = NODETYPE::DECORATOR;
}

BTNODESTATE CRepeater_Node::Tick(CBlackBoard* BB)
{
	while (m_iCurrentCount < m_iRepeatCount)
	{
		BTNODESTATE eState = m_pChild->Tick(BB);

		if (BTNODESTATE::RUNNING == eState)
			return BTNODESTATE::RUNNING;

		++m_iCurrentCount;
	}
	m_iCurrentCount = 0;

	return BTNODESTATE::SUCCESS;
}

void CRepeater_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
	m_iCurrentCount = 0;
}

void CRepeater_Node::Abort(CBlackBoard* BB)
{
    m_iCurrentCount = 0;
}


CRepeater_Node* CRepeater_Node::Create(_int iRepeatCount)
{
	return new CRepeater_Node(iRepeatCount);
}

void CRepeater_Node::Free()
{
	__super::Free();
}
