#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CIronGate_Part_R final : public CPartObject
{
public:
    typedef struct tagIronGatePartRightDesc : public CPartObject::PARTOBJECT_DESC
    {
        LEVEL eLevel{ LEVEL::END };

        _float4x4* pSocketMatrix = { nullptr };

    }IRONGATE_PART_RIGHT_DESC;

private:
    CIronGate_Part_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CIronGate_Part_R(const CIronGate_Part_R& Prototype);
    virtual ~CIronGate_Part_R() = default;

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

    // Blink Rim Light Shader
    _float m_fBlinkTimeAcc = {};
    _bool m_isEnableBlink = {};

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);
    HRESULT Bind_Blink_ShaderResources();

public:
    static CIronGate_Part_R* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

NS_END