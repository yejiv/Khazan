#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CDestinyGem final : public CPartObject
{
public:
    typedef struct tagDestinyGemDesc : public CPartObject::PARTOBJECT_DESC
    {
        _bool* pConsumed{ nullptr };
        _bool* pDissolved{ nullptr };

        LEVEL eLevel{ LEVEL::END };

    }DESTINYGEM_DESC;

private:
    CDestinyGem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDestinyGem(const CDestinyGem& Prototype);
    virtual ~CDestinyGem() = default;

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
    CTexture* m_pDissolveTextureCom = { nullptr };

    _bool* m_pConsumed = { nullptr };
    _bool* m_pDissolved = { nullptr };

    _float m_fTimeAcc = { 0.f };
    _float m_fDecreaseAlpha = { 0.f };

    _uint m_iNumGem = { 1 };

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_DissolveValues();
    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);

public:
    static CDestinyGem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};

NS_END