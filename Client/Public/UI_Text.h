#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_Text abstract : public CUIObject
{
protected:
	CUI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Text(const CUI_Text& Prototype);
	virtual ~CUI_Text() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	_bool						m_bIsTextBox = { false };
	_int						m_iTextAlign = {};
	_float						m_fMaxWidth = {};
	_float						m_fOffsetHeight = {};
	_int						m_iPivotX = {};
	_int						m_iPivotY = {};
	_wstring					m_wstrTexttag = {};
	_wstring					m_wstrText = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END