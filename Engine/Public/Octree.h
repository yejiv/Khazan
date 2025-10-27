#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL COctree : public CBase
{
private:
	COctree();
	virtual ~COctree() = default;

public:
	HRESULT Initialize(const AABB_DESC& tWorldBounds, _uint iMaxDepth = 6, _uint iCapacityPerNode = 8, _float fLooseness = 1.25f);


public:
	_bool Insert(class CGameObject* pObject, const AABB_DESC& tWorldBoundsOfObject);
	_bool Remove(class CGameObject* pObject);
	_bool UpdateObject(class CGameObject* pObject, const AABB_DESC& tNewWorldBoundsOfObject);

	void QueryVisible(class CFrustum* pFrustum, vector<class CGameObject*>& outVisibleObjects);

	void Clear();


private:
	class COctNode* m_pRootNode = nullptr;
	unordered_map<class CGameObject*, class COctNode*> m_ObjectToNode;

	_uint m_iMaxDepth = 6;
	_uint m_iCapacityPerNode = 8;
	_float m_fLooseness = 1.25f;

public:
	static COctree* Create(const AABB_DESC& tWorldBounds, _uint iMaxDepth = 6, _uint iCapacityPerNode = 8, _float fLooseness = 1.25f);
	virtual void Free() override;

};

NS_END