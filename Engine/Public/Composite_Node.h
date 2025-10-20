#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)

template<typename T>
class CComposite_Node abstract : public CBTNode
{
protected:
	CComposite_Node();
	virtual ~CComposite_Node() = default;

public:
	virtual BTNODESTATE Tick(class CBlackBoard* BB) = 0;

	// 노드 종료 처리 : 성공 / 실패 후 정리기능
	virtual void	Terminate(BTNODESTATE eState) {};
	// 노드 강제 중단 : 외부 이벤트나 상위 노드 요청시 노드들을 강제 중단
	virtual void	Abort() { Terminate(BTNODESTATE::FAILURE); };

public:
	void			Add_Child(CBTNode* pChild)
	{
		if (nullptr != pChild)
			m_Children.push_back(pChild);
	}

protected:
	vector<CBTNode*>	m_Children{};
	_uint				m_iCurrentIndex = {};

public:
	virtual void	Free() override
	{
		for (auto& pChild : m_Children)
		{
			Safe_Release(pChild);
		}

		m_Children.clear();
		__super::Free();
	}

};

NS_END

template<typename T>
inline CComposite_Node<T>::CComposite_Node()
{
}

