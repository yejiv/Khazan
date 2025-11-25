#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CVerticalGate final : public CProp_Interactive
{
private:
    enum class ANIM_STATE { ACTIVATION, DEACTIVATION, IDLE1, IDLE2, END };

    // 0번 이벤트 ID ( 첫번째 퍼즐 문 ) | 1번 이벤트 ID ( 두번째 퍼즐 문 * 2 EA )

public:
    typedef struct tagVerticalGateDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {

    }VERTICAL_GATE_DESC;

private:
    CVerticalGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVerticalGate(const CVerticalGate& Prototype);
    virtual ~CVerticalGate() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE1 };

    EventGimmick m_EventGimmick = {};

    _bool m_isUnLock = { false };

    _float m_fTimeAcc = { 0.f };

private:
    virtual HRESULT Ready_Components(void* pArg) override;

    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

public:
    static CVerticalGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END