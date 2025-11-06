#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CSkill_Slot final :public CUI_Slot
{
private:
	CSkill_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Slot(const CSkill_Slot& Prototype);
	virtual ~CSkill_Slot() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

public:
	static CSkill_Slot*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END