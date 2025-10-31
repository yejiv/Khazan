#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_State_MainPanel final : public CUI_Panel
{
private:
	CUI_State_MainPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_State_MainPanel(const CUI_State_MainPanel& Prototype);
	virtual ~CUI_State_MainPanel() = default;

public:
	HRESULT								Setting_PanelLevel(_int iPanelType, UI_PLAYER_STATE_DATA* Data);

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	_int								m_iPanelType = { -1 };
	unordered_map<string, 
		class CUI_TextBox*>				m_pValue_Text;

	unordered_map<string,
		class CUI_State_Panel*>			m_pState_Panel;

	UI_PLAYER_STATE_DATA*				m_pData;

private:
	HRESULT								Mapping_Child();

public:
	static CUI_State_MainPanel*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};
NS_END
