#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CSkill_BG_Smoke final : public CUI_Panel
{
private:
    CSkill_BG_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkill_BG_Smoke(const CSkill_BG_Smoke& Prototype);
    virtual ~CSkill_BG_Smoke() = default;

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
private:
    HRESULT					Ready_Component();

public:
    static CSkill_BG_Smoke*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END