#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_Button abstract : public Engine::CUIObject
{
public:
	enum class STATE { DISABLE, ENABLE, OVER, SELETE, END };

protected:
	CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Button(const CUI_Button& Prototype);
	virtual ~CUI_Button() = default;

public:
	void						Set_State(STATE eState) { m_eState = eState; }

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

protected:
	virtual _bool				ButtonOver(HWND hWnd);
	virtual _bool				ButtonClick(HWND hWnd, _bool IsRight, _bool IsDown, INPUT_TYPE eType = INPUT_TYPE::UI);

	virtual void				ButtonEvent(STATE eState);
protected:
	vector<std::function<void()>>	m_Events;
	STATE							m_eState;
public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END