#pragma once
#include "UIParent.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_Tap abstract : public CUIParent
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

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

protected:
	vector<std::function<void()>>	m_Events;

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END