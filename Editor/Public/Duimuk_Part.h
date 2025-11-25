#pragma once

#include "Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

class CDuimuk_Part final : public CPartObject
{
private:
    enum class ANIM_STATE { IDLE2, END };

public:
    typedef struct tagDuimukPartDesc : public CPartObject::PARTOBJECT_DESC
    {
        LEVEL eLevel{ LEVEL::END };

        _float4x4* pSocketMatrix{ nullptr };

        _bool* pStateIdle2 = { nullptr };

    }DUIMUK_PART_DESC;

private:
    CDuimuk_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDuimuk_Part(const CDuimuk_Part& Prototype);
    virtual ~CDuimuk_Part() = default;

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

    _bool* m_pStateIdle2 = { nullptr };

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);

public:
    static CDuimuk_Part* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

NS_END