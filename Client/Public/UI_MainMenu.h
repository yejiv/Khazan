#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_MainMenu final : public CUI_Panel
{
public:
	enum class UIANIMSTATE { ON, OFF, END };
	enum class MENULIST { WEAPON, OTHER, SKILL, STATE, OPTION, TITLE, EXIT, END };

	typedef struct tagMainBubbleEventTag : public CUIObject::BUBBLEEVENT
	{
		MENULIST	eListType = {};
		_bool		isClick = { false };
	}MAINMENUBUBBLE_DESC;

private:
	CUI_MainMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MainMenu(const CUI_MainMenu& Prototype);
	virtual ~CUI_MainMenu() = default;

public:
	void								On_Panel();
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

	vector<class CMainMenu_List*>		m_pList;

	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
	MENULIST							m_eNextEvent = { MENULIST::END};

	_float								m_fAccTime = {};

	_int								m_iSeleteIndex = {};
private:
	virtual	HRESULT						Ready_Prototype();
	HRESULT								Ready_Object();

	void								UI_Animation(_float fTimeDelta);
	void								Next_Event();
public:
	static CUI_MainMenu*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END