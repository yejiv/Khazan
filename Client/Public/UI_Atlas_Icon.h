#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Atlas_Icon final : public CUI_Panel
{
public:
	typedef struct UI_Atlas_Icon_tag : public CUIObject::UIOBJECT_DESC
	{
		_int iTexPass = {};
		_int iShaderPass = {};
		_float4 vUV = {};
	}UIATLASICON_DESC;
private:
	CUI_Atlas_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Atlas_Icon(const CUI_Atlas_Icon& Prototype);
	virtual ~CUI_Atlas_Icon() = default;

public:
	void					Set_LocalPos(_float2 vPos, CUIObject* pParent);
	void					Set_Pos(_float2 vPos);
	void					Set_Texture(_float4 vUV, _uint iTexPass);
	void					Set_Shader(_uint iShaderPass);
	void					Update_Color(_float4 vColor);

	_float2					Get_LocalPos() { return m_vLocalPos; }

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