#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Inven final : public CUI_Panel
{
private:
	CUI_Inven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Inven(const CUI_Inven& Prototype);
	virtual ~CUI_Inven() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual void						Bubble_EventCall();

private:
	class CUI_BackGround*				m_pBackGround = { nullptr };

private:
	virtual	HRESULT						Ready_Prototype();
	HRESULT								Ready_Object();


public:
	static CUI_Inven*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END