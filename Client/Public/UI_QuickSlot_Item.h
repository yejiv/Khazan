#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_QuickSlot_Item final : public CUI_Slot
{
private:
	enum class QUICKITMESLOTSTATE { NONITEM, DISABLE, ENABLE, END };

private:
	CUI_QuickSlot_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QuickSlot_Item(const CUI_QuickSlot_Item& Prototype);
	virtual ~CUI_QuickSlot_Item() = default;
public:
	void								Input_Slot();

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual void						Bubble_EventCall();
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	_int								m_iItemIndex = { -1};
	_int								m_iItemValue = { 0 };
	_float4								m_vFxColor = {};

	class CUI_Atlas_Icon*				m_pDisableFX = { nullptr };

private:
	virtual	HRESULT						Ready_Prototype();
	virtual	HRESULT						Ready_Childer();
	void								Update_State();
	void								Update_DisableFX(_float fTimeDelta);
public:
	static CUI_QuickSlot_Item*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END