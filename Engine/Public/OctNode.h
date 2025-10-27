#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL COctNode : public CBase
{
public:
    typedef struct OctItem
    {
        class CGameObject* pObject = nullptr;
        AABB_DESC         Bounds;
    };

    // 프러스텀 판정 결과
    enum class FrustumCheck : _uint
    {
        Outside = 0,
        Intersect,
        Inside
    };

private:
    COctNode();
    virtual ~COctNode() = default;

public:
    HRESULT Initialize(const AABB_DESC& tDesc, _uint iDepth, _uint iMaxDepth, _uint iCapacityPerNode, _float fLoosness);
    _bool Insert(const OctItem& Item);
    _bool Remove(class CGameObject* pObject);

    void QueryVisible(const class CFrustum* pFrustum, vector<CGameObject*>& OutObjects) const;

    void Clear();
    _uint GetItemCountRecursive() const;

private:
    AABB_DESC				m_Bound_Tight = {};
    AABB_DESC               m_Bound_Loose = {};

    _uint                   m_iDepth = {};
    _uint                   m_iMaxDepth = 3;
    _uint                   m_iCapacity = 8;
    _float                  m_fLooseness = 1.25f;


    vector<OctItem>         m_Items = {};
    COctNode*               m_Children[8];

private:
    _bool IsNear() const;
    void Subdivide();

    _uint ChildIndexFor(const AABB_DESC& tBox) const;

    static _float PlaneDistanceAABB(const _float4& vPlane, const AABB_DESC& tBox);
    static FrustumCheck BoxInFrustum(const AABB_DESC& tBox, const _float4 vPlane[6]);

	void QueryVisible_Internal(const _float4 vPlane[6], vector<CGameObject*>& OutObjects) const;

public:
    static COctNode* Create(const AABB_DESC& tDesc, _uint iMaxDepth, _uint iCapacityPerNode, _float fLooseness);
    virtual void Free() override;


};

NS_END
