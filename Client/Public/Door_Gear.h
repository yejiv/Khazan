#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CDoor_Gear final : public CProp_Interactive
{
private:
    enum ANIM_STATE
    {
        ACTIVATION,
        IDLE1,
        IDLE2,
        END
    };

public:
    typedef struct tagDoorGearDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }DOOR_GEAR_DESC;

private:
    CDoor_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDoor_Gear(const CDoor_Gear& Prototype);
    virtual ~CDoor_Gear() = default;

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
    static CDoor_Gear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END