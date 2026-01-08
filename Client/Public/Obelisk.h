#pragma once
#include "Client_Defines.h"
#include "Prop_Destructible.h"

NS_BEGIN(Client)

class CObelisk final : public CProp_Destructible
{
public:
    typedef struct tagPropFenceDesc : public CProp_Destructible::PROP_DEST_DESC
    {

    }PROP_FENCE_DESC;
private:
    CObelisk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CObelisk(const CObelisk& Prototype);
    virtual ~CObelisk() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Destory();

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    _bool m_isDestruct = { false };
    queue<_uint> m_DestoryQueue;

    _bool m_isDestroy = { false };
    _float m_fDestoryTime = {};

    _bool m_isExplosion = { false };
    _bool m_isVortex = { false };

    _uint m_iEventID = {};
    
private:
    HRESULT Ready_Components(void* pArg);
    HRESULT Ready_Chunk(void* pArg);

public:
    static CObelisk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void                Free() override;
};
NS_END