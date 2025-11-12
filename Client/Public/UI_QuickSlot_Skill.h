#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_QuickSlot_Skill final : public CUI_Slot
{
private:
	enum class QUICKITMESLOTSTATE { NONITEM, DISABLE, ENABLE, END };

private:
	CUI_QuickSlot_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QuickSlot_Skill(const CUI_QuickSlot_Skill& Prototype);
	virtual ~CUI_QuickSlot_Skill() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

    virtual HRESULT					    Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
    _int                                m_iIndex = {};
    _int                                m_iSkillIndex = { -1};
    class CUI_Atlas_Icon*               m_pIcon = { nullptr };

    const _float*                             m_pGauge = { nullptr };
private:
    HRESULT     						Ready_Children();
    void                                Skill_Equip(const EVENT_SKILL_SLOT& e);
    void								Update_State();
public:
	static CUI_QuickSlot_Skill*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END