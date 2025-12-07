#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_BladeNexus final : public CUI_Panel
{
public:
	enum class UIANIMSTATE { ON, OFF, END };
	enum class ONTYPE { DEFAULT = 2, EMBARS = 3, END};
	enum class MENULIST { STATE, WARP, CREVICE, END };

	typedef struct tagMainBubbleEventTag : public CUIObject::BUBBLEEVENT
	{
		MENULIST	eListType = {};
		_bool		isClick = { false };
	}MAINMENUBUBBLE_DESC;

	typedef struct tagBladeNexusON
	{
		ONTYPE eType;
		_wstring strMapName;
	}BLADENEXUS_ON_DESC;

private:
	CUI_BladeNexus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BladeNexus(const CUI_BladeNexus& Prototype);
	virtual ~CUI_BladeNexus() = default;

public:
	void								On_Panel(ONTYPE eType, _wstring strMapName);
	void								Off_Panel();
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual void						Bubble_EventCall(BUBBLEEVENT* pArg) override;
	virtual	HRESULT						Update_Switch(void* pArg);

private:
	class CUI_BackGround*				m_pBackGround = { nullptr };

	vector<class CBladeNexus_List*>		m_pList;
	vector<class CUI_TextBox*>			m_pText;

	vector<	class CUI_TextBox*>			m_pGuideText;
	vector<	class CUI_Atlas_Icon*>		m_pGuideIcon;


	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
	MENULIST							m_eNextEvent = { MENULIST::END };

	_float								m_fAccTime = {};

	_int								m_iSeleteIndex = {};

	_int								m_iListeType = {};
    _bool                               m_isTalk = {};
private:
	virtual	HRESULT						Ready_Prototype();
	HRESULT								Ready_Object();

	void								UI_Animation(_float fTimeDelta);
	void								Next_Event();
public:
	static CUI_BladeNexus*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END