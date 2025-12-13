#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"
#include "UI_Announce_Result.h"
NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Announce_Symbol final : public CUI_Panel
{
private:
    CUI_Announce_Symbol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Announce_Symbol(const CUI_Announce_Symbol& Prototype);
    virtual ~CUI_Announce_Symbol() = default;

public:
    void                            Set_Dissovle(_bool isDissovel);
    void                            Set_SubTexScalling(_bool isScaling);
    void                            Set_Elite(_bool isElite);
public:
    virtual HRESULT					Initialize_Prototype();
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;
    virtual HRESULT					Render() override;
private:

    CShader*                        m_pShaderCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };
    CTexture*                       m_pDissolveTextureCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };

    _float							m_fDissolveTime = {};
    _bool							m_isDissovle = {};
    
    _bool                           m_isScaling = {};
    _float							m_fScalingSize = {};
    _float                          m_fSetTexAlpha = {};

    class CUI_Default_Tex*          m_pMainText = { nullptr };
    class CUI_Default_Tex*          m_pSubText = { nullptr };

    _bool                           m_isElite = { false };
private:
    HRESULT							Ready_Component();
    HRESULT                         Ready_Children();
public:
    static CUI_Announce_Symbol*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;
};
NS_END
