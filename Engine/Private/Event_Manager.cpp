#include "Event_Manager.h"
#include "GameInstance.h"


CEvent_Manager::CEvent_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

bool CEvent_Manager::Unsubscribe(_uint iEventType, _uint iListenerId)
{
	IChannel* ch = FindChannel(iEventType);
	return ch ? ch->Unsubscribe(iListenerId) : false;
}

void CEvent_Manager::UnsubscribeAll(_uint iEventType)
{
	auto it = m_Channels.find(iEventType);
	if (it == m_Channels.end()) return;
	it->second->Clear();
	delete it->second;
	m_Channels.erase(it);
}

void CEvent_Manager::ClearAll()
{
	for (auto& kv : m_Channels) { kv.second->Clear(); delete kv.second; }
	m_Channels.clear();
	m_iNextListenerID = 0;
}

CEvent_Manager* CEvent_Manager::Create()
{
	return new CEvent_Manager();
}

void CEvent_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
	ClearAll();

}
