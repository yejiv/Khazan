#pragma once
#include "Client_Defines.h"
#include "Prop.h"

NS_BEGIN(Engine)
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CProp_Destructible : public CProp
{
public:
    typedef struct tagPropDestructibleDesc : public CProp::PROP_DESC
    {
        _uint iIndex;
    }PROP_DEST_DESC;
protected:
    CProp_Destructible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CProp_Destructible(const CProp_Destructible& Prototype);
    virtual ~CProp_Destructible() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual void			Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void			Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void			Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

protected:
    class CBody* m_pBodyCom = { nullptr };

    _bool m_isDestruct = { false };
    _uint m_iIndex = {};

    vector<class CProp_Chunk*> m_Chunks;

protected:
    HRESULT Ready_Components(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Chunk(void* pArg);

public:
    static CProp_Destructible* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void                Free() override;
};
NS_END