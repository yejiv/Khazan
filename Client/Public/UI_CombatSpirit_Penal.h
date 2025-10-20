#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_CombatSpirit_Penal final : public CUI_Panel
{
private:
	CUI_CombatSpirit_Penal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CombatSpirit_Penal(const CUI_CombatSpirit_Penal& Prototype);
	virtual ~CUI_CombatSpirit_Penal() = default;

public:
	virtual HRESULT					Initialize_Prototype(_uint iLevel);
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	virtual void					Bubble_EventCall();

private:
	virtual	HRESULT					Ready_Prototype();
	virtual HRESULT					Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	vector<class CUI_ComBatSpirit_Slot*>	m_pSlot;
	_int									m_iMaxSlotNum = {};
	_int									m_iCulSlotNum = {};
	_float									m_fOffsetX = {};

	_float									m_fCulGaugeValue = {};
public:
	static CUI_CombatSpirit_Penal*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END