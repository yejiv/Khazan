#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_SkillTree final : public CUI_Panel
{
private:
	enum class UIANIMSTATE { ON, OFF, END};
	enum TAP_TYPE { SPEAR, GS, PUBLIC, END };

private:
	CUI_SkillTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillTree(const CUI_SkillTree& Prototype);
	virtual ~CUI_SkillTree() = default;

public:
void									On_Panel(GUIDE_TYPE eType);
	void								Off_Panel();
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual void						Bubble_EventCall(BUBBLEEVENT* pArg) override;
	virtual	HRESULT						Update_Switch(void* pArg);
private:
	class CUI_BackGround*				m_pBackGround = { nullptr };
	vector<class CSkill_Tap*>			m_SkillTap;
	
	_float								m_fAccTime = {};
	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };

private:
	HRESULT								Ready_Prototype();
	HRESULT								Ready_Object();

	void								UI_Animation(_float fTimeDelta);

public:
	static CUI_SkillTree*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END