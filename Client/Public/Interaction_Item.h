#pragma once

#include "Client_Defines.h"
#include "Interaction_Object.h"

NS_BEGIN(Engine)
class CShader;
class CBody;
NS_END

NS_BEGIN(Client)

class CInteraction_Item final : public CInteraction_Object
{
public:
    enum ItemType { NORMAL, SPECIAL, END };
    typedef struct ItemInfo {
        _uint iItemIndex;
        _uint iGrade;
    }ITEM_INFO;

private:
    CInteraction_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CInteraction_Item(const CInteraction_Item& Prototype);
    virtual ~CInteraction_Item() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    void Ready_Item(_uint iItemIndex, _vector vPos);
    void RandNormal_Item(_vector vPos);
    void Special_Item(_wstring strNameTag, _vector vPos);
    
public:
    void Item_Check();

public:
    virtual void Reset() override;

private:
    class CEffect_Prefab*   m_pEffect = { nullptr };
    _uint m_iItemIndex = {};
    _uint m_iItemGrade = {};
    _uint m_iItemType = {};
    class CInteraction_Guide* m_pGuide = { nullptr };
    _bool m_isGuideVisible = { false };


    _bool m_isShow = { false };


private:
    vector<ITEM_INFO> m_NormalItem;
    map<_wstring, ITEM_INFO> m_SpecialItem;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    HRESULT Ready_Components(void* pArg);
    HRESULT Bind_ShaderResources(void* pArg);
    HRESULT Ready_Effect(_uint iGrade);
    HRESULT Ready_Guide();
    HRESULT Ready_Collision();


private:
    void Push_NormalItem(_uint iItemIndex, _uint iGrade);
    void Push_SpecialItem(_wstring strName, _uint iItemIndex, _uint iGrade);

public:
    static CInteraction_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END