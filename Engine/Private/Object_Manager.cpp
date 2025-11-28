#include "Object_Manager.h"
#include "GameInstance.h"

#include "GameObject.h"

#include "Layer.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}


CComponent* CObject_Manager::Get_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer*		pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if(nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex);
}

CGameObject* CObject_Manager::Get_GameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObject(iIndex);
}

CGameObject* CObject_Manager::Get_BackGameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (pLayer == nullptr)
		return nullptr;

	return 	pLayer->Get_BackGameObject();
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_pLayers = new map<const _wstring, CLayer*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, TIME_CHANNEL eTimeChannel, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
 		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		pLayer->Set_TimeChannel(eTimeChannel);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Push_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, TIME_CHANNEL eTimeChannel, CGameObject* pGameObject)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

void CObject_Manager::Priority_Update(TIME_DELTA tTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Priority_Update(tTimeDelta.TimeDeltas[ENUM_CLASS(Pair.second->Get_TimeChannel())]);
		}
	}
}

void CObject_Manager::Update(TIME_DELTA tTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(Pair.second->Get_TimeChannel())]);
		}
	}
}

void CObject_Manager::Late_Update(TIME_DELTA tTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(tTimeDelta.TimeDeltas[ENUM_CLASS(Pair.second->Get_TimeChannel())]);
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iLevelIndex])
	{		
        Pair.second->PoolObject_Back();
		Safe_Release(Pair.second);
	}
	m_pLayers[iLevelIndex].clear();
}

void CObject_Manager::Static_Clear()
{
	for (auto& Pair : m_pLayers[m_pGameInstance->Get_StaticLevel()])
	{     
		Pair.second->DeadObject_Clear();
	}
}

CLayer* CObject_Manager::Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLayerLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();	
	}


	Safe_Release(m_pGameInstance);

	Safe_Delete_Array(m_pLayers);
	
}
