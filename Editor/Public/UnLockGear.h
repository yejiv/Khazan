#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

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

    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE };

private:
    virtual HRESULT Ready_Components(void* pArg) override;

public:
    static CUnLockGear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END