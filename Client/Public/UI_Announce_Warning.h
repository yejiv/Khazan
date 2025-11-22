#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Annouce_Warning final : public CUI_Panel
{
private:
    enum class UIANIMSTATE { START, EFFECT_1, FINASH, END };
private:
    CUI_Annouce_Warning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Annouce_Warning(const CUI_Annouce_Warning& Prototype);
    virtual ~CUI_Annouce_Warning() = default;

public:
    virtual HRESULT					Initialize_Prototype(_int iLevel);
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;

private:
    class CUI_Default_Tex*          m_pBG = { nullptr };
    class CUI_TextBox*              m_pTextBox = { nullptr };

    UIANIMSTATE                     m_eAnimState = { UIANIMSTATE::END };

    _float                          m_fAcctime = {};
    _float                          m_fDelaytime = {};
private:
    HRESULT							Ready_Children();

    void                            Start_Anim(const EVENT_ANNOUNCE_WARNING& e);

public:
    static CUI_Annouce_Warning*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;
};
NS_END
