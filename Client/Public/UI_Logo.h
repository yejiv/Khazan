#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Logo final : public CUI_Panel
{
public:
	enum class UISTATE { START, START_END, MENU, END};
	enum class LISTTYPE { NEWGAME, OPTION, EXIT, END};

	typedef struct tagMainBubbleEventTag : public CUIObject::BUBBLEEVENT
	{
		LISTTYPE	eListType = {};
		_bool		isClick = { false };
	}LOGOBUBBLE_DESC;
private:
	CUI_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Logo(const CUI_Logo& Prototype);
	virtual ~CUI_Logo() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual void						Bubble_EventCall(BUBBLEEVENT* pArg) override;
	virtual	HRESULT						Update_Switch(void* pArg);

private:
	UISTATE								m_eState = { UISTATE::END};
	
	class CUI_TextBox*					m_pStartText = { nullptr };
	vector<class CUI_Logo_List*>		m_pList = {};
	
	_bool								m_isUpDelta = {};
	_float								m_fAccTime = {};
	_int								m_iSeleteIndex = {};
	
	LISTTYPE							m_eNextEvent = { LISTTYPE::END };

	_bool								m_isClick = { false };
	
private:
	virtual	HRESULT						Ready_Prototype();

	void								UI_Animation(_float fTimeDelta);

	void								Update_StartState(_float fTimeDelta);
	void								Update_MenuState(_float fTimeDelta);

	void								Event_NewGame();
public:
	static CUI_Logo*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END