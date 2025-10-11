#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CEvent_Manager final : public CBase
{
public:
	typedef struct tagEvent
	{
		_uint iID;
		std::function<void()> fEvent;
	}EVENT_DESC;
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:
	_uint Subscribe(_uint iEventType, std::function<void()> fEvent);
	void UnSubscribeAll(_uint iEventType);
	void UnSubscribe(_uint iEventType, _uint iID);
	HRESULT Emit(_uint iEventType);

	void Clear();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	map<_uint, vector<EVENT_DESC>>	m_Events;

	static _uint	iEvent_Index;

public:
	static CEvent_Manager* Create();
	virtual void Free() override;
};

NS_END