#pragma once
#include "UI_Text.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Announce_Talk final : public CUI_Text
{
private:
	enum class UIAnimeStae { ON, SHOW, OFF, END};
public:
	void		ShowUI(_int iTalkIndex);

private:
	CUI_Announce_Talk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Announce_Talk(const CUI_Announce_Talk& Prototype);
	virtual ~CUI_Announce_Talk() = default;

public:
	virtual HRESULT					Initialize_Prototype(_int iLevel);
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	CShader*						m_pTextShaderCom = { nullptr };

	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Rect*					m_pVIBufferCom = { nullptr };

	_float							m_fAccTime = {};

	_bool							m_isOneLine = {};

	_wstring						m_wstrTalkName = {};
	_wstring						m_wstrText_1 = {};
	_wstring						m_wstrText_2 = {};

	_float4							m_vNameColor = {};
	UIAnimeStae						m_eState = { UIAnimeStae::END};

    _wstring                        m_wstrSoundName = {};
    _int                            m_iNextIndex = {};

private:
	void							NameColor();
	HRESULT							Ready_Prototype();
	HRESULT							Ready_Component();

	void							Setting_Text(const EVENT_ANNOUNCE_MAPNAME& e);

	void							BG_RenderSetting();
	HRESULT							Font_TextOneLine();
	HRESULT							Font_TextTwoLine();
	HRESULT							BG_Render();

public:
	static CUI_Announce_Talk*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
