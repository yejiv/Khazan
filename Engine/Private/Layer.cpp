#include "Layer.h"
#include "GameInstance.h"
#include "GameObject.h"



CLayer::CLayer()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;
	
	return (*iter)->Get_Component(strComponentTag);
}

CGameObject* CLayer::Get_GameObject(_uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return *iter;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Update(fTimeDelta);
	}
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Update(fTimeDelta);
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); )
	{
		if ((*it)->Get_IsDead() && !(*it)->Get_IsPool())
		{
			m_DeadGameObjects.push_back((*it));
			it = m_GameObjects.erase(it);
		}
		else if ((*it)->Get_IsDead() && (*it)->Get_IsPool())
		{
			m_pGameInstance->Reset_PoolObject((*it));
			it = m_GameObjects.erase(it);
		}
		else
		{
			if (nullptr != (*it))
				(*it)->Late_Update(fTimeDelta);
			++it;
		}

	}
}

void CLayer::DeadObject_Clear()
{
	for (auto& pGameObject : m_DeadGameObjects)
		Safe_Release(pGameObject);

	m_DeadGameObjects.clear();
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_DeadGameObjects)
		Safe_Release(pGameObject);

	m_DeadGameObjects.clear();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();

	Safe_Release(m_pGameInstance);
	
}
