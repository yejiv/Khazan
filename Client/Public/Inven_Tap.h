#pragma once
#include "UI_Tap.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CInven_Tap final : public CUI_Tap
{
private:
	CInven_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInven_Tap(const CInven_Tap& Prototype);
	virtual ~CInven_Tap() = default;

public:
	void								Tap_Enable();
	void								Tap_Disable();

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

public:
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

public:
	static CInven_Tap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END