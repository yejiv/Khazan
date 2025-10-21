#pragma once
#include "UI_Panel.h"

NS_BEGIN(Client)
class CItem_Icon final : public CUI_Panel
{
private:
	CItem_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Icon(const CItem_Icon& Prototype);
	virtual ~CItem_Icon() = default;

public:
	virtual HRESULT				Initialize_Prototype(_uint iLevel);
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

private:
	virtual	HRESULT				Ready_Prototype();
	virtual	HRESULT				Ready_Childer();

public:
	static CItem_Icon*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};
NS_END
