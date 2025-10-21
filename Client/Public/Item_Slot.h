#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CItem_Slot final : public CUI_Slot
{
private:
	enum class QUICKITMESLOTSTATE { NONITEM, DISABLE, ENABLE, END };

private:
	CItem_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Slot(const CItem_Slot& Prototype);
	virtual ~CItem_Slot() = default;
public:
	void								Input_Slot();

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	virtual	HRESULT						Ready_Prototype();
	virtual	HRESULT						Ready_Childer();

public:
	static CItem_Slot*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END