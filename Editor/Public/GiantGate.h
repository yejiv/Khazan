#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CGiantGate final : public CProp_Interactive
{
private:
    enum class ANIM_STATE { OPENED, CLOSED, OPENNING, END };

public:
    typedef struct tagGiantGateDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {

    }GIANTGATE_DESC;

    // 플레이어 포지션 Position_Ch or IA_BeginLoc_Open

private:
    CGiantGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGiantGate(const CGiantGate& Prototype);
    virtual ~CGiantGate() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    ANIM_STATE m_eAnimState = { ANIM_STATE::CLOSED };
    _bool m_isUnLock = { false };
    _bool m_isState = { false };

    _float m_fTimeAcc = { 0.f };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);

public:
    static CGiantGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END