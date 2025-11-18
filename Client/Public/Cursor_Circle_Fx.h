#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CCursor_Circle_Fx final : public CUI_Panel
{
private:
    CCursor_Circle_Fx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCursor_Circle_Fx(const CCursor_Circle_Fx& Prototype);
    virtual ~CCursor_Circle_Fx() = default;

public:
    virtual HRESULT			Initialize_Prototype();
    virtual HRESULT			Initialize_Clone(void* pArg) override;
    virtual void			Priority_Update(_float fTimeDelta) override;
    virtual void			Update(_float fTimeDelta) override;
    virtual void			Late_Update(_float fTimeDelta) override;
    virtual HRESULT			Render() override;

private:
    CShader*                m_pShaderCom = { nullptr };
    CTexture*               m_pTextureCom = { nullptr };
    CVIBuffer_Rect*         m_pVIBufferCom = { nullptr };

    _float                  m_fAccTime = {};
    POINT                   m_CulPoint = {};
private:
    HRESULT					Ready_Component();

public:
    static CCursor_Circle_Fx*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END