#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUI_Panel abstract : public CUIObject
{
public:
	enum class PANEL_TYPE { VERTICAL, HORIZONTAL, END };

protected:
	CUI_Panel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CUI_Panel(const CUI_Panel& Prototype);
	virtual ~CUI_Panel() = default;

public:
	void						Set_PanelType(PANEL_TYPE eType) { m_eType = eType; }
	void						Set_Space(_float fSpace) { m_fSpace = fSpace; }
	void						Set_Allign(_bool isAllign) { m_isAlign = isAllign; }


public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						Update_LayOut();


protected:
	PANEL_TYPE					m_eType = { PANEL_TYPE::END };
	_float						m_fSpace = {};
	_bool						m_isAlign = { false };

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END