#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_State final : public CUI_Panel
{
public:
	enum class UI_TYPE {DEFAULT, UPAGERD, END };
	enum class UIANIMSTATE { ON, OFF, END };
	enum class STATE_LIST { VITALITY, ENDURANCE, POWER, WILL, COMPETENCY, END };
	enum class STATE_PANEL {LEVEL, LACHRYMA, DEFAULT_STATE, ADD_STATE, ELEMENTAL, END};


	typedef struct UI_StateONDesc
	{
		UI_TYPE eType;
	}UI_STATE_ON;

	typedef struct BubbleDesc : public CUIObject::BUBBLEEVENT
	{
		_bool		isClick = {};
		_bool		isUp = {};
		_bool		isList = {};
		STATE_LIST	eListType = {};
	}UI_STATE_BUBLLE;
private:
	CUI_State(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_State(const CUI_State& Prototype);
	virtual ~CUI_State() = default;

public:
	void								On_Panel(UI_TYPE eType);
	void								Off_Panel();
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual void						Bubble_EventCall(BUBBLEEVENT* pArg);
	virtual	HRESULT						Update_Switch(void* pArg);

private:
	class CUI_BackGround*				m_pBackGround = { nullptr };
	class CUI_TextBox*					m_pTitle = { nullptr };
	class CUI_State_List*				m_pUpButton = { nullptr };
	vector<class CUI_State_List*>		m_pState;
	vector<class CUI_State_MainPanel*>	m_pPanel;

	class CUI_TextBox*					m_pGuide_Text = { nullptr };
	class CUI_Atlas_Icon*				m_pGuide_Icon = { nullptr };

	UI_TYPE								m_eType = { UI_TYPE::END };
	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
	_float								m_fAccTime = {};

	vector<_uint*>				        m_CulStateLevel;
	vector<_int>						m_UpStateLevel;

	UI_PLAYER_STATE_DATA				m_Player_Data;
	UI_PLAYER_STATE_DATA				m_UpPlayer_Data;

    _uint*                              m_pLachryma = { nullptr };
private:
	virtual	HRESULT						Ready_Prototype();
	HRESULT								Ready_Object();
	void								Ready_PlayerData();
	HRESULT								Ready_UISetting();
	void								UI_Animation(_float fTimeDelta);

	void								List_Bubble_Event(UI_STATE_BUBLLE* pDesc);
	void								Button_Bubble_Event(UI_STATE_BUBLLE* pDesc);

    void                                Update_Data();

    void                                Setting_Data();
public:
	static CUI_State*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END