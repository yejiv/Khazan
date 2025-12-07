#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CLever_Gear final : public CProp_Interactive
{
private:
    enum MESH_TYPE { GEAR, RUNE };

    enum ANIM_STATE
    {
        ACTIVATION1,
        ACTIVATION2,
        IDLE1,
        IDLE2,
        END
    };

public:
    typedef struct tagLeverGearDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }LEVER_GEAR_DESC;

private:
    CLever_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLever_Gear(const CLever_Gear& Prototype);
    virtual ~CLever_Gear() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE1 };

    EventGateGear m_EventGate = {};
    _uint m_iEventID = { 0 };

private:
    virtual HRESULT Ready_Components(void* pArg) override;

    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

public:
    static CLever_Gear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END