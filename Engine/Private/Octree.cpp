#include "Octree.h"
#include "Frustum.h"
#include "GameObject.h"

static inline bool ContainsSphereFully(const BoundingBox& box, const XMFLOAT3& p, float r)
{
	const XMFLOAT3& c = box.Center;
	const XMFLOAT3& e = box.Extents;
	return (fabsf(p.x - c.x) + r <= e.x) &&
		(fabsf(p.y - c.y) + r <= e.y) &&
		(fabsf(p.z - c.z) + r <= e.z);
}

COctree::COctree()
	: m_pParent(nullptr),
	m_iDepth(0),
	m_isVisible(false),
	m_isObtainStatic(false)
{
	for (int i = 0; i < CHILDEND; ++i) m_pChilds[i] = nullptr;
}

COctree::~COctree()
{
}

HRESULT COctree::Initialize(const _float3& vCenter, const _float& fHalfWidth, const _int& iDepthLimit, COctree* pParent)
{
	if (pParent)
		m_pParent = pParent;

	memcpy_s(&m_BoundingBox.Center, sizeof(_float3), &vCenter, sizeof(_float3));
	memcpy_s(&m_BoundingBox.Extents.x, sizeof(_float), &fHalfWidth, sizeof(_float));
	memcpy_s(&m_BoundingBox.Extents.y, sizeof(_float), &fHalfWidth, sizeof(_float));
	memcpy_s(&m_BoundingBox.Extents.z, sizeof(_float), &fHalfWidth, sizeof(_float));
	memcpy_s(&m_iDepth, sizeof(_int), &iDepthLimit, sizeof(_int));

	_int childDepth = iDepthLimit - 1;
	if (childDepth < 0)
	{
		return S_OK;
	}

	_float vHalfWidth{ fHalfWidth * 0.5f };

	_float vRight{ vCenter.x + vHalfWidth };
	_float vLeft{ vCenter.x - vHalfWidth };
	_float vTop{ vCenter.y + vHalfWidth };
	_float vBottom{ vCenter.y - vHalfWidth };
	_float vFront{ vCenter.z + vHalfWidth };
	_float vBack{ vCenter.z - vHalfWidth };

	m_pChilds[LBB] = COctree::Create({ vLeft,vBottom,vBack }, vHalfWidth, childDepth, this);
	m_pChilds[RBB] = COctree::Create({ vRight,vBottom,vBack }, vHalfWidth, childDepth, this);
	m_pChilds[LTB] = COctree::Create({ vLeft,vTop,vBack }, vHalfWidth, childDepth, this);
	m_pChilds[RTB] = COctree::Create({ vRight,vTop,vBack }, vHalfWidth, childDepth, this);

	m_pChilds[LBF] = COctree::Create({ vLeft,vBottom,vFront }, vHalfWidth, childDepth, this);
	m_pChilds[RBF] = COctree::Create({ vRight,vBottom,vFront }, vHalfWidth, childDepth, this);
	m_pChilds[LTF] = COctree::Create({ vLeft,vTop,vFront }, vHalfWidth, childDepth, this);
	m_pChilds[RTF] = COctree::Create({ vRight,vTop,vFront }, vHalfWidth, childDepth, this);
	return S_OK;
}

void COctree::Priority_Update(_float fTimeDelta)
{
	if (m_isObtainStatic && m_isVisible)
	{
		if (!m_GameObjects.empty())
		{
			for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
			{
				if ((*iter)->Get_IsActive())
				{
					(*iter)->Priority_Update(fTimeDelta);
				}
				else
				{
					Safe_Release(*iter);
					iter = m_GameObjects.erase(iter);
					continue;
				}
				iter++;
			}
		}
		if (m_iDepth)
		{
			for (int Child = LBB; Child < CHILDEND; Child++)
			{
				m_pChilds[Child]->Priority_Update(fTimeDelta);
			}
		}
	}
}

void COctree::Update(_float fTimeDelta)
{
	if (m_isObtainStatic && m_isVisible)
	{
		if (!m_GameObjects.empty())
		{
			for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
			{
				if ((*iter)->Get_IsActive())
				{
					(*iter)->Update(fTimeDelta);
				}
				else
				{
					Safe_Release(*iter);
					iter = m_GameObjects.erase(iter);
					continue;
				}
				iter++;
			}
		}
		if (m_iDepth)
		{
			for (int Child = LBB; Child < CHILDEND; Child++)
			{
				m_pChilds[Child]->Update(fTimeDelta);
			}
		}
	}
}

