#include "Selector_Node.h"

CSelector_Node::CSelector_Node()
{
}

BTNODESTATE CSelector_Node::Tick(CBlackBoard* BB)
{
	// 가지고있는 노드 수만큼 돌면서
	while (m_iCurrentIndex < m_Children.size())
	{
		// 현재 액션 노드의 상태를 가지고와서
		auto pChild = m_Children[m_iCurrentIndex];
		BTNODESTATE eState = pChild->Tick(BB);

		// 상태가 SUCCESS 라면
		if (BTNODESTATE::SUCCESS == eState)
		{
			// 현재 노드의 성공 초기화를 실행하고
			pChild->Terminate(BTNODESTATE::SUCCESS,BB);
			// 가장 앞의 자식노드를 가리키고 SUCCESS 반환
			m_iCurrentIndex = 0;
			return BTNODESTATE::SUCCESS;
		}
		// 상태가 RUNNING 이라면 RUNNING 유지
		else if (BTNODESTATE::RUNNING == eState)
			return BTNODESTATE::RUNNING;

		// FAILURE 이라면 
		else
		{
			// 실패 초기화 하고
			pChild->Terminate(BTNODESTATE::FAILURE,BB);
			// 다음 인덱스를 검사한다.
			m_iCurrentIndex++;
		}
	}

	// 모든 노드를 다 돌면 가장 앞 노드를 가리키게하고 FAILURE를 반환한다.
	m_iCurrentIndex = 0;
	return BTNODESTATE::FAILURE;
}

void CSelector_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
	// 노드 종료시 실행한다.

	if (m_iCurrentIndex < m_Children.size())
		m_Children[m_iCurrentIndex]->Terminate(eState, BB);
	m_iCurrentIndex = 0;
}

void CSelector_Node::Abort()
{
	if (m_iCurrentIndex < m_Children.size())
		m_Children[m_iCurrentIndex]->Abort();
	m_iCurrentIndex = 0;
}

CSelector_Node* CSelector_Node::Create()
{
    return new CSelector_Node();
}

void CSelector_Node::Free()
{
    __super::Free();
}
