#pragma once
#include "UI_Button.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Default_Button final : public CUI_Button
{
public:
	typedef struct UI_Atlas_Icon_tag : public CUIObject::UIOBJECT_DESC
	{
		_int iTexPass = {};
		_int iShaderPass = {};
		_float4 vUV = {};
	}UIATLASICON_DESC;
private:
	CUI_Default_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Default_Button(const CUI_Default_Button& Prototype);
	virtual ~CUI_Default_Button() = default;

public:
	void						Set_Texture(_float4 vUV, _uint iTexPass);
	void						Set_Shader(_uint iShaderPass);
	void						Update_Color(_float4 vColor);

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
    _bool                       m_isOver = {};

public:
	static CUI_Default_Button*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END