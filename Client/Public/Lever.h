#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CLever final : public CProp_Interactive
{
    // 0, 1 : 시작 지점 레버, 기어, 문 연동 이벤트

private:
    enum ANIM_STATE
    {
        ACTIVATION,
        DEACTIVATION,
        IDLE1,
        IDLE2,
        END
    };

public:
    typedef struct tagLeverDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }LEVER_DESC;

private:
    CLever(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLever(const CLever& Prototype);
    virtual ~CLever() = default;

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
    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE1 };

    EventObject m_Event = {};

    _float3 m_vCharacterPosition = {};

    _float m_fColTimeAcc = { 0.f };

    EventGateGear m_EventGate = {};

    _uint m_iObjectInteractEventID = {};
    _uint m_iEventID = { 0 };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Interaction_Guide(void* pArg);

    void Input_Interact_Event(_float fTimeDelta);
    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

public:
    static CLever* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END