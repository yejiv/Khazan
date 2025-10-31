#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_Text abstract : public CUIObject
{
public:
	typedef struct tagUITextBox
	{
		_bool						bIsTextBox = { false };
		TEXT_ALIGN					eTextAlign = {};
		_float						fMaxWidth = {};
		_float						fOffsetHeight = {};
		_int						iPivotX = {};
		_int						iPivotY = {};
		_wstring					wstrTexttag = {};
		_wstring					wstrText = {};
		_float4						vColor = {};

	}TEXTBOX_DESC;
protected:
	CUI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Text(const CUI_Text& Prototype);
	virtual ~CUI_Text() = default;

public:
	void						Set_Text(_wstring wstrText);
	void						Setting_Text(const TEXTBOX_DESC& pDesc);
	void						Setting_Pivot(_int iPivotX, _int iPivotY);
public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

protected:
	_bool						m_bIsTextBox = { false };
	TEXT_ALIGN					m_eTextAlign = {};
	_float						m_fMaxWidth = {};
	_float						m_fOffsetHeight = {};
	_int						m_iPivotX = {};
	_int						m_iPivotY = {};
	_wstring					m_wstrTexttag = {};
	_wstring					m_wstrText = {};

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END