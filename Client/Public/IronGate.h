#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CIronGate final : public CProp_Interactive
{
private:
    enum class ANIM_STATE { ACTIVATION, ACTIVATION_AUTO, IDLE1, IDLE2, END };

public:
    typedef struct tagIronGateDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {

    }IRONGATE_DESC;

    // 플레이어 포지션 Position_Ch or IA_BeginLoc_Open

private:
    CIronGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CIronGate(const CIronGate& Prototype);
    virtual ~CIronGate() = default;

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

    _bool m_isUnLock = { false };

    _float m_fColTimeAcc = { 0.f };

    _uint m_iEventID = { 0 };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Interaction_Guide(void* pArg);

    void Input_Interact_Event(_float fTimeDelta);
    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    static CIronGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END