#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CSkill_Table final : public CUI_Panel
{
private:
	CSkill_Table(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Table(const CSkill_Table& Prototype);
	virtual ~CSkill_Table() = default;


public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	_uint								m_iPreSkillLevel = { 0 };
	vector<class CUI_Atlas_Icon*>		m_pLine;
	const _uint*						m_iSkillLevel = { nullptr };

public:
	static CSkill_Table*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END