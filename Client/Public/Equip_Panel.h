#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CEquip_Panel final : public CUI_Panel
{
private:
	CEquip_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEquip_Panel(const CEquip_Panel& Prototype);
	virtual ~CEquip_Panel() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	string								MapPing_Label(_int iIndex);
public:
	static CEquip_Panel*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END