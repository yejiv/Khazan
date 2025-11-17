#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CSkill_Slot final :public CUI_Slot
{
public:
	typedef struct UI_Skill_Slot_tag : public CUIObject::UIOBJECT_DESC
	{
		_int iSkillIndex = {};
	}UISKILLSLOT_DESC;
private:
	CSkill_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Slot(const CSkill_Slot& Prototype);
	virtual ~CSkill_Slot() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

private:
	class CUI_Atlas_Icon*				m_pLine = { nullptr };
	class CUI_Atlas_Icon*				m_pIcon = { nullptr };
	vector<CUI_Atlas_Icon*>				m_pPreSkillLine;
    
    class CUI_Atlas_Icon*               m_pPointBG = { nullptr };
    class CUI_TextBox*                  m_pSkillPointText = { nullptr };

	_int								m_iSkillIndex = { -1 };
	_int								m_iSkillPoint = { };

	_bool								m_isLock = {};
	const SKILL_DB*						m_pSkilData = {nullptr};
	_int								m_iLevel = {};

    _int                                m_iMaxCount = {};
    _wstring                            m_wstrMaxCount = {};
   
    _int                                m_iPreSkillIndex = { -1 };
    _bool                               m_isPreSkillOn = {};
private:
	HRESULT								Ready_Child(const SKILL_DB* pData);
	void								Setting_Skill();
    void                                On_PreSkill(const EVENT_SKILL_ON& e);
    void                                Render_SkillInfo();

    void                                Reset_Slot();
public:
	static CSkill_Slot*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END