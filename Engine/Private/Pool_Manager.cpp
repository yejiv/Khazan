#include "Pool_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

CPool_Manager::CPool_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPool_Manager::Initialize(_uint iNumLevels)
{
	m_pPools = new map<const _wstring, deque<class CGameObject*>>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CPool_Manager::Add_PoolObject(_uint iPrototypeLevelIndex, const _wstring strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strPoolTag, void* pArg, _uint iCount)
{
	for (size_t i = 0; i < iCount; i++)
	{
		CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
		if (nullptr == pGameObject)
			return E_FAIL;

		pGameObject->Set_IsPool(true);
		pGameObject->Set_Tag(strPoolTag);
		pGameObject->Set_Layer(iLayerLevelIndex);

		deque<CGameObject*>* pPool = Find_Pool(iLayerLevelIndex, strPoolTag);
		if (nullptr == pPool)
		{
			deque<CGameObject*> dePool;
			dePool.push_back(pGameObject);
			m_pPools[iLayerLevelIndex].emplace(strPoolTag, dePool);
		}
		else
			pPool->push_back(pGameObject);
	}


	return S_OK;
}

CGameObject* CPool_Manager::Pop_PoolObject(_uint iLayerLevelIndex, const _wstring& strPoolTag)
{
	deque<CGameObject*>* pPool = Find_Pool(iLayerLevelIndex, strPoolTag);

	if (pPool == nullptr || pPool->size() <= 0)
		return nullptr;

	CGameObject* pGameObject = pPool->front();

	pPool->pop_front();

	return pGameObject;
}


HRESULT CPool_Manager::Reset_PoolObject(CGameObject* pGameObject)
{

	pGameObject->Set_IsPool(true);
	pGameObject->Set_IsDead(false);
	pGameObject->Reset();

	deque<CGameObject*>* pPool = Find_Pool(pGameObject->Get_Layer(), pGameObject->Get_Tag());

	if (pPool == nullptr || pPool->size() <= 0)
		return E_FAIL;

	pPool->push_back(pGameObject);

	return S_OK;
}

void CPool_Manager::Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	m_pGameInstance->Push_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, pGameObject);
}

void CPool_Manager::Clear(_uint iLevelIndex)
{
	for (auto Pool : m_pPools[iLevelIndex])
	{
		for (auto PoolObj : Pool.second)
		{
			Safe_Release(PoolObj);
		}
		Pool.second.clear();
	}
	m_pPools[iLevelIndex].clear();
}


deque<CGameObject*>* CPool_Manager::Find_Pool(_uint iLayerLevelIndex, const _wstring& strPoolTag)
{
	if (!m_pPools)
		return nullptr;

	auto iter = m_pPools[iLayerLevelIndex].begin();

	for (auto iter = m_pPools[iLayerLevelIndex].begin(); iter != m_pPools[iLayerLevelIndex].end();)
	{
		if (iter->first == strPoolTag)
		{
			return &(iter->second);
		}
		else
			++iter;
	}

	return nullptr;
}

CPool_Manager* CPool_Manager::Create(_uint iNumLevels)
{
	CPool_Manager* pInstance = new CPool_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CPool_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPool_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPools[i])
		{
			for (auto pGameObject : Pair.second)
			{
				Safe_Release(pGameObject);
			}
			Pair.second.clear();
		}
		m_pPools[i].clear();
	}

	Safe_Delete_Array(m_pPools);
}
