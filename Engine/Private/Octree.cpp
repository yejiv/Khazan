#include "Octree.h"
#include "OctNode.h"
#include "Frustum.h"

COctree::COctree()
{
}

HRESULT COctree::Initialize(const AABB_DESC& tWorldBounds, _uint iMaxDepth, _uint iCapacityPerNode, _float fLooseness)
{
	m_iMaxDepth = iMaxDepth;
	m_iCapacityPerNode = iCapacityPerNode;
	m_fLooseness = fLooseness;

	m_pRootNode = COctNode::Create(tWorldBounds, m_iMaxDepth, m_iCapacityPerNode, m_fLooseness);
	if (m_pRootNode == nullptr)
		return E_FAIL;

	return S_OK;
}

_bool COctree::Insert(CGameObject* pObject, const AABB_DESC& tWorldBoundsOfObject)
{
	if (m_pRootNode == nullptr || pObject == nullptr)
		return false;

	COctNode::OctItem item{};
	item.pObject = pObject;
	item.Bounds = tWorldBoundsOfObject;

	if (!m_pRootNode->Insert(item))
		return false;

	m_ObjectToNode[pObject] = nullptr;

	return true;
}

_bool COctree::Remove(CGameObject* pObject)
{
	if (m_pRootNode == nullptr || pObject == nullptr)
		return false;

	const _bool removed = m_pRootNode->Remove(pObject);
	if (removed)
		m_ObjectToNode.erase(pObject);

	return removed;
}

_bool COctree::UpdateObject(CGameObject* pObject, const AABB_DESC& tNewWorldBoundsOfObject)
{
	if (pObject == nullptr)
		return false;

	// 단순 Remove + Insert (필요하면 노드 포인터 보관 최적화를 추가 가능)
	Remove(pObject);
	return Insert(pObject, tNewWorldBoundsOfObject);
}

void COctree::QueryVisible(CFrustum* pFrustum, vector<class CGameObject*>& outVisibleObjects)
{
	outVisibleObjects.clear();

	if (m_pRootNode == nullptr || pFrustum == nullptr)
		return;

	m_pRootNode->QueryVisible(pFrustum, outVisibleObjects);
}

void COctree::Clear()
{
}

COctree* COctree::Create(const AABB_DESC& tWorldBounds, _uint iMaxDepth, _uint iCapacityPerNode, _float fLooseness)
{
	COctree* pInstance = new COctree();

	if (FAILED(pInstance->Initialize(tWorldBounds, iMaxDepth, iCapacityPerNode, fLooseness)))
	{
		MSG_BOX(TEXT("Failed to Created : COctree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COctree::Free()
{
	__super::Free();
	if (m_pRootNode)
	{
		m_pRootNode->Clear();
		Safe_Release(m_pRootNode);
		m_pRootNode = nullptr;
	}
	m_ObjectToNode.clear();
}
