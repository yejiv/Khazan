#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CLever final : public CProp_Interactive
{
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

    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE1 };

private:
    virtual HRESULT Ready_Components(void* pArg) override;

public:
    static CLever* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END