#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Announce_Over final : public CUI_Panel
{
private:
    enum class UIANIMSTATE {START, EFFECT_1, FINASH, END};
private:
	CUI_Announce_Over(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Announce_Over(const CUI_Announce_Over& Prototype);
	virtual ~CUI_Announce_Over() = default;

public:
	virtual HRESULT					Initialize_Prototype(_int iLevel);
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
private:
    class CUI_BackGround*           m_pBackGround = { nullptr };
    class CUI_Default_Tex*          m_pMainText = { nullptr };

    UIANIMSTATE                     m_eAnimState = { UIANIMSTATE::END };

    _float                          m_fAcctime = {};
    _float                          m_fDelaytime = {};

private:
	HRESULT							Ready_Prototype();
    HRESULT							Ready_Children();

    void                            Start_Anim();

public:
	static CUI_Announce_Over*	    Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
