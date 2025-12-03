#include "UI_SkillTree.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_BackGround.h"
#include "UI_Default_Button.h"
#include "UI_TextBox.h"
#include "UI_Guide_Icon.h"
#include "UI_Default_Tex.h"

#include "Skill_Tap.h"
#include "Skill_Table.h"
#include "Skill_Panel.h"
#include "Skill_Gauge.h"
#include "Skill_Slot_Panel.h"
#include "Skill_Slot_Flag.h"
#include "Popup_Reset.h"

#include "Cursor_Circle_Fx.h"
#include "Skill_BG_Smoke.h"

CUI_SkillTree::CUI_SkillTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_SkillTree::CUI_SkillTree(const CUI_SkillTree& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_SkillTree::On_Panel()
{
	if (m_IsUpdate)
		return;

	m_eAnimState = UIANIMSTATE::ON;
	m_fAccTime = 0.5f;
	m_IsUpdate = true;

	m_iSelete = 0;
	for (_int i = 0; i < TAP_TYPE::END; ++i)
	{
		if (m_iSelete == i)
			m_SkillTap[i]->Set_Selete(true);
		else
			m_SkillTap[i]->Set_Selete(false);
	}

    m_pGameInstance->PlaySoundOnce(TEXT("UI_skill_open (SFX).wav"));

	m_pGameInstance->Emit_Event<EVENT_SKILL_OPEN>(ENUM_CLASS(EVENT_TYPE::SKILL_EVENT), EVENT_SKILL_OPEN{ });
}

void CUI_SkillTree::Off_Panel()
{
	if (!m_IsUpdate)
		return;

    m_IsUpdate = false;

    CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("MainMeun"));

    m_pGameInstance->StopByKey(TEXT("UI_mainmenu_open_renew (SFX).wav"));
    m_pGameInstance->PlaySoundOnce(TEXT("UI_skill_close (SFX).wav"));

}

HRESULT CUI_SkillTree::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_SkillTree::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

	return S_OK;
}

void CUI_SkillTree::Priority_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CUI_SkillTree::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	InputKey();
	UI_Animation(fTimeDelta);
	__super::Update(fTimeDelta);
}

void CUI_SkillTree::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_SkillTree::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	m_SkillTap.resize(TAP_TYPE::END);

	for (auto pChild : m_Children)
	{
		string strName = pChild->Get_Name();

		if (strName == "Spear")
		{
			m_SkillTap[TAP_TYPE::SPEAR] = static_cast<CSkill_Tap*>(pChild);
			Safe_AddRef(pChild);
			m_SkillTap[TAP_TYPE::SPEAR]->Setting_Panel(TAP_TYPE::SPEAR);
		}
		else if (strName == "GS")
		{
			m_SkillTap[TAP_TYPE::GS] = static_cast<CSkill_Tap*>(pChild);
			Safe_AddRef(pChild);
			m_SkillTap[TAP_TYPE::GS]->Setting_Panel(TAP_TYPE::GS);
		}
		else if (strName == "Public")
		{
			m_SkillTap[TAP_TYPE::PUBLIC] = static_cast<CSkill_Tap*>(pChild);
			Safe_AddRef(pChild);
			m_SkillTap[TAP_TYPE::PUBLIC]->Setting_Panel(TAP_TYPE::PUBLIC);
		}
	}

	CHECK_FAILED(Ready_Object(), E_FAIL);
	return S_OK;
}

void CUI_SkillTree::Bubble_EventCall(BUBBLEEVENT* pArg)
{
	SKillBUBBLE_DESC* pDesc = static_cast<SKillBUBBLE_DESC*>(pArg);

	for (_int i = 0; i < TAP_TYPE::END; ++i)
	{
		if (pDesc->iIndex == i)
			m_SkillTap[i]->Set_Selete(true);
		else
			m_SkillTap[i]->Set_Selete(false);
	}
}

HRESULT CUI_SkillTree::Update_Switch(void* pArg)
{
	return S_OK;
}

HRESULT CUI_SkillTree::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_Tap"),
		CSkill_Tap::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_Table"),
		CSkill_Table::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_Panel"),
		CSkill_Panel::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_Gauge"),
		CSkill_Gauge::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_SlotPanel"),
		CSkill_Slot_Panel::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_Flag"),
        CSkill_Slot_Flag::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Flag"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Flag/T_SkillTreeBg_0%d.png"), 2)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Cursor_Circle_Fx"),
        CCursor_Circle_Fx::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_BG_Smoke"),
        CSkill_BG_Smoke::Create(m_pDevice, m_pContext)), E_FAIL);


	return S_OK;
}

