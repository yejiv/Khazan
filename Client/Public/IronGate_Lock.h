#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CIronGate_Lock final : public CPartObject
{
private:
    enum class ANIM_STATE { ACTIVATION, IDLE1, IDLE2, END };

public:
    typedef struct tagIronGateLockDesc : public CPartObject::PARTOBJECT_DESC
    {
        LEVEL eLevel{ LEVEL::END };

        _float4x4* pSocketMatrix{ nullptr };

        _bool* pUnLock{ nullptr };

    }IRONGATE_LOCK_DESC;

private:
    CIronGate_Lock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CIronGate_Lock(const CIronGate_Lock& Prototype);
    virtual ~CIronGate_Lock() = default;

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

    ANIM_STATE m_eAnimState = { ANIM_STATE::END };

private:
    _float4x4* m_pSocketMatrix = { nullptr };
    _matrix m_pTempSocketMatrix = {};

    _bool* m_pUnLock = { nullptr };

    // Blink Rim Light Shader
    _float m_fBlinkTimeAcc = {};

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);
    HRESULT Bind_Blink_ShaderResources();

public:
    static CIronGate_Lock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

NS_END