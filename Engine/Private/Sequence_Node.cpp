#include "Sequence_Node.h"

CSequence_Node::CSequence_Node()
{
}

BTNODESTATE CSequence_Node::Tick(CBlackBoard* BB)
{
	// 시퀀스에 등록된 자식 노드들을 모두 순회
	while (m_iCurrentIndex < m_Children.size())
	{
		// 현재 자식노드를 실행시켜서 반환 값을 저장한다.
		auto pChild = m_Children[m_iCurrentIndex];
		BTNODESTATE eState = pChild->Tick(BB);

		// 반환 값이 FAIL이라면
		if (BTNODESTATE::FAILURE == eState)
		{
			// 나가는 로직을 실행시키고
			pChild->Terminate(BTNODESTATE::FAILURE,BB);
			// 가장 앞의 자식을 가리키게하고 FAIL반환
			m_iCurrentIndex = 0;
			return BTNODESTATE::FAILURE;
		}
		// RUNNING 이면 유지
		else if (BTNODESTATE::RUNNING == eState)
			return BTNODESTATE::RUNNING;
		// SUCCESS 라면
		else
		{
			// 나가는 로직을 실행시키고
			pChild->Terminate(BTNODESTATE::SUCCESS,BB);
			// 다음 자식 노드를 가리키게한다.
			m_iCurrentIndex++;
		}
	}
	// 모든 자식 노드들을 성공 시켰을시 가장 앞에 노드를 가리키고 SUCCESS 반환
	m_iCurrentIndex = 0;
	return BTNODESTATE::SUCCESS;
}

void CSequence_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{

	for (auto& Child : m_Children)
		Child->Terminate(eState, BB);
	m_iCurrentIndex = 0;
}

void CSequence_Node::Abort()
{
	for (auto& Child : m_Children)
		Child->Abort();
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
