#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CElevator_Gear final : public CPartObject
{
private:
    enum ANIM_STATE { DONE, IDLE, LOOP, START, END};

public:
    typedef struct tagElevatorGearDesc : public CPartObject::PARTOBJECT_DESC
    {
        LEVEL eLevel{ LEVEL::END };

        _float fOffsetRotation{};
        _bool* pActive{ nullptr };

    }ELEVATOR_GEAR_DESC;

private:
    CElevator_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CElevator_Gear(const CElevator_Gear& Prototype);
    virtual ~CElevator_Gear() = default;

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

    ANIM_STATE m_eAnimState = ANIM_STATE::END;

    _float m_fOffsetRotation = {};
    _bool* m_pActive = { nullptr };

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);

public:
    static CElevator_Gear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

NS_END