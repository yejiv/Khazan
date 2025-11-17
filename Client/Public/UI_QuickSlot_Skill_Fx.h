#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_QuickSlot_Skill_Fx final : public CUI_Texture
{
private:
    enum class UIANIMSTATE { ON, OFF, END};
private:
    CUI_QuickSlot_Skill_Fx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_QuickSlot_Skill_Fx(const CUI_QuickSlot_Skill_Fx& Prototype);
    virtual ~CUI_QuickSlot_Skill_Fx() = default;

public:
    void                    On_Anim();
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
    UIANIMSTATE             m_eAnim = { UIANIMSTATE::END };
private:
    HRESULT					Ready_Component();

public:
    static CUI_QuickSlot_Skill_Fx* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END