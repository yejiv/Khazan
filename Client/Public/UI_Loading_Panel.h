#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Loading_Panel final : public CUI_Panel
{
private:
	enum class UIANIM { LOAD, FINSH_START, FINSH, END};
private:
	CUI_Loading_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Loading_Panel(const CUI_Loading_Panel& Prototype);
	virtual ~CUI_Loading_Panel() = default;

public:
	void								Start_Load();
	void								Finsh_Load();
public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	_bool								m_isUpDelta = {};

	class CUI_Atlas_Icon*				m_pDecoL = { nullptr };
	class CUI_Atlas_Icon*				m_pDecoR = { nullptr };

	UIANIM								m_eAnimState = { UIANIM::END };
private:
	void								Update_DeltaAlpha(_float fTimeDelta);
public:
	static CUI_Loading_Panel*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END