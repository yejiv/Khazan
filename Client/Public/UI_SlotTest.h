#pragma once
#include "UI_Slot.h"

NS_BEGIN(Client)
class CUI_SlotTest final : public CUI_Slot
{
private:
	CUI_SlotTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SlotTest(const CUI_SlotTest& Prototype);
	virtual ~CUI_SlotTest() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	static CUI_SlotTest*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END