#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CDestructible_Stone final : public CGameObject
{
public:
    typedef struct tagStoneDesc : public CGameObject::GAMEOBJECT_DESC
    {
        _vector vPos;
    }STONE_DESC;
private:
    CDestructible_Stone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDestructible_Stone(const CDestructible_Stone& Prototype);
    virtual ~CDestructible_Stone() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_Pos(_vector vPos);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    _bool m_isDestruct = { false };
    _uint m_iIndex = {};

    vector<class CChunk*> m_Chunks;

    _bool m_isPlayDestroy = { false };
    _float m_fDestroyVolume = { 5.f };

private:
    HRESULT Ready_Components(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Chunk(void* pArg);

public:
    static CDestructible_Stone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void                Free() override;
};
NS_END