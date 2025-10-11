#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUI_Button abstract : public CUIObject
{
protected:
	enum class BUTTON_STATE { HOVERED, PRESSED, NORMAL };


protected:
	CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Button(const CUI_Button& Prototype);
	virtual ~CUI_Button() = default;


public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	_bool						IsPicked(HWND hWnd);
	_bool						Update_Picking(HWND hWnd);


protected:
	BUTTON_STATE				m_eState = { BUTTON_STATE::NORMAL };

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END