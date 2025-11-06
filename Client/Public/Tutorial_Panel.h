#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CTutorial_Panel final : public CUI_Panel
{
private:
	CTutorial_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorial_Panel(const CTutorial_Panel& Prototype);
	virtual ~CTutorial_Panel() = default;

public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

public:
	static CTutorial_Panel*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END