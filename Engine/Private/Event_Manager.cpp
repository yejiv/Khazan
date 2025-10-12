#include "Event_Manager.h"
#include "GameInstance.h"

_uint CEvent_Manager::iEvent_Index = 0;

CEvent_Manager::CEvent_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

_uint CEvent_Manager::Subscribe(_uint iEventType, std::function<void()> fEvent)
{
	EVENT_DESC EventDesc{};

	EventDesc.iID = iEvent_Index;
	EventDesc.fEvent = fEvent;

	iEvent_Index++;

	auto iter = m_Events.find(iEventType);

	if (iter == m_Events.end())
	{
		vector<EVENT_DESC> vecFunc;

		vecFunc.push_back(EventDesc);

		m_Events.emplace(iEventType, vecFunc);
	}
	else
	{
		iter->second.push_back(EventDesc);
	}

	return EventDesc.iID;
}

void CEvent_Manager::UnSubscribeAll(_uint iEventType)
{
	auto iter = m_Events.find(iEventType);

	if (iter == m_Events.end())
		return;

	iter->second.clear();

	m_Events.erase(iEventType);
}

void CEvent_Manager::UnSubscribe(_uint iEventType, _uint iID)
{
	auto iter = m_Events.find(iEventType);

	if (iter == m_Events.end())
		return;

	vector<EVENT_DESC> Descs = iter->second;

	for (auto iter = Descs.begin(); iter != Descs.end(); ) {
		if ((*iter).iID == 3) {
			iter = Descs.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
}

HRESULT CEvent_Manager::Emit(_uint iEventType)
{
	auto iter = m_Events.find(iEventType);

	if (iter == m_Events.end())
		return E_FAIL;

	for (auto& Event : iter->second)
	{
		try {
			Event.fEvent();
		}
		catch (std::bad_function_call&)
		{

		}
		
	}


	return S_OK;
}

void CEvent_Manager::Clear()
{
	m_Events.clear();

	iEvent_Index = 0;
}

CEvent_Manager* CEvent_Manager::Create()
{
	return new CEvent_Manager();
}

void CEvent_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
	Clear();
}
