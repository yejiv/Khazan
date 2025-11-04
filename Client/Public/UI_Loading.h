#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Loading final : public CUI_Panel
{
private:
	CUI_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading(const CUI_Loading& Prototype);
	virtual ~CUI_Loading() = default;

public:
	void								On_Panel();
	void								Off_Panel();
	void								Finsh_UI();

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	class CUI_TextBox*					m_pInfoName = { nullptr };
	class CUI_TextBox*					m_pInfoText = { nullptr };
	class CUI_Loading_Panel*			m_pIcon = { nullptr };
	class CUI_Atlas_Icon*				m_pPointIcon = { nullptr };
	class CUI_BackGround*				m_pBackGround = { nullptr };
	_int								m_iSelete = {};
private:
	HRESULT								Ready_Prototype();
	HRESULT								Ready_Object();
	
	void								Input_Update();
	void								Update_Text();
public:
	static CUI_Loading*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END