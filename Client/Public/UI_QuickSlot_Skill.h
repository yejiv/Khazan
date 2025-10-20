#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_QuickSlot_Skill final : public CUI_Slot
{
private:
	enum class QUICKITMESLOTSTATE { NONITEM, DISABLE, ENABLE, END };

private:
	CUI_QuickSlot_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QuickSlot_Skill(const CUI_QuickSlot_Skill& Prototype);
	virtual ~CUI_QuickSlot_Skill() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual void						Bubble_EventCall();

private:
	virtual	HRESULT						Ready_Prototype();

public:
	static CUI_QuickSlot_Skill*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END