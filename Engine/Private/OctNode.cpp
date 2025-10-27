#include "OctNode.h"
#include "Frustum.h"

COctNode::COctNode()
{
}

HRESULT COctNode::Initialize(const AABB_DESC& tDesc, _uint iDepth, _uint iMaxDepth, _uint iCapacityPerNode, _float fLoosness)
{
	m_Bound_Tight = tDesc;
	m_iDepth = iDepth;
	m_iMaxDepth = iMaxDepth;
	m_iCapacity = iCapacityPerNode;
	m_fLooseness = fLoosness;


	m_Bound_Loose.vCenter = m_Bound_Tight.vCenter;
	m_Bound_Loose.vExtents = _float3(
		m_Bound_Tight.vExtents.x * m_fLooseness,
		m_Bound_Tight.vExtents.y * m_fLooseness,
		m_Bound_Tight.vExtents.z * m_fLooseness
	);

	return S_OK;
}

_bool COctNode::Insert(const OctItem& Item)
{
	const _float3& vCenter = m_Bound_Loose.vCenter;
	const _float3& vExtents = m_Bound_Loose.vExtents;

    if (fabsf(Item.Bounds.vCenter.x - vCenter.x) > (Item.Bounds.vExtents.x + vExtents.x)) return false;
    if (fabsf(Item.Bounds.vCenter.y - vCenter.y) > (Item.Bounds.vExtents.y + vExtents.y)) return false;
    if (fabsf(Item.Bounds.vCenter.z - vCenter.z) > (Item.Bounds.vExtents.z + vExtents.z)) return false;

    if (IsNear() && (m_Items.size() < m_iCapacity || m_iDepth >= m_iMaxDepth))
    {
		m_Items.push_back(Item);
		return true;
    }

    if (m_Children[0] == nullptr)
		Subdivide();

	const _uint idx = ChildIndexFor(Item.Bounds);
    if (idx < 8 && m_Children[idx] && m_Children[idx]->Insert(Item))
        return true;

	m_Items.push_back(Item);
	return true;
}

_bool COctNode::Remove(CGameObject* pObject)
{
    for (auto it = m_Items.begin(); it != m_Items.end(); it++)
    {
        if (it->pObject == pObject)
        {
            m_Items.erase(it);
            return true;
        }
    }

    for (COctNode* pChild : m_Children)
    {
        if (!pChild) continue;
        if (pChild->Remove(pObject)) return true;
    }
    return false;
}

void COctNode::QueryVisible(const CFrustum* pFrustum, vector<CGameObject*>& OutObjects) const
{
    const _float4* planes = pFrustum->Get_WorldPlanes();
    QueryVisible_Internal(planes, OutObjects);
}

void COctNode::Clear()
{
}

_uint COctNode::GetItemCountRecursive() const
{
    _uint iCnt = (uint)m_Items.size();
    for (COctNode* pChild : m_Children)
        if (pChild) iCnt += pChild->GetItemCountRecursive();
    return iCnt;
}

_bool COctNode::IsNear() const
{
	return m_iDepth >= m_iMaxDepth || (m_Children[0] == nullptr);
}

void COctNode::Subdivide()
{
    if (m_iDepth >= m_iMaxDepth) return;
    if (m_Children[0] != nullptr) return;

    const _float3& vCenter = m_Bound_Tight.vCenter;
    const _float3& vExtents = m_Bound_Tight.vExtents;
    const _float3  vChildExt{ vExtents.x * 0.5f, vExtents.y * 0.5f, vExtents.z * 0.5f };

    // 8자식 (x,y,z) 부호 패턴
    static const int s[8][3] = {
        {-1,+1,-1}, {+1,+1,-1}, {+1,-1,-1}, {-1,-1,-1},
        {-1,+1,+1}, {+1,+1,+1}, {+1,-1,+1}, {-1,-1,+1}
    };

    for (_uint i = 0; i < 8; ++i)
    {
        _float3 vChildCenter{
            vCenter.x + vChildExt.x * (float)s[i][0],
            vCenter.y + vChildExt.y * (float)s[i][1],
            vCenter.z + vChildExt.z * (float)s[i][2]
        };

        COctNode* pChild = new COctNode();
        AABB_DESC childDesc{};
        childDesc.vCenter = vChildCenter;
        childDesc.vExtents = vChildExt;

        pChild->Initialize(childDesc, m_iDepth + 1, m_iMaxDepth, m_iCapacity, m_fLooseness);
        m_Children[i] = pChild;
    }

    // 기존 아이템을 자식으로 재분배 (완전히 들어가는 경우에만)
    vector<OctItem> remain;
    remain.reserve(m_Items.size());
    for (const auto& it : m_Items)
    {
        _uint idx = ChildIndexFor(it.Bounds);
        if (idx < 8 && m_Children[idx])
            m_Children[idx]->Insert(it);
        else
            remain.push_back(it);
    }
    m_Items.swap(remain);
}