void COctree::Late_Update(_float fTimeDelta)
{
	if (m_isObtainStatic && m_isVisible)
	{
		if (!m_GameObjects.empty())
		{
			for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
			{
				if ((*iter)->Get_IsActive())
				{
					(*iter)->Late_Update(fTimeDelta);
				}
				else
				{
					Safe_Release(*iter);
					iter = m_GameObjects.erase(iter);
					continue;
				}
				iter++;
			}
		}
		if (m_iDepth)
		{
			for (int Child = LBB; Child < CHILDEND; Child++)
			{
				m_pChilds[Child]->Late_Update(fTimeDelta);
			}
		}
	}
}

bool COctree::AddStaticObject(CGameObject* pGameObject, const _float3& vPoint, const _float& fRadius)
{
	lock_guard<recursive_mutex> lock(m_Mutex);

	ContainmentType Containment = (fRadius == 0.0f ? m_BoundingBox.Contains(XMLoadFloat3(&vPoint)) : m_BoundingBox.Contains(BoundingSphere(vPoint, fRadius)));
	if (CONTAINS == Containment)
	{
		m_isObtainStatic = true;
		if (m_iDepth)
		{
			int ChildIndex{ 0 };
			if (m_BoundingBox.Center.x <= vPoint.x)
				ChildIndex |= 1;
			if (m_BoundingBox.Center.y <= vPoint.y)
				ChildIndex |= 2;
			if (m_BoundingBox.Center.z <= vPoint.z)
				ChildIndex |= 4;

			if (m_pChilds[ChildIndex]->AddStaticObject(pGameObject, vPoint, fRadius))
				return true;

			Safe_AddRef(pGameObject);
			m_GameObjects.emplace_back(pGameObject);
			return true;
		}
		else
		{
			Safe_AddRef(pGameObject);
			m_GameObjects.emplace_back(pGameObject);
			return true;
		}
	}
	else if (!m_pParent && Containment)
	{
		m_isObtainStatic = true;
		Safe_AddRef(pGameObject);
		m_GameObjects.emplace_back(pGameObject);
		return true;
	}

	return false;
}

void COctree::Culling(CFrustum* pFrustum)
{
	if (m_isObtainStatic)
	{
		switch (isDraw(pFrustum))
		{
		case DISJOINT:
		{
			Invisible();
		}
		break;
		case INTERSECTS:
		{
			m_isVisible = true;
			if (m_iDepth)
			{
				for (auto& Child : m_pChilds)
				{
					Child->Culling(pFrustum);
				}
			}
		}
		break;
		case CONTAINS:
		{
			AllVisible();
		}
		break;
		}
	}
}

void COctree::Clear()
{


}

void COctree::Destroy()
{
	 for (int i = 0; i < CHILDEND; ++i)
    {
        if (m_pChilds[i])
        {
            Safe_Release(m_pChilds[i]); // 내부적으로 Free() 호출될 것으로 가정
            m_pChilds[i] = nullptr;
        }
    }
}

ContainmentType COctree::isDraw(CFrustum* pFrustum)
{
	return pFrustum->isIn_WorldSpace(m_BoundingBox);
}

void COctree::AllVisible()
{
	m_isVisible = true;
	if (m_iDepth)
	{
		for (auto& Child : m_pChilds)
		{
			Child->AllVisible();
		}
	}
}

void COctree::Invisible()
{
	m_isVisible = false;
	/*if (m_iDepth)
	{
		for (auto& Child : m_pChilds)
		{
			Child->Invisible();
		}
	}*/
}

vector<COctree::tInstance>* COctree::FindInstance(const string& _strModelTag)
{
	auto iter = m_Instances.find(_strModelTag);
	if (iter == m_Instances.end())
		return nullptr;

	return &iter->second;
}


COctree* COctree::Create(const _float3& vCenter, const _float& fHalfWidth, const _int& iDepthLimit, COctree* pParent)
{
	COctree* pInstance = new COctree();

	if (FAILED(pInstance->Initialize(vCenter, fHalfWidth, iDepthLimit, pParent)))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void COctree::Free()
{
	__super::Free();

	// 1) 내가 가진 게임오브젝트 해제
	for (auto* p : m_GameObjects) Safe_Release(p);
	m_GameObjects.clear();

	// 2) 인스턴스 안의 게임오브젝트 해제
	for (auto& kv : m_Instances)
		for (auto& inst : kv.second)
			Safe_Release(inst.pGameObject);
	m_Instances.clear();

	// 3) 자식 노드 해제 (각 자식의 Free에서 자기 것 정리)
	Destroy();

	m_isVisible = false;
	m_isObtainStatic = false;

	
}