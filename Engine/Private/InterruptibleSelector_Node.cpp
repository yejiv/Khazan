#include "InterruptibleSelector_Node.h"

CInterruptibleSelector_Node::CInterruptibleSelector_Node(INTERRUPTCONDITION Func)
	:m_pInterruptCondition{Func}
{
    m_eNodeType = NODETYPE::COMPOSITE;
}

BTNODESTATE CInterruptibleSelector_Node::Tick(CBlackBoard* BB)
{
    if (m_pInterruptCondition && m_pInterruptCondition(BB))
    {
        /*if (m_iCurrentIndex < m_Children.size())
            m_Children[m_iCurrentIndex]->Abort(BB);*/
        for (auto& pChild : m_Children)
            pChild->Abort(BB);

        m_iCurrentIndex = 0;
        return BTNODESTATE::FAILURE; // 인터룹트 발생시 현재 노드는 FAIL처리한다.
    }

	// 기존 셀렉터와 동일
	
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
			pChild->Terminate(BTNODESTATE::SUCCESS, BB);
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
			pChild->Terminate(BTNODESTATE::FAILURE, BB);
			// 다음 인덱스를 검사한다.
			m_iCurrentIndex++;
		}
	}

	// 모든 노드를 다 돌면 가장 앞 노드를 가리키게하고 FAILURE를 반환한다.
	m_iCurrentIndex = 0;
	return BTNODESTATE::FAILURE;


}

void CInterruptibleSelector_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
	if (m_iCurrentIndex < m_Children.size())
		m_Children[m_iCurrentIndex]->Terminate(eState, BB);

	m_iCurrentIndex = 0;

}

void CInterruptibleSelector_Node::Abort(CBlackBoard* BB)
{
    if (m_iCurrentIndex < m_Children.size())
        m_Children[m_iCurrentIndex]->Terminate(BTNODESTATE::FAILURE, BB);

    m_iCurrentIndex = 0;
}

CInterruptibleSelector_Node* CInterruptibleSelector_Node::Create(INTERRUPTCONDITION Func)
{
    return new CInterruptibleSelector_Node(Func);
}

void CInterruptibleSelector_Node::Free()
{
    __super::Free();
}
