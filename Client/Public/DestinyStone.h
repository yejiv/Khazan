#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CDestinyStone final : public CProp_Interactive
{
public:
    typedef struct tagDestinyStoneDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {

    }DESTINYSTONE_DESC;

private:
    CDestinyStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDestinyStone(const CDestinyStone& Prototype);
    virtual ~CDestinyStone() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    CBody* m_pStaticCom = { nullptr };
    CBody* m_pTriggerCom = { nullptr };

    class CInteraction_Guide* m_pGuide = { nullptr };

private:
    COLLISION_DESC m_TriggerCollisionDesc;

private:
    EventObject m_Event = {};

    _bool m_isInteracted = { false };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Interaction_Guide(void* pArg);
    HRESULT Ready_PartObjects(void* pArg);

    void Event_Update(_float fTimeDelta);
    void Input_Interact_Event(_float fTimeDelta);
    HRESULT Bind_Materials(_uint iMeshIndex) override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    static CDestinyStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END