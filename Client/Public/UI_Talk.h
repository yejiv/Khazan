#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Talk final : public CUI_Panel
{
private:
    CUI_Talk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Talk(const CUI_Talk& Prototype);
    virtual ~CUI_Talk() = default;

public:
    void                    Update_UITransform(_vector vPos);

public:
    virtual HRESULT			Initialize_Prototype() override;
    virtual HRESULT			Initialize_Clone(void* pArg) override;
    virtual void			Priority_Update(_float fTimeDelta) override;
    virtual void			Update(_float fTimeDelta) override;
    virtual void			Late_Update(_float fTimeDelta) override;
    virtual HRESULT			Render() override;

private:
    CShader*                m_pShaderCom = { nullptr };
    CTexture*               m_pTextureCom = { nullptr };
    CVIBuffer_Rect*         m_pVIBufferCom = { nullptr };

    class CUI_WorldTextBox* m_pText = { nullptr };
private:
    HRESULT					Ready_Component();
    HRESULT                 Ready_Children();
public:
    static CUI_Talk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END