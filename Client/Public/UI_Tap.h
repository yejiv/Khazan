#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_Tap abstract : public CUIObject
{
public:
	enum class STATE { DISABLE, ENABLE, OVER, SELETE, END };
protected:
	CUI_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Tap(const CUI_Tap& Prototype);
	virtual ~CUI_Tap() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID) override;

protected:
	std::function<void()>		m_ButtonEvent[ENUM_CLASS(STATE::END)];

	STATE						m_eState = { STATE::END };
public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END