_uint COctNode::ChildIndexFor(const AABB_DESC& tBox) const
{
	const _float3& vCenter = m_Bound_Tight.vCenter;

    const _bool isRight = (tBox.vCenter.x >= vCenter.x);
    const _bool isTop = (tBox.vCenter.y >= vCenter.y);
    const _bool isFront = (tBox.vCenter.z >= vCenter.z);

    const _uint idx = (isRight ? 1u : 0u) | (isTop ? 2u : 0u) | (isFront ? 4u : 0u);

    const _float3 vChildExtents = {
        m_Bound_Tight.vExtents.x * 0.5f,
        m_Bound_Tight.vExtents.y * 0.5f,
        m_Bound_Tight.vExtents.z * 0.5f
    };
    const _float3 vChildCenter = {
        vCenter.x + vChildExtents.x * (isRight ? +1.f : -1.f),
        vCenter.y + vChildExtents.y * (isTop ? +1.f : -1.f),
        vCenter.z + vChildExtents.z * (isFront ? +1.f : -1.f)
    };

    if (fabsf(tBox.vCenter.x - vChildCenter.x) + tBox.vExtents.x > vChildExtents.x) return 8;
    if (fabsf(tBox.vCenter.y - vChildCenter.y) + tBox.vExtents.y > vChildExtents.y) return 8;
    if (fabsf(tBox.vCenter.z - vChildCenter.z) + tBox.vExtents.z > vChildExtents.z) return 8;


	return idx;
}

_float COctNode::PlaneDistanceAABB(const _float4& vPlane, const AABB_DESC& tBox)
{
    // plane: (nx, ny, nz, d)
    _vector n = XMVectorSet(vPlane.x, vPlane.y, vPlane.z, 0.f);
    _vector vCenter = XMLoadFloat3(&tBox.vCenter);
    _vector vExtents = XMLoadFloat3(&tBox.vExtents);

    _vector absn = XMVectorAbs(n);
    _vector r = XMVector3Dot(absn, vExtents);
    _float s = XMVectorGetX(XMVector3Dot(n, vCenter)) + vPlane.w;

    if (s + XMVectorGetX(r) < 0.f) return -1.f; // Outside (완전 음측)
    if (s - XMVectorGetX(r) > 0.f) return +1.f; // Inside  (완전 양측)
    return 0.f;                                  // Intersect
}

COctNode::FrustumCheck COctNode::BoxInFrustum(const AABB_DESC& tBox, const _float4 vPlane[6])
{
    _bool anyIntersect = false;
    for (int i = 0; i < 6; ++i)
    {
        const _float c = PlaneDistanceAABB(vPlane[i], tBox);
        if (c < 0.f) return FrustumCheck::Outside;
        if (c == 0.f) anyIntersect = true;
    }
    return anyIntersect ? FrustumCheck::Intersect : FrustumCheck::Inside;
}

void COctNode::QueryVisible_Internal(const _float4 vPlane[6], vector<CGameObject*>& OutObjects) const
{
    // 먼저 노드의 루즈 AABB를 검사해서 빠르게 거름
    const FrustumCheck chk = BoxInFrustum(m_Bound_Loose, vPlane);
    if (chk == FrustumCheck::Outside) return;

    if (chk == FrustumCheck::Inside)
    {
        // 노드 전체가 프러스텀 내부 → 노드의 아이템 전부 통과
        for (const auto& it : m_Items)
            OutObjects.push_back(it.pObject);
    }
    else
    {
        // 교차 → 아이템 개별 검사
        for (const auto& it : m_Items)
        {
            if (BoxInFrustum(it.Bounds, vPlane) != FrustumCheck::Outside)
                OutObjects.push_back(it.pObject);
        }
    }

    // 자식 재귀
    for (COctNode* pChild : m_Children)
    {
        if (!pChild) continue;
        pChild->QueryVisible_Internal(vPlane, OutObjects);
    }
}

COctNode* COctNode::Create(const AABB_DESC& tDesc, _uint iMaxDepth, _uint iCapacityPerNode, _float fLooseness)
{
	COctNode* pInstance = new COctNode();

	if (FAILED(pInstance->Initialize(tDesc, 0, iMaxDepth, iCapacityPerNode, fLooseness)))
	{
		MSG_BOX(TEXT("Failed to Created : COctNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COctNode::Free()
{
    m_Items.clear();
    for (COctNode*& pChild : m_Children)
    {
        if (!pChild) continue;
        pChild->Clear();
        Safe_Release(pChild);
        pChild = nullptr;
    }

    __super::Free();
}
