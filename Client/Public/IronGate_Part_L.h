#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CIronGate_Part_L final : public CPartObject
{
public:
    typedef struct tagIronGatePartLeftDesc : public CPartObject::PARTOBJECT_DESC
    {
        LEVEL eLevel{ LEVEL::END };

        _float4x4* pSocketMatrix = { nullptr };

    }IRONGATE_PART_LEFT_DESC;

private:
    CIronGate_Part_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CIronGate_Part_L(const CIronGate_Part_L& Prototype);
    virtual ~CIronGate_Part_L() = default;

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

private:
    _float4x4* m_pSocketMatrix = { nullptr };

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);

public:
    static CIronGate_Part_L* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

NS_END