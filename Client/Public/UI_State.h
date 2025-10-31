#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_State final : public CUI_Panel
{
public:
	enum class UIANIMSTATE { ON, OFF, END };
	enum class STATE_LIST { VITALITY, ENDURANCE, POWER, WILL, COMPETENCY, END };
	enum class STATE_PANEL {LEVEL, LACHRYMA, DEFAULT_STATE, ADD_STATE, ELEMENTAL, END};
private:
	CUI_State(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_State(const CUI_State& Prototype);
	virtual ~CUI_State() = default;

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
	virtual	HRESULT						Update_Switch(void* pArg);

private:
	class CUI_BackGround*				m_pBackGround = { nullptr };
	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
	_float								m_fAccTime = {};

	class CUI_TextBox*					m_pTitle = { nullptr };
	class CUI_State_List*				m_pUpButton = { nullptr };

	vector<class CUI_State_List*>		m_pState;
	vector<class CUI_State_Panel*>		m_pPanel;

private:
	virtual	HRESULT						Ready_Prototype();
	HRESULT								Ready_Object();

	void								UI_Animation(_float fTimeDelta);

public:
	static CUI_State*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END