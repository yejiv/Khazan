#include "Pool_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

CPool_Manager::CPool_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPool_Manager::Add_PoolObject(_uint iPrototypeLevelIndex, const _wstring strPrototypeTag, const _wstring& strPoolTag, void* pArg, _uint iCount)
{
	for (size_t i = 0; i < iCount; i++)
	{
		CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
		if (nullptr == pGameObject)
			return E_FAIL;

		pGameObject->Set_Tag(strPoolTag);

		deque<CGameObject*>* pPool = Find_Pool(strPoolTag);
		if (nullptr == pPool)
		{
			deque<CGameObject*> dePool;
			dePool.push_back(pGameObject);
			m_Pools.emplace(strPoolTag, dePool);
		}
		else
			pPool->push_back(pGameObject);
	}


	return S_OK;
}

CGameObject* CPool_Manager::Pop_PoolObject(const _wstring& strPoolTag)
{
	deque<CGameObject*>* pPool = Find_Pool(strPoolTag);

	if (pPool == nullptr || pPool->size() <= 0)
		return nullptr;

	CGameObject* pGameObject = pPool->front();

	pPool->pop_front();

	return pGameObject;
}


HRESULT CPool_Manager::Reset_PoolObject(CGameObject* pGameObject)
{
	if (!pGameObject->Get_IsDead())
		return E_FAIL;

	pGameObject->Set_IsDead(false);
	pGameObject->Reset();

	deque<CGameObject*>* pPool = Find_Pool(pGameObject->Get_Tag());

	if (pPool == nullptr || pPool->size() <= 0)
		return E_FAIL;

	pPool->push_back(pGameObject);

	return S_OK;
}

void CPool_Manager::Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	m_pGameInstance->Push_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, pGameObject);
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


deque<CGameObject*>* CPool_Manager::Find_Pool(const _wstring& strPoolTag)
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
	return new CPool_Manager();
}

void CPool_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
	Clear();
}
