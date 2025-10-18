#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_Button abstract : public Engine::CUIObject
{
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

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID) override;

protected:
	class CClientInstance* m_pClientInstance = { nullptr };

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END