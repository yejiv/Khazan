#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CTutorial_Panel final : public CUI_Panel
{
private:
	CTutorial_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorial_Panel(const CTutorial_Panel& Prototype);
	virtual ~CTutorial_Panel() = default;

public:
	void								Setting_Panel(GUIDE_TYPE eType, _int iPage);

public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:
	class CUI_Guide_Icon*				m_pKeyIcon_1 = { nullptr };
	class CUI_Guide_Icon*				m_pKeyIcon_2 = { nullptr };

	class CUI_TextBox*					m_pText = { nullptr };

private:
	HRESULT								Ready_Child();

	void								Text_Set(GUIDE_TYPE eType, _int iPage);
public:
	static CTutorial_Panel*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END