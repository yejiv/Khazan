#include "UI_Layer.h"

CUI_Layer::CUI_Layer()
{
}

HRESULT CUI_Layer::Add_Event(const _wstring& strLayerTag, function<void()> Event)
{
	function<void()> pEvent = Find_Event(strLayerTag);

	if (pEvent != nullptr)
		return E_FAIL;

	m_Event.emplace(strLayerTag, Event);

	return S_OK;
}

HRESULT CUI_Layer::Add_ParamEvent(const _wstring& strLayerTag, function<void(void*)> Event)
{
	function<void(void*)> pEvent = Find_ParamEvent(strLayerTag);

	if (pEvent != nullptr)
		return E_FAIL;

	m_ParamEvents.emplace(strLayerTag, Event);

	return S_OK;
}

function<void()> CUI_Layer::Pop_Event(const _wstring& strLayerTag)
{
	return Find_Event(strLayerTag);
}

function<void(void*)> CUI_Layer::Pop_ParamEvent(const _wstring& strLayerTag)
{
	return Find_ParamEvent(strLayerTag);
}

function<void()> CUI_Layer::Find_Event(const _wstring& strLayerTag)
{
	auto iter = m_Event.find(strLayerTag);

	if (iter == m_Event.end())
		return nullptr;

	return iter->second;
}

function<void(void*)> CUI_Layer::Find_ParamEvent(const _wstring& strLayerTag)
{
	auto iter = m_ParamEvents.find(strLayerTag);

	if (iter == m_ParamEvents.end())
		return nullptr;

	return iter->second;
}

CUI_Layer* CUI_Layer::Create()
{
	return new CUI_Layer();
}

void CUI_Layer::Free()
{
	__super::Free();
}