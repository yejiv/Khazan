#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Atlas_Icon final : public CUI_Texture
{
private:
	CUI_Atlas_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Atlas_Icon(const CUI_Atlas_Icon& Prototype);
	virtual ~CUI_Atlas_Icon() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	static CUI_Atlas_Icon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END