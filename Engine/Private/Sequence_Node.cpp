#include "Sequence_Node.h"

CSequence_Node::CSequence_Node()
{
}

BTNODESTATE CSequence_Node::Tick(CBlackBoard* BB)
{
	while (m_iCurrentIndex < m_Children.size())
	{
		auto pChild = m_Children[m_iCurrentIndex];
		BTNODESTATE eState = pChild->Tick(BB);

		if (BTNODESTATE::FAILURE == eState)
		{
			pChild->Terminate(BTNODESTATE::FAILURE);
			m_iCurrentIndex = 0;
			return BTNODESTATE::FAILURE;
		}
		else if (BTNODESTATE::RUNNING == eState)
			return BTNODESTATE::RUNNING;
		else
		{
			pChild->Terminate(BTNODESTATE::SUCCESS);
			m_iCurrentIndex++;
		}
	}

	m_iCurrentIndex = 0;
	return BTNODESTATE::SUCCESS;
}

void CSequence_Node::Terminate(BTNODESTATE eState)
{
	if (m_iCurrentIndex < m_Children.size())
		m_Children[m_iCurrentIndex]->Terminate(eState);

	m_iCurrentIndex = 0;
}

void CSequence_Node::Abort()
{
	if (m_iCurrentIndex < m_Children.size())
		m_Children[m_iCurrentIndex]->Abort();

	m_iCurrentIndex = 0;
}

CSequence_Node* CSequence_Node::Create()
{
	return new CSequence_Node();
}

void CSequence_Node::Free()
{
	__super::Free();
}
