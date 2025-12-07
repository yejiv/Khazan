#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CDoor_Gear final : public CPartObject
{
private:
    enum MESH_TYPE { GEAR, RUNE };

    enum ANIM_STATE
    {
        ACTIVATION,
        IDLE1,
        IDLE2,
        END
    };

public:
    typedef struct tagDoorGearDesc : public CPartObject::PARTOBJECT_DESC
    {
        LEVEL eLevel{ LEVEL::END };

        _float4x4* pSocketMatrix{ nullptr };

        _bool* pUnLock{ nullptr };
        _int iEventID{};

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

private:
    CModel* m_pModelCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };

    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE1 };

    _float4x4* m_pSocketMatrix = { nullptr };

    _bool* m_pUnLock = { nullptr };

    _int m_iEventID = {};

    EVENT_TYPE m_eEventType = { EVENT_TYPE::END };

    EventGateGear m_EventGate = {};

    _uint m_iEventLisID = { 0 };

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);

    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

public:
    static CDoor_Gear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END