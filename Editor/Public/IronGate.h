#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

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
    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE1 };
    _bool m_isUnLock = { false };
    _bool m_isState = { false };

    _float m_fTimeAcc = { 0.f };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);

public:
    static CIronGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END