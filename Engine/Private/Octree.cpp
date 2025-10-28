#include "Octree.h"
#include "GameInstance.h"
#include "GameObject.h"

COctree::COctree()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
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

	m_pChilds[LBB] = COctree::Create({ vLeft,vBottom,vBack }, vHalfWidth, childDepth, pParent);
	m_pChilds[RBB] = COctree::Create({ vRight,vBottom,vBack }, vHalfWidth, childDepth, pParent);
	m_pChilds[LTB] = COctree::Create({ vLeft,vTop,vBack }, vHalfWidth, childDepth, pParent);
	m_pChilds[RTB] = COctree::Create({ vRight,vTop,vBack }, vHalfWidth, childDepth, pParent);

	m_pChilds[LBF] = COctree::Create({ vLeft,vBottom,vFront }, vHalfWidth, childDepth, pParent);
	m_pChilds[RBF] = COctree::Create({ vRight,vBottom,vFront }, vHalfWidth, childDepth, pParent);
	m_pChilds[LTF] = COctree::Create({ vLeft,vTop,vFront }, vHalfWidth, childDepth, pParent);
	m_pChilds[RTF] = COctree::Create({ vRight,vTop,vFront }, vHalfWidth, childDepth, pParent);
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
					/*if ((*iter)->GetDistanceFromCam() < 40.0f)
					{*/
					//(*iter)->SetEnable(true);
					(*iter)->Priority_Update(fTimeDelta);
					//}
					/*else
						(*iter)->SetEnable(false);*/
				}
				else
				{
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
					//if ((*iter)->IsEnabled())
					(*iter)->Update(fTimeDelta);
				}
				else
				{
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
					//if ((*iter)->IsEnabled())
					(*iter)->Late_Update(fTimeDelta);
				}
				else
				{
					iter = m_GameObjects.erase(iter);
					continue;
				}
				iter++;
			}
		}
		/*if (!m_Instances.empty())
		{
			
			for (auto& pair : m_Instances)
			{
				for (auto& vecInstance : pair.second)
				{
					for (auto& Info : vecInstance.InstanceInfo)
					{
						if (vecInstance.fRadius < 5.2f)
						{
							if (40.0f > pGameInstance->DistanceFromCam(Info.first) && pGameInstance->isInWorldSpace(Info.first, vecInstance.fRadius))
							{
								vecInstance.pGameObject.lock()->AddInstancingWorldMatrix(Info.second);
							}
						}
						else
						{
							if (pGameInstance->isInWorldSpace(Info.first, vecInstance.fRadius))
							{
								vecInstance.pGameObject.lock()->AddInstancingWorldMatrix(Info.second);
							}
						}
					}
				}
			}
		}*/
		if (m_iDepth)
		{
			for (int Child = LBB; Child < CHILDEND; Child++)
			{
				m_pChilds[Child]->Late_Update(fTimeDelta);
			}
		}
	}
}

