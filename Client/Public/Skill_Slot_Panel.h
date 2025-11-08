#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CSkill_Slot_Panel final : public CUI_Panel
{
private:
	CSkill_Slot_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Slot_Panel(const CSkill_Slot_Panel& Prototype);
	virtual ~CSkill_Slot_Panel() = default;

public:
	void								LocalPos_Set(_float2 vPos, CUIObject* pParent);
public:
	virtual HRESULT						Initialize_Prototype(_int iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:
	HRESULT								Ready_Prototype();
	HRESULT								Ready_Children();

public:
	static CSkill_Slot_Panel*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END