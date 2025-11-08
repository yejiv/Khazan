#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CSkill_Panel final : public CUI_Panel
{
private:
	CSkill_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Panel(const CSkill_Panel& Prototype);
	virtual ~CSkill_Panel() = default;


public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	class CSkill_Gauge*					m_pGauge = { nullptr };
	class CUI_TextBox*					m_pPoint = { nullptr };
	class CUI_TextBox*					m_pLevel = { nullptr };
	class CUI_TextBox*					m_pExp = { nullptr };

	const _uint*						m_iLevel = { nullptr };
	const _uint*						m_iPoint = { nullptr };
	const _float*						m_fExp	 = { nullptr };
public:
	static CSkill_Panel*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END