HRESULT CUI_SkillTree::Ready_Object()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 5.4f;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "BackGround";
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
	if (m_pBackGround == nullptr)
		return E_FAIL;
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::DEFAULT);
	m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 1.f });
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

    Desc.fDepth = 5.3f;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "BG_SMOKE";
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

    m_pBG_Smoke = static_cast<CSkill_BG_Smoke*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Skill_BG_Smoke"), &Desc));
    if (m_pBG_Smoke == nullptr)
        return E_FAIL;
    m_Children.push_back(m_pBG_Smoke);
    Safe_AddRef(m_pBG_Smoke);

    


    Desc.fDepth = 0.5f;
    Desc.szName = "Cursor_Fx";
    Desc.vLocalSize = { 96.f, 96.f };
    Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

    m_pCircle_Fx = static_cast<CCursor_Circle_Fx*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Cursor_Circle_Fx"), &Desc));
    if (m_pCircle_Fx == nullptr)
        return E_FAIL;
    m_Children.push_back(m_pCircle_Fx);
    Safe_AddRef(m_pCircle_Fx);

    
	return S_OK;

}

void CUI_SkillTree::UI_Animation(_float fTimeDelta)
{
	if (m_eAnimState == UIANIMSTATE::ON)
	{
		m_fAccTime += fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime >= 1.f)
		{
			m_fAccTime = 1.f;
			m_eAnimState = UIANIMSTATE::END;
		}
	}
	else if (m_eAnimState == UIANIMSTATE::OFF)
	{
		m_fAccTime -= fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime <= 0.f)
		{
			m_fAccTime = 0.f;
			m_eAnimState = UIANIMSTATE::END;
			m_IsUpdate = false;
		}
	}
}

void CUI_SkillTree::InputKey()
{
	if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::UI))
		Off_Panel();

	_bool isInput = false;
    if (m_pGameInstance->Key_Down(DIK_LCONTROL, INPUT_TYPE::UI))
    {
        _int iLevel = CClientInstance::GetInstance()->Get_PlayerData().iSkillLevel;
        _int iCulPoint = CClientInstance::GetInstance()->Get_PlayerData().iSkilPoint;

        if ((iLevel - iCulPoint) > 0)
        {
            CPopup_Reset::POPUP_RESET_DESC Desc;
            Desc.Event = [this]() {
                m_pGameInstance->Emit_Event<EVENT_SKILL_RESET>(ENUM_CLASS(EVENT_TYPE::SKILL_RESET), {});
                m_pGameInstance->PlaySoundOnce(TEXT("UI_skill_reset (SFX).wav"));
                };
            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("Popup_Reset"), &Desc);
        }
        else
            m_pGameInstance->Emit_Event< EVENT_ANNOUNCE_WARNING>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_WARNING), { TEXT("획득한 스킬이 없습니다.") });
    }
    else if (m_pGameInstance->Key_Down(DIK_E, INPUT_TYPE::UI))
	{
		isInput = true;
		++m_iSelete;

		if (m_iSelete >= TAP_TYPE::END)
			m_iSelete = 0;
	}
	else if (m_pGameInstance->Key_Down(DIK_Q, INPUT_TYPE::UI))
	{
		isInput = true;
		--m_iSelete;

		if (m_iSelete < 0)
			m_iSelete = TAP_TYPE::END - 1;
	}

	if(isInput)
		for (_int i = 0; i < TAP_TYPE::END; ++i)
		{
			if (m_iSelete == i)
				m_SkillTap[i]->Set_Selete(true);
			else
				m_SkillTap[i]->Set_Selete(false);
		}
}

CUI_SkillTree* CUI_SkillTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_SkillTree* pInstance = new CUI_SkillTree(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_SkillTree"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SkillTree::Clone(void* pArg)
{
	CUI_SkillTree* pInstance = new CUI_SkillTree(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_SkillTree"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_SkillTree::Free()
{
	__super::Free();

	Safe_Release(m_pBackGround);
    Safe_Release(m_pCircle_Fx);
    Safe_Release(m_pBG_Smoke);

	for (auto pTap : m_SkillTap)
		Safe_Release(pTap);
	m_SkillTap.clear();

}