bool COctree::AddStaticObject(CGameObject* pGameObject, const _float3& vPoint, const _float& _fRadius)
{
	ContainmentType Containment = (_fRadius == 0.0f ? m_BoundingBox.Contains(XMLoadFloat3(&vPoint)) : m_BoundingBox.Contains(BoundingSphere(vPoint, _fRadius)));
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

			if (m_pChilds[ChildIndex]->AddStaticObject(pGameObject, vPoint, _fRadius))
				return true;

			m_GameObjects.emplace_back(pGameObject);
			return true;
		}
		else
		{
			m_GameObjects.emplace_back(pGameObject);
			return true;
		}
	}
	else if (!m_pParent && Containment)
	{
		m_isObtainStatic = true;
		m_GameObjects.emplace_back(pGameObject);
		return true;
	}

	return false;
}
//
//bool COctree::AddStaticInstance(const string& _strModelTag, shared_ptr<CGameObject> _pGameObject, const _float3& vPoint, const _float4x4& _WorldMatrix, const float& _fRadius)
//{
//	ContainmentType Containment = (_fRadius == 0.0f ? m_BoundingBox.Contains(vPoint) : m_BoundingBox.Contains(BoundingSphere(vPoint, _fRadius)));
//	if (CONTAINS == Containment)
//	{
//		m_isObtainStatic = true;
//		if (m_iDepth)
//		{
//			int ChildIndex{ 0 };
//			if (m_BoundingBox.Center.x <= vPoint.x)
//				ChildIndex |= 1;
//			if (m_BoundingBox.Center.y <= vPoint.y)
//				ChildIndex |= 2;
//			if (m_BoundingBox.Center.z <= vPoint.z)
//				ChildIndex |= 4;
//
//			if (m_pChilds[ChildIndex]->AddStaticInstance(_strModelTag, _pGameObject, vPoint, _WorldMatrix, _fRadius))
//				return true;
//
//			vector<tInstance>* pInstance = FindInstance(_strModelTag);
//			if (pInstance)
//			{
//				for (auto& Instances : *pInstance)
//				{
//					if (Instances.pGameObject.lock() == _pGameObject)
//					{
//						Instances.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//						return true;
//					}
//				}
//				tInstance Temp;
//				Temp.pGameObject = _pGameObject;
//				Temp.fRadius = _fRadius;
//				Temp.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//				(*pInstance).emplace_back(Temp);
//			}
//			else
//			{
//				vector<tInstance> vector;
//				tInstance Temp;
//				Temp.pGameObject = _pGameObject;
//				Temp.fRadius = _fRadius;
//				Temp.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//				vector.emplace_back(Temp);
//				m_Instances.emplace(_strModelTag, vector);
//			}
//			return true;
//		}
//		else
//		{
//			vector<tInstance>* pInstance = FindInstance(_strModelTag);
//			if (pInstance)
//			{
//				for (auto& Instances : *pInstance)
//				{
//					if (Instances.pGameObject.lock() == _pGameObject)
//					{
//						Instances.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//						return true;
//					}
//				}
//				tInstance Temp;
//				Temp.pGameObject = _pGameObject;
//				Temp.fRadius = _fRadius;
//				Temp.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//				(*pInstance).emplace_back(Temp);
//			}
//			else
//			{
//				vector<tInstance> vector;
//				tInstance Temp;
//				Temp.pGameObject = _pGameObject;
//				Temp.fRadius = _fRadius;
//				Temp.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//				vector.emplace_back(Temp);
//				m_Instances.emplace(_strModelTag, vector);
//			}
//			return true;
//		}
//	}
//	else if (!m_pParent.lock() && Containment)
//	{
//		vector<tInstance>* pInstance = FindInstance(_strModelTag);
//		if (pInstance)
//		{
//			for (auto& Instances : *pInstance)
//			{
//				if (Instances.pGameObject.lock() == _pGameObject)
//				{
//					Instances.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//					return true;
//				}
//			}
//			tInstance Temp;
//			Temp.pGameObject = _pGameObject;
//			Temp.fRadius = _fRadius;
//			Temp.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//			(*pInstance).emplace_back(Temp);
//		}
//		else
//		{
//			vector<tInstance> vector;
//			tInstance Temp;
//			Temp.pGameObject = _pGameObject;
//			Temp.fRadius = _fRadius;
//			Temp.InstanceInfo.emplace_back(vPoint, _WorldMatrix);
//			vector.emplace_back(Temp);
//			m_Instances.emplace(_strModelTag, vector);
//		}
//		m_isObtainStatic = true;
//		return true;
//	}
//
//	return false;
//}

void COctree::Culling()
{
	if (m_isObtainStatic)
	{
		switch (isDraw())
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
					Child->Culling();
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
	m_GameObjects.clear();
	m_Instances.clear();
	m_isVisible = false;
	m_isObtainStatic = false;
	if (m_iDepth)
	{
		for (auto& Child : m_pChilds)
		{
			Child->Clear();
		}
	}
}

ContainmentType COctree::isDraw()
{
	return m_pGameInstance->isIn_Frustum_WorldSpace(m_BoundingBox);
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
	}

	return pInstance;
}

void COctree::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);

	for (auto Child : m_pChilds)
		Safe_Release(Child);
}