#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CUnLockGear final : public CProp_Interactive
{
private:
    enum ANIM_STATE
    {
        STOP,
        IDLE,
        START,
        END
    };

    // Position_Ch

public:
    typedef struct tagUnLockGearDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }UNLOCK_GEAR_DESC;

private:
    CUnLockGear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUnLockGear(const CUnLockGear& Prototype);
    virtual ~CUnLockGear() = default;

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
    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE };

    EventObject m_Event = {};

    _float3 m_vCharacterPosition = {};

    _float m_fColTimeAcc = { 0.f };

    EventHallElevator m_EventHallElevator = {};

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
    static CUnLockGear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END