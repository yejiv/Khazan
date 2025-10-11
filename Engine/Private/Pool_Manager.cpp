#include "EnginePch.h"
#include "Pool_Manager.h"
#include "GameInstance.h"
#include "Pool.h"

CPool_Manager::CPool_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
}

HRESULT CPool_Manager::Add_PoolObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring strPrototypeTag, const _wstring& strPoolTag, void* pArg, _uint iCount)
{
	for (size_t i = 0; i < iCount; i++)
	{
		CPool* pPoolObject = dynamic_cast<CPool*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
		if (nullptr == pPoolObject)
			return E_FAIL;

		deque<CPool*>* pPool = Find_Pool(strPoolTag);
		if (nullptr == pPool)
		{
			deque<CPool*> dePool;
			dePool.push_back(pPoolObject);
			m_Pools.emplace(strPoolTag, dePool);
		}
		else
			pPool->push_back(pPoolObject);
	}


	return S_OK;
}

CPool* CPool_Manager::Pop_PoolObject(const _wstring& strPoolTag)
{
	deque<CPool*>* pPool = Find_Pool(strPoolTag);

	if (pPool == nullptr || pPool->size() <= 0)
		return nullptr;

	CPool* pPoolObject = pPool->front();

	pPool->pop_front();

	return pPoolObject;
}

HRESULT CPool_Manager::Reset_PoolObject(CPool* pPoolObject)
{
	if (!pPoolObject->Get_IsDead())
		return E_FAIL;

	pPoolObject->Set_IsDead(false);
	pPoolObject->Reset();

	deque<CPool*>* pPool = Find_Pool(pPoolObject->Get_Tag());

	if (pPool == nullptr || pPool->size() <= 0)
		return;

	pPool->push_back(pPoolObject);

	return S_OK;
}

HRESULT CPool_Manager::Reset_PoolObject(CGameObject* pGameObject)
{
	CPool* pPoolObject = dynamic_cast<CPool*>(pGameObject);
	if (!pPoolObject->Get_IsDead())
		return E_FAIL;

	pPoolObject->Set_IsDead(false);
	pPoolObject->Reset();

	deque<CPool*>* pPool = Find_Pool(pPoolObject->Get_Tag());

	if (pPool == nullptr || pPool->size() <= 0)
		return;

	pPool->push_back(pPoolObject);

	return S_OK;
}

void CPool_Manager::Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CPool* pPoolObject)
{
	m_pGameInstance->Push_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, pPoolObject);
}

void CPool_Manager::Clear()
{
	for (auto Pool : m_Pools)
	{
		for (auto PoolObj : Pool.second)
		{
			Safe_Release(PoolObj);
		}
		Pool.second.clear();
	}
	m_Pools.clear();
}


deque<CPool*>* CPool_Manager::Find_Pool(const _wstring& strPoolTag)
{
	auto	iter = m_Pools.begin();

	for (auto iter = m_Pools.begin(); iter != m_Pools.end(); )
	{
		if (iter->first == strPoolTag)
		{
			return &(iter->second);
		}
	}

	return nullptr;
}

CPool_Manager* CPool_Manager::Create()
{
	return nullptr;
}

void CPool_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
	Clear();
}
