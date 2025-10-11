#pragma once
#include "Base.h"

// MultiDelegate는 함수를 여러개 저장해두었다가 
// 한 번에 호출하는 클래스

NS_BEGIN(Engine)

// 템플릿을 사용함으로써 매개변수를 받을 수 있도록 한다.
// 함수 형태를 좀더 자유롭게 지정할 수 있게 가능해짐.
template<typename... Args>

class CMultiDelegate final : public CBase
{
public:
	CMultiDelegate() = default;
	virtual ~CMultiDelegate() = default;

public:
	// using을 사용하여 function<void(Args...)> FuncType으로 정의
	using FuncType = function<void(Args...)>;
	using DelegateID = size_t;


public:
	size_t	Count() const { return m_Delegates.size(); }
	bool IsEmpty() const { return m_Delegates.empty(); }

	// 구독 추가
	DelegateID Add(const FuncType& Func)
	{
		DelegateID id = m_NextID++;
		m_Delegates[id] = Func;
		return id;
	}
	// 구독 해제
	void Remove(DelegateID id)
	{
		m_Delegates.erase(id);
	}

	// 전체 해제
	void Clear()
	{
		m_Delegates.clear();
	}

	void Broadcast(Args... args) const
	{
		// 복사본을 사용해서 호출 중 삭제되어도 안전하도록 한다.
		auto DelegatesCopy = m_Delegates;
		for (auto& pair : DelegatesCopy)
		{
			if (pair.second)
				pair.second(args...); // pair.second가 함수
		}
	}

private:
	unordered_map<DelegateID, FuncType> m_Delegates;
	DelegateID							m_NextID = 1;

public:
	virtual void						Free() override;
};

NS_END

template<typename ...Args>
inline void CMultiDelegate<Args...>::Free()
{
	__super::Free();
}
