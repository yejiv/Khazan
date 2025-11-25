#pragma once
#include "Client_Defines.h"
#include "Prop_Destructible.h"

NS_BEGIN(Client)

class CBarrel final : public CProp_Destructible
{
public:
    typedef struct tagPropFenceDesc : public CProp_Destructible::PROP_DEST_DESC
    {

    }PROP_FENCE_DESC;
private:
    CBarrel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBarrel(const CBarrel& Prototype);
    virtual ~CBarrel() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;


public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    class CBody* m_pBodyCom = { nullptr };

    _bool m_isDestruct = { false };

private:
    HRESULT Ready_Components(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Chunk(void* pArg);

public:
    static CBarrel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void                Free() override;
};
NS_END