#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_Panel abstract : public CUIObject
{
protected:
	CUI_Panel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CUI_Panel(const CUI_Panel& Prototype);
	virtual ~CUI_Panel() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END