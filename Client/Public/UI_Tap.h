#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_TextBox abstract : public CUIObject
{
public:
	typedef struct tagTextBoxDesc : CUIObject::UIOBJECT_DESC
	{
		_wstring					strText;
		_wstring					strFontTag;
		_float3						vColor;
		_float2						vFontScale;
		_float						fAlpha;

	}TEXTBOX_DESC;


protected:
	CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TextBox(const CUI_TextBox& Prototype);
	virtual ~CUI_TextBox() = default;

public:
	_wstring					Get_Text() const { return m_strText; }
	_wstring					Get_FontTag() const { return m_strFontTag; }
	_float3						Get_FontColor() const { return m_vFontColor; }
	_float						Get_FontAlpha() const { return m_fAlpha; }
	_float2						Get_FontScale() const { return m_vFontScale; }

	void						Set_Text(const _wstring& strText);
	void						Set_FontColor(const _float3& vColor);
	void						Set_FontTag(const _wstring& strFontTag);
	void						Set_FontScale(const _float2& vScale);
	void						Set_FontAlpha(_float fAlpha);
	void						Set_Centered(UI_ALIGNMENT eAligment);



public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;


public:
	void						Recalculate_TextInfo();


protected:
	_wstring					m_strText = {};
	_wstring					m_strFontTag = {};
	_float3						m_vFontColor = {};
	_float2						m_vFontScale = {};
	_float2						m_vFontPos = {};
	_float						m_fAlpha = {};
	_bool						m_isChange = false;

	_float2						m_vFontOffset = {};

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END