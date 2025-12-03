#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)
class CUI_Layer final : public CBase
{
private:
	CUI_Layer();
	virtual ~CUI_Layer() = default;

public:
	HRESULT								Add_Event(const _wstring& strEventTag, std::function<void()> Event);
	HRESULT								Add_ParamEvent(const _wstring& strEventTag, std::function<void(void*)> Event);
	function<void()>					Pop_Event(const _wstring& strEventTag);
	function<void(void*)>				Pop_ParamEvent(const _wstring& strEventTag);

private:
	unordered_map<_wstring,
		std::function<void()>>			m_Event;
	unordered_map<_wstring,
		std::function<void(void*)>>		m_ParamEvents;

private:
	function<void()>					Find_Event(const _wstring& strEventTag);
	function<void(void*)>				Find_ParamEvent(const _wstring& strEventTag);

public:
	static CUI_Layer*					Create();
	virtual void						Free() override;
};

NS_END