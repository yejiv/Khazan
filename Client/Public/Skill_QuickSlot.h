#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CSkill_QuickSlot final : public CUI_Panel
{
public:
    typedef struct Skill_Info {
        _int iSkillIndex;
    }SKILLQUICK_DESC;

    typedef struct Skill_Bubble_Tag : public CUIObject::BUBBLEEVENT{
        _int iSkillIndex;
        _int iIndex;
    }BUBBLE_DESC;
private:
    enum class UIANIMSTATE { ON, OFF, END };

private:
    CSkill_QuickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkill_QuickSlot(const CSkill_QuickSlot& Prototype);
    virtual ~CSkill_QuickSlot() = default;

public:
    void                                Equip_Check(_int iSkillIndex);
    void								On_Panel();
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
    class CUI_BackGround*               m_pBackGround = { nullptr };

    _float								m_fAccTime = {};
    UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };

    vector<class CSkill_QuickSlot_List*> m_pList;
    class CUI_TextBox*                  m_pSkillName = { nullptr };

    _int                                m_iSkillIndex = {};
private:
    HRESULT								Ready_Prototype();
    HRESULT								Ready_Object();

    void								UI_Animation(_float fTimeDelta);
    void								InputKey();
public:
    static CSkill_QuickSlot*               Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void						Free() override;
};

NS_END