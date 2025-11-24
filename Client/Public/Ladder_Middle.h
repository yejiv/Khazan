#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CLadder_Middle final : public CPartObject
{
public:
    typedef struct tagLadderMiddleDesc : public CPartObject::PARTOBJECT_DESC
    {
        LEVEL eLevel{ LEVEL::END };

        _float fOffSetHeight = {};

    }LADDER_MIDDLE_DESC;

private:
    CLadder_Middle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLadder_Middle(const CPartObject& Prototype);
    virtual ~CLadder_Middle() = default;

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
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);

public:
    static CLadder_Middle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

NS_END