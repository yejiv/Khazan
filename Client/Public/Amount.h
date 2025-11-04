#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CAmount final : public CUI_Panel
{
public:
	enum class UIANIMSTATE { ON, OFF, END };
	enum class AMOUNT_TYPE {GOLD, LACHRYMA, STONE, END};
private:
	CAmount(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAmount(const CAmount& Prototype);
	virtual ~CAmount() = default;

public:
	void								Add_Value(AMOUNT_TYPE eType, _int IValue);
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
	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
	_float								m_fAccTime = {};
	vector<class CAmount_Panel*>		m_pAmount;

	_bool								m_isAddValue = {};
	_int								m_iGold = {};
	_int								m_iLachryma = {};
	_int								m_iStone = {};

private:
	HRESULT								Ready_Prototype();
	void								UI_Animation(_float fTimeDelta);

public:
	static CAmount*						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END