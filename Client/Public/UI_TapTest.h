#pragma once
#include "UI_Tap.h"

NS_BEGIN(Client)
class CUI_TapTest final : public CUI_Tap
{
private:
	CUI_TapTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TapTest(const CUI_TapTest& Prototype);
	virtual ~CUI_TapTest() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	static CUI_TapTest*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END