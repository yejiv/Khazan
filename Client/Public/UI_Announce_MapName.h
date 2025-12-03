#pragma once
#include "UI_Text.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Client)
class CUI_Announce_MapName final : public CUI_Text
{
public:
	enum class MAP_TYPE { HEINMACH, DEFAULT, END };
private:
	CUI_Announce_MapName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Announce_MapName(const CUI_Announce_MapName& Prototype);
	virtual ~CUI_Announce_MapName() = default;

public:
	virtual HRESULT					Initialize_Prototype(_int iLevel);
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pMaskTextureCom = { nullptr };
	CTexture*						m_pDissolveTextureCom = { nullptr };

	_float							m_fAccTime = {};
	_float							m_fFadeDuration = {};
	_float							m_fFadeTime = {};
	_float							m_fDissolveTime = {};

	_float							m_fTexAspect = {};
	_float							m_fDissovleAspect = {};

	_bool							m_isDissovle = {};

    MAP_TYPE                        m_eAnnounce_Type = { MAP_TYPE ::END};

    _int                            m_iDissolveTexPass = {};
private:
	HRESULT							Ready_Prototype();
	HRESULT							Ready_Component();

	void							Setting_Text(const EVENT_ANNOUNCE_MAPNAME& e);
public:
	static CUI_Announce_MapName*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
