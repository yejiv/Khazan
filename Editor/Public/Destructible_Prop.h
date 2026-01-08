#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CDestructible_Prop final : public CProp_Interactive
{
public:
    enum class MODEL_TYPE { FENCE, POT, BARREL, END};

    typedef struct tagDestructiblePropDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        MODEL_TYPE eModelType{ MODEL_TYPE::FENCE };

    }DESTRUCTIBLE_PROP_DESC;

private:
    CDestructible_Prop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDestructible_Prop(const CDestructible_Prop& Prototype);
    virtual ~CDestructible_Prop() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    MODEL_TYPE Get_Destructible_ModelType() { return m_eModelType; }
    void Set_Destructible_ModelType(MODEL_TYPE eModelType);

    string Get_Destructible_ModelType_ByString();

private:
    MODEL_TYPE m_eModelType = { MODEL_TYPE::END };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

public:
    static CDestructible_Prop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END