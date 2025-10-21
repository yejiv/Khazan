#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_QuickSlot_Skill_Panel final : public CUI_Panel
{
private:
	CUI_QuickSlot_Skill_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QuickSlot_Skill_Panel(const CUI_QuickSlot_Skill_Panel& Prototype);
	virtual ~CUI_QuickSlot_Skill_Panel() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

private:
	virtual	HRESULT						Ready_Prototype();

public:
	static CUI_QuickSlot_Skill_